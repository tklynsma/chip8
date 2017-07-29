#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <fstream>
#include <stdio.h>
#include "chip8.h"

// Display:
const int SCALE = 10;
const SDL_Color COLOR_BLACK = { 0x00, 0x00, 0x00, 0xFF };
const SDL_Color COLOR_WHITE = { 0xFF, 0xFF, 0xFF, 0xFF };

void setRenderDrawColor(SDL_Renderer* renderer, SDL_Color c) {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
}

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

// Keypad:
const int KEYMAP[16] = {
    SDLK_x, SDLK_1, SDLK_2, SDLK_3, 
    SDLK_q, SDLK_w, SDLK_e, SDLK_a,
    SDLK_s, SDLK_d, SDLK_z, SDLK_c,
    SDLK_4, SDLK_r, SDLK_f, SDLK_v
};

// Timing:
const float MIN_CYCLES_PER_MS = 0.015625;
const int MAX_SPEED           = 10;

float cycles_per_ms;
int speed = 6;

// Sound:
const int SAMPLE_FREQUENCY = 22050;
const int TONE_FREQUENCY   = 960;
const int TONE_VOLUME      = 32;
const int SOUND_VOLUME     = 32;
const int MAX_SECONDS      = 5;

Uint8* audio_buffer = NULL;
Mix_Chunk beep;

// CPU:
Chip8 cpu;

bool parse_arguments(int argc, char *argv[]);   // Parse command line arguments.
bool initialize();                              // Start up SDL and create window.
bool load_rom(char *path);                      // Load the ROM.
void generate_sound();                          // Generate the sound samples.
void handle_event(SDL_Event* event);            // Handle event.
void draw_display(SDL_Renderer* renderer);      // Draw the display.
void close();                                   // Destroy the window and quit SDL.

int main(int argc, char *argv[]) {
    // Parse command line arguments.
    if (argc < 2) {
        printf("Error: missing argument.\n");
        return 1;
    }

    // Initialize the chip8 cpu and load the ROM.
    cpu.initialize();
    if (!load_rom(argv[1])) {
        printf("Failed to load ROM.\n");
        return 1;
    }

    // Start up SDL and create window.
    if (!initialize()) {
        printf("Failed to initialize SDL.\n");
        return 1;
    }

    // Generate sound wave.
    generate_sound();

    // Initialize variables.
    cycles_per_ms = MIN_CYCLES_PER_MS * (1 << (speed - 1));
    unsigned int last_cycle_time = SDL_GetTicks();
    double last_update_time = SDL_GetTicks();
    float num_cycles = 0.0;
    int update_count = 0;

    // Run the emulator.
    SDL_Event event;
    while (true) {
        // Update the number of cycles to run.
        unsigned int current_time = SDL_GetTicks();
        num_cycles += (current_time - last_cycle_time) * cycles_per_ms;
        last_cycle_time = current_time;

        // Update the CPU.
        while (num_cycles >= 1) {
            // Handle events.
            while (SDL_PollEvent(&event) != 0) {
                if (event.type == SDL_QUIT) {
                    close();
                    return 0;
                }

                handle_event(&event);
            }
            
            // Update the CPU's timers at a rate of 60Hz.
            while (SDL_GetTicks() > last_update_time + 16.666667) {
                cpu.update_timers();
                last_update_time += 16.666667;
                update_count++;

                if (update_count == 60) {
                    last_update_time -= 0.00002;
                    update_count = 0;
                }
            }

            cpu.cycle();
            num_cycles -= 1;

            // Redraw the display.
            if (cpu.is_draw_flag()) {
                draw_display(renderer);
                cpu.reset_draw_flag();
            }

            // Play sound.
            if (cpu.is_sound_flag()) {
                beep.alen = (SAMPLE_FREQUENCY * cpu.get_sound_duration()) / 30;
                Mix_PlayChannel(-1, &beep, 0);
                cpu.reset_sound_flag();
            }
        }
    }

    return 0;
}

bool initialize() {
    // Initialize SDL.
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize. SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Create window.
    window = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, SCALE * DISPLAY_WIDTH, SCALE * DISPLAY_HEIGHT,
            SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created. SDL_ERROR: %s\n", SDL_GetError());
        return false;
    }

    // Create renderer.
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created. SDL_ERROR: %s\n", SDL_GetError());
        return false;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(SAMPLE_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 2048) < 0) {
        printf("SDL_mixer could not initialize. SDL_mixer Error: %s\n", Mix_GetError());
        return false;
    }

    return true;
}

bool load_rom(char *path) {
    std::ifstream file(path, std::ios::binary);

    if (file.is_open()) {
        // Get file size using the associated buffer object.
        std::filebuf* buffer = file.rdbuf();
        if (buffer == NULL) {
            return false;
        }
        size_t size = buffer->pubseekoff(0, file.end, file.in);
        buffer->pubseekpos(0, file.in);

        // Get file data.
        char* rom = new char[size];
        buffer->sgetn(rom, size);

        // Load the file into the cpu and close the file.
        cpu.load_rom(rom, size);
        file.close();

        return true;
    }

    return false;
}

void generate_sound() {
    int period = SAMPLE_FREQUENCY / TONE_FREQUENCY;
    int half_period = 0.5 * period;
    int num_samples = period * TONE_FREQUENCY * MAX_SECONDS;
    Uint32 num_bytes = 2 * num_samples;

    audio_buffer = new Uint8[num_bytes];

    // Generate square wave.
    for (int i = 0; i < num_samples; i++) {
        audio_buffer[2 * i]     = 0;
        audio_buffer[2 * i + 1] = (i / half_period) % 2 ? TONE_VOLUME : -TONE_VOLUME;
    }

    beep = {0, audio_buffer, num_bytes, SOUND_VOLUME};
}

void handle_event(SDL_Event* event) {
    switch (event->type) {
        case SDL_KEYDOWN:
            // Update CPU keypad.
            for (byte i = 0; i <= 0xF; i++) {
                if (event->key.keysym.sym == KEYMAP[i]) {
                    cpu.set_key(i, true);
                }
            }

            // Decrease the emulator speed if "-" is pressed.
            if (event->key.keysym.sym == SDLK_MINUS) {
                speed = speed > 1 ? speed - 1 : 1;
                cycles_per_ms = MIN_CYCLES_PER_MS * (1 << (speed - 1));
            }

            // Increase the emulator speed if "+" is pressed.
            else if (event->key.keysym.sym == SDLK_EQUALS) {
                speed = speed < MAX_SPEED ? speed + 1 : MAX_SPEED;
                cycles_per_ms = MIN_CYCLES_PER_MS * (1 << (speed - 1));
            }

            break;

        case SDL_KEYUP:
            // Update CPU keypad.
            for (byte i = 0; i <= 0xF; i++) {
                if (event->key.keysym.sym == KEYMAP[i]) {
                    cpu.set_key(i, false);
                }
            } break;
    }
}

void draw_display(SDL_Renderer* renderer) {
    // Clear the screen.
    setRenderDrawColor(renderer, COLOR_BLACK);
    SDL_RenderClear(renderer);

    // Draw all pixels on the display.
    setRenderDrawColor(renderer, COLOR_WHITE);
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
        for (int y = 0; y < DISPLAY_HEIGHT; y++) {
            if (cpu.is_pixel(x, y)) {
                SDL_Rect pixel = {SCALE * x, SCALE * y, SCALE, SCALE};
                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }

    // Present the result on screen.
    SDL_RenderPresent(renderer);
}

void close() {
    // Delete audio buffer.
    delete[] audio_buffer;

    // Delete window and renderer.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    renderer = NULL;
    window = NULL;

    // Quit SDL subsystems.
    Mix_Quit();
    SDL_Quit();
}
