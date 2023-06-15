#pragma once

#define SDL_MAIN_HANDLED
#include "../SDL2-2.26.5/x86_64-w64-mingw32/include/SDL2/SDL.h"
#include "result.h"

struct button_widget;
typedef struct button_widget button_widget;

DEFINE_RESULT_WITH_TYPE_AND_NAME(button_widget *, result_button_widget_ptr);
DEFINE_RESULT_WITH_TYPE_AND_NAME(int, result_int);
DEFINE_RESULT_WITH_TYPE_AND_NAME(bool, result_bool);
DEFINE_RESULT_WITH_TYPE_AND_NAME(unsigned int, result_unsigned_int);
DEFINE_RESULT_WITH_TYPE_AND_NAME(SDL_Color, result_sdl_color);
DEFINE_RESULT_WITH_TYPE_AND_NAME(const char *, result_const_char_ptr);

/// Constructor
result_button_widget_ptr button_widget__new(const char *text);

/// Destructor
result button_widget__free(button_widget *button);

/// Pack Function
result button_widget__pack(button_widget *button);

/// Event processor
result_bool button_widget__process_sdl_event(button_widget *button,
                                             const SDL_Event event);
result button_widget__set_animator(button_widget *button,
                                   void (*animator)(button_widget *));
result_bool button_widget__update(button_widget *button);

/// Render Function
result button_widget__render(button_widget *button);

/// Getters
result_int button_widget__get_x(button_widget *button);
result_int button_widget__get_y(button_widget *button);
result_unsigned_int button_widget__get_width(button_widget *button);
result_unsigned_int button_widget__get_height(button_widget *button);
result_unsigned_int button_widget__get_padding_x(button_widget *button);
result_unsigned_int button_widget__get_padding_y(button_widget *button);
result_sdl_color button_widget__get_foreground_color(button_widget *button);
result_sdl_color button_widget__get_background_color(button_widget *button);
result_const_char_ptr button_widget__get_text(button_widget *button);

/// Setters
result button_widget__set_x(button_widget *button, const int x);
result button_widget__set_y(button_widget *button, const int y);
// result button_widget__set_width(button_widget *button,
//                                 const unsigned int width);
// result button_widget__set_height(button_widget *button,
//                                  const unsigned int height);
result button_widget__set_padding_x(button_widget *button, const int padding_x);
result button_widget__set_padding_y(button_widget *button, const int padding_y);
result button_widget__set_foreground_color(button_widget *button,
                                           const SDL_Color color);
result button_widget__set_background_color(button_widget *button,
                                           const SDL_Color color);
result button_widget__set_text(button_widget *button, const char *text);