#include <SDL2/SDL.h>
#include <fstream>
#include <stdio.h>
#include "chip8.h"

const int SCALE = 10;
const SDL_Color COLOR_BLACK = { 0x00, 0x00, 0x00, 0xFF };
const SDL_Color COLOR_WHITE = { 0xFF, 0xFF, 0xFF, 0xFF };

const float MIN_CYCLES_PER_MS = 0.015625;
const float MAX_CYCLES_PER_MS = 64.0;

const int KEYMAP[16] = {
    SDLK_x, SDLK_1, SDLK_2, SDLK_3, 
    SDLK_q, SDLK_w, SDLK_e, SDLK_a,
    SDLK_s, SDLK_d, SDLK_z, SDLK_c,
    SDLK_4, SDLK_r, SDLK_f, SDLK_v
};

unsigned int last_cycle_time  = 0;
unsigned int last_update_time = 0;
float cycles_per_ms           = 0.5;
float num_cycles              = 0.0;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
Chip8 cpu;

bool initialize();                         // Start up SDL and create window.
bool load_rom(char *path);                 // Load the ROM.
void handle_event(SDL_Event* event);       // Handle event.
void draw_display(SDL_Renderer* renderer); // Draw the display.
void close();                              // Destroy the window and quit SDL.

void setRenderDrawColor(SDL_Renderer* renderer, SDL_Color c) {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
}

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

    last_cycle_time = last_update_time = SDL_GetTicks();

    // Run the emulator.
    SDL_Event event;
    while (true) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                close();
                return 0;
            }

            handle_event(&event);
        }

        // Update the number of cycles to run.
        while (SDL_GetTicks() > last_cycle_time + 1) {
            num_cycles += cycles_per_ms;
            last_cycle_time += 1;
        }

        // Update the CPU's timers.
        while (SDL_GetTicks() > last_update_time + 17) {
            cpu.update_timers();
            last_update_time += 17;
        }

        // Update the CPU.
        while (num_cycles >= 1) {
            cpu.cycle();
            num_cycles -= 1;
        }

        // Redraw the display.
        if (cpu.is_draw_flag()) {
            draw_display(renderer);
            cpu.reset_draw_flag();
        }
    }

    return 0;
}

bool initialize() {
    // Initialize SDL.
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
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

void handle_event(SDL_Event* event) {
    switch (event->type) {
        case SDL_KEYDOWN:
            // Update CPU keypad.
            for (byte i = 0; i <= 0xF; i++) {
                if (event->key.keysym.sym == KEYMAP[i]) {
                    cpu.set_key(i, true);
                }
            }

            // Decrease the number of cycles per ms if "-" is pressed.
            if (event->key.keysym.sym == SDLK_MINUS) {
                cycles_per_ms /= 2;
                if (cycles_per_ms < MIN_CYCLES_PER_MS) {
                    cycles_per_ms = MIN_CYCLES_PER_MS;
                }
            }

            // Increase the number of cycles per ms if "+" is pressed.
            else if (event->key.keysym.sym == SDLK_EQUALS) {
                cycles_per_ms *= 2;
                if (cycles_per_ms > MAX_CYCLES_PER_MS) {
                    cycles_per_ms = MAX_CYCLES_PER_MS;
                }
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
    // Destroy window.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    renderer = NULL;
    window = NULL;

    // Quit SDL subsystems.
    SDL_Quit();
}
