// For reference, see
// https://www.parallelrealities.co.uk/tutorials/shooter/shooter1.php

#include <inttypes.h>
#include <SDL2/SDL.h>
#include "include/sdl.h"
#include "instruction.h"
#include "log.h"

struct Image
{
    SDL_Renderer* renderer;
    SDL_Window* window;
    uint8_t screen[IMAGE_WIDTH][IMAGE_HEIGHT];
};


void sdl_get_input(void)
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            exit(0);
            break;
        default:
            break;
        }
    }
}

void sdl_prepare_scene(Image* app)
{
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);
}

void sdl_present_scene(Image* app)
{
    SDL_RenderPresent(app->renderer);
}

void sdl_do_delay(uint32_t delay)
{
    SDL_Delay(delay);
}

void sdl_free(Image* app)
{
    free(app);
}

Image* sdl_init(int width, int height)
{
    // allocate memory for image app
    // make sure to initialize to 0 with calloc
    Image* app = calloc(1, sizeof(Image));
    if (!app) {
        fprintf(stderr, "Could not allocate memory for image!");
        exit(1);
    }

    int rendererFlags, windowFlags;
    rendererFlags = SDL_RENDERER_ACCELERATED;
    windowFlags = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    app->window = SDL_CreateWindow(
        "CHIP-8 interpreter",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        windowFlags);
    if (!app->window) {
        printf(
            "Failed to open %d x %d window: %s\n",
            width,
            height,
            SDL_GetError());
        exit(1);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    app->renderer = SDL_CreateRenderer(app->window, -1, rendererFlags);
    if (!app->renderer) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        exit(1);
    }

    return app;
}

// these are chip 8 related instruction functions

void sdl_instr_clear_screen(Image* app)
{
    SDL_RenderClear(app->renderer);
}

void sdl_instr_set_pixel(Image* app, int x, int y)
{
    if (SDL_SetRenderDrawColor(app->renderer, 255, 0, 0, 255) < 0) {
        LOGE("Could not set 1 color for pixel at (%d, %d).", x, y);
        exit(EXIT_FAILURE);
    }

    for (int x_i = 0; x_i < IMAGE_SCALE_FACTOR; x_i++) {
        int x_tmp = x + x_i;
        for (int y_i = 0; y_i < IMAGE_SCALE_FACTOR; y_i++) {
            int y_tmp = y + y_i;
            if (SDL_RenderDrawPoint(app->renderer, x_tmp, y_tmp) < 0) {
                LOGE("Could not draw active pixel at (%d, %d).", x_tmp, y_tmp);
                exit(EXIT_FAILURE);
            }
            app->screen[x_tmp][y_tmp] = 1;
        }
    }
}

void sdl_instr_unset_pixel(Image* app, int x, int y)
{
    if (SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255) < 0) {
        LOGE("Could not set 0 color for pixel at (%d, %d).", x, y);
        exit(EXIT_FAILURE);
    }
    
    for (int x_i = 0; x_i < IMAGE_SCALE_FACTOR; x_i++) {
        int x_tmp = x + x_i;
        for (int y_i = 0; y_i < IMAGE_SCALE_FACTOR; y_i++) {
            int y_tmp = y + y_i;
            if (SDL_RenderDrawPoint(app->renderer, x_tmp, y_tmp) < 0) {
                LOGE("Could not draw inactive pixel at (%d, %d).", x_tmp, y_tmp);
                exit(EXIT_FAILURE);
            }
            app->screen[x_tmp][y_tmp] = 0;
        }
    }
}

uint8_t sdl_instr_get_pixel(Image* app, int x, int y)
{
    // there is a function SDL_RenderReadPixels apparently,
    // but this seems to be a very slow function
    return app->screen[x][y];
}
