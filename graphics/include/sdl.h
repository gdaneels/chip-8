#ifndef SDL_H
#define SDL_H

#include <stdint.h>

// make it an incomplete type so the details are hidden for the user
typedef struct Image Image;

Image* sdl_init(int width, int height);
void sdl_get_input(void);
void sdl_prepare_scene(Image* image);
void sdl_present_scene(Image* image);
void sdl_do_delay(uint32_t delay);
void sdl_free(Image* image);

// CHIP 8 related instructions

void sdl_instr_clear_screen(Image* image);
void sdl_instr_draw_pixel(Image* image, int x, int y);

#endif 
