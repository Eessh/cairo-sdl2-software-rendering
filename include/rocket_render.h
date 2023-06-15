#pragma once

#define SDL_MAIN_HANDLED
#include "../SDL2-2.26.5/x86_64-w64-mingw32/include/SDL2/SDL.h"
#include "../cairo-windows-1.17.2/include/cairo.h"

void rocket_render__rectangle_filled(const int x, const int y,
                                     const unsigned int width,
                                     const unsigned int height,
                                     const SDL_Color color);
void rocket_render__rectangle_rounded(const int x, const int y,
                                      const unsigned int width,
                                      const unsigned int height,
                                      const unsigned int radius,
                                      const SDL_Color color);
void rocket_render__rectangle_rounded_with_auto_corner_radius(
    const int x, const int y, const unsigned int width,
    const unsigned int height, const SDL_Color color);

void rocket_render__text(const int x, const int y, const char *text,
                         const SDL_Color color, const char *font_family,
                         const unsigned int font_size,
                         const cairo_font_slant_t font_style,
                         const cairo_font_weight_t font_weight);