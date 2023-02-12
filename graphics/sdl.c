// For reference, see https://www.parallelrealities.co.uk/tutorials/shooter/shooter1.php

#include <SDL2/SDL.h>
#include "include/sdl.h"

struct Image {
	SDL_Renderer *renderer;
	SDL_Window *window;
};

void sdl_get_input(void) {
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

void sdl_prepare_scene(Image* app) {
	SDL_SetRenderDrawColor(app->renderer, 96, 128, 255, 255);
	SDL_RenderClear(app->renderer);
}

void sdl_present_scene(Image* app) {
	SDL_RenderPresent(app->renderer);
}

void sdl_do_delay(uint32_t delay) {
    SDL_Delay(delay);
}

void sdl_free(Image* app) {
    free(app);
}

Image* sdl_init(int width, int height) {
    // allocate memory for image app
    Image* app = malloc(sizeof(Image));
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

	app->window = SDL_CreateWindow("CHIP-8 interpreter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, windowFlags);
	if (!app->window) {
		printf("Failed to open %d x %d window: %s\n", width, height, SDL_GetError());
		exit(1); }

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	app->renderer = SDL_CreateRenderer(app->window, -1, rendererFlags);
	if (!app->renderer) {
		printf("Failed to create renderer: %s\n", SDL_GetError());
		exit(1);
	}

    return app;
}

// these are chip 8 related instruction functions

void sdl_instr_clear_screen(Image* app) {
	SDL_RenderClear(app->renderer);
}

void sdl_instr_draw_pixel(Image* app, int x, int y) {
    SDL_SetRenderDrawColor(app->renderer, 255, 0, 0, 255);
    for (uint8_t i = 0; i < 50; ++i)
        SDL_RenderDrawPoint(app->renderer, i, i);
	// SDL_RenderDrawPoint(app->renderer, x, y);
}
