#include "../include/button.h"
#include "../cairo-windows-1.17.2/include/cairo.h"
#include "../include/rocket_render.h"
#include "../log-boii/log_boii.h"
#include <stdlib.h>
#include <string.h>

extern cairo_t *cr;

/// Struct Definition
struct button_widget {
  int x, y;
  unsigned int width, height;
  unsigned int padding_x, padding_y;

  SDL_Color foreground_color, background_color;

  char *text;

  /// Animation shit
  bool animation_happening;
  unsigned int animation_frames;
  void (*animator)(button_widget *button);
};

/// Constructor
result_button_widget_ptr button_widget__new(const char *text) {
  if (!text) {
    log_error("Cannot create a button with text pointed to NULL!");
    return (result_button_widget_ptr){
        .ok = false,
        .error = "Cannot create a button with text pointed to NULL!"};
  }

  button_widget *button = (button_widget *)calloc(1, sizeof(button_widget));
  if (!button) {
    log_error("Unable to allocate memory for button_widget!");
    return (result_button_widget_ptr){
        .ok = false, .error = "Unable to allocate memory for button_widget!"};
  }

  button->x = 0;
  button->y = 0;
  button->width = 0;
  button->height = 0;
  button->padding_x = 6;
  button->padding_y = 4;
  button->foreground_color = (SDL_Color){255, 255, 255, 255};
  button->background_color = (SDL_Color){0, 0, 0, 255};
  button->text = _strdup(text);

  button->animation_happening = false;
  button->animation_frames = 0;
  button->animator = NULL;

  cairo_text_extents_t text_extents;
  cairo_text_extents(cr, button->text, &text_extents);

  button->width = text_extents.width + 2 * button->padding_x;
  button->height = text_extents.height + 2 * button->padding_y;

  return (result_button_widget_ptr){.ok = true, .value = button};
}

/// Destructor
result button_widget__free(button_widget *button) {
  if (!button) {
    log_warn("Attempt to free a NULL pointing button_widget!");
    return (result){.ok = false,
                    .error = "Attempt to free a NULL pointing button_widget!"};
  }

  free(button->text);
  free(button);
  return (result){.ok = true};
}

/// Pack Function
result button_widget__pack(button_widget *button) {
  if (!button) {
    log_error("Cannot pack a button_widget pointing to NULL!");
    return (result){.ok = false,
                    .error = "Cannot pack a button_widget pointing to NULL!"};
  }

  cairo_text_extents_t text_extents;
  cairo_text_extents(cr, button->text, &text_extents);

  button->width = text_extents.width + 2 * button->padding_x;
  button->height = text_extents.height + 2 * button->padding_y;

  return (result){.ok = true};
}

result_bool button_widget__process_sdl_event(button_widget *button,
                                             const SDL_Event event) {
  if (!button) {
    log_error("Cannot process SDL_Event on a button_widget pointing to NULL!");
    return (result_bool){
        .ok = false,
        .error =
            "Cannot process SDL_Event on a button_widget pointing to NULL!"};
  }

  switch (event.type) {
  case SDL_KEYDOWN: {
    if (event.key.keysym.sym == SDLK_LEFT) {
      if (!button->animation_happening && button->animator) {
        button->animation_frames = 60;
        button->animation_happening = true;
      } else {
        break;
      }
    } else if (event.key.keysym.sym == SDLK_RIGHT) {
      break;
    } else {
      break;
    }

    return (result_bool){.ok = true, .value = true};
  }
  default:
    break;
  }

  return (result_bool){.ok = true, .value = false};
}

result button_widget__set_animator(button_widget *button,
                                   void (*animator)(button_widget *)) {
  if (!button) {
    log_error("Cannot set animator to button_widget pointing to NULL!");
    return (result){
        .ok = false,
        .error = "Cannot set animator to button_widget pointing to NULL!"};
  }

  if (!animator) {
    log_error("Cannot set NULL pointing animator to button_widget!");
    return (result){.ok = false,
                    .error =
                        "Cannot set NULL pointing animator to button_widget!"};
  }

  button->animator = animator;
  return (result){.ok = true};
}

result_bool button_widget__update(button_widget *button) {
  if (!button) {
    log_error("Cannot call update on button_widget pointing to NULL!");
    return (result_bool){
        .ok = false,
        .error = "Cannot call update on button_widget pointing to NULL!"};
  }

  if (!button->animation_happening) {
    return (result_bool){.ok = true, .value = false};
  }

  if (button->animation_frames == 0) {
    button->animation_happening = false;
    return (result_bool){.ok = true, .value = false};
  }

  button->animator(button);
  --button->animation_frames;

  return (result_bool){.ok = true, .value = true};
}

/// Render Function
result button_widget__render(button_widget *button) {
  if (!button) {
    log_error("Cannot render a button_widget pointing to NULL!");
    return (result){.ok = false,
                    .error = "Cannot render a button_widget pointing to NULL!"};
  }

  /// TODO: Render button
  rocket_render__rectangle_filled(button->x, button->y, button->width,
                                  button->height, button->background_color);
  rocket_render__text(button->x + button->padding_x,
                      button->y + button->padding_y, button->text,
                      button->foreground_color, "JetBrainsMono Nerd Font", 24,
                      CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);

  return (result){.ok = true};
}

/// Getters
result_int button_widget__get_x(button_widget *button) {
  if (!button) {
    log_error("Cannot get x-coordinate of a button_widget, pointing to NULL!");
    return (result_int){
        .ok = false,
        .error =
            "Cannot get x-coordinate of a button_widget, pointing to NULL!"};
  }

  return (result_int){.ok = true, .value = button->x};
}

result_int button_widget__get_y(button_widget *button) {
  if (!button) {
    log_error("Cannot get y-coordinate of a button_widget, pointing to NULL!");
    return (result_int){
        .ok = false,
        .error =
            "Cannot get y-coordinate of a button_widget, pointing to NULL!"};
  }

  return (result_int){.ok = true, .value = button->y};
}

result_unsigned_int button_widget__get_width(button_widget *button) {
  if (!button) {
    log_error("Cannot get width of a button_widget, pointing to NULL!");
    return (result_unsigned_int){
        .ok = false,
        .error = "Cannot get width of a button_widget, pointing to NULL!"};
  }

  return (result_unsigned_int){.ok = true, .value = button->width};
}

result_unsigned_int button_widget__get_height(button_widget *button) {
  if (!button) {
    log_error("Cannot get height of a button_widget, pointing to NULL!");
    return (result_unsigned_int){
        .ok = false,
        .error = "Cannot get height of a button_widget, pointing to NULL!"};
  }

  return (result_unsigned_int){.ok = true, .value = button->height};
}

result_unsigned_int button_widget__get_padding_x(button_widget *button) {
  if (!button) {
    log_error("Cannot get padding-x of a button_widget, pointing to NULL!");
    return (result_unsigned_int){
        .ok = false,
        .error = "Cannot get padding-x of a button_widget, pointing to NULL!"};
  }

  return (result_unsigned_int){.ok = true, .value = button->padding_x};
}

result_unsigned_int button_widget__get_padding_y(button_widget *button) {
  if (!button) {
    log_error("Cannot get padding-y of a button_widget, pointing to NULL!");
    return (result_unsigned_int){
        .ok = false,
        .error = "Cannot get padding-y of a button_widget, pointing to NULL!"};
  }

  return (result_unsigned_int){.ok = true, .value = button->padding_y};
}

result_sdl_color button_widget__get_foreground_color(button_widget *button) {
  if (!button) {
    log_error(
        "Cannot get foreground_color of a button_widget, pointing to NULL!");
    return (result_sdl_color){.ok = false,
                              .error = "Cannot get foreground_color of a "
                                       "button_widget, pointing to NULL!"};
  }

  return (result_sdl_color){.ok = true, .value = button->foreground_color};
}

result_sdl_color button_widget__get_background_color(button_widget *button) {
  if (!button) {
    log_error(
        "Cannot get background_color of a button_widget, pointing to NULL!");
    return (result_sdl_color){.ok = false,
                              .error = "Cannot get background_color of a "
                                       "button_widget, pointing to NULL!"};
  }

  return (result_sdl_color){.ok = true, .value = button->background_color};
}

result_const_char_ptr button_widget__get_text(button_widget *button) {
  if (!button) {
    log_error("Cannot get text of a button_widget, pointing to NULL!");
    return (result_const_char_ptr){
        .ok = false,
        .error = "Cannot get text of a button_widget, pointing to NULL!"};
  }

  return (result_const_char_ptr){.ok = true, .value = button->text};
}

/// Setters
result button_widget__set_x(button_widget *button, const int x) {
  if (!button) {
    log_error("Cannot set x-coordinate of a button_widget, pointing to NULL!");
    return (result){
        .ok = false,
        .error =
            "Cannot set x-coordinate of a button_widget, pointing to NULL!"};
  }

  button->x = x;
  return (result){.ok = true};
}

result button_widget__set_y(button_widget *button, const int y) {
  if (!button) {
    log_error("Cannot set y-coordinate of a button_widget, pointing to NULL!");
    return (result){
        .ok = false,
        .error =
            "Cannot set y-coordinate of a button_widget, pointing to NULL!"};
  }

  button->y = y;
  return (result){.ok = true};
}

// result button_widget__set_width(button_widget *button,
//                                 const unsigned int width) {
//   if (!button) {
//     log_error("Cannot set width of a button_widget, pointing to NULL!");
//     return (result){
//         .ok = false,
//         .error = "Cannot set width of a button_widget, pointing to NULL!"};
//   }

//   button->width = width;
//   return (result){.ok = true};
// }

// result button_widget__set_height(button_widget *button,
//                                  const unsigned int height) {
//   if (!button) {
//     log_error("Cannot set height of a button_widget, pointing to NULL!");
//     return (result){
//         .ok = false,
//         .error = "Cannot set height of a button_widget, pointing to NULL!"};
//   }

//   button->height = height;
//   return (result){.ok = true};
// }

result button_widget__set_padding_x(button_widget *button,
                                    const int padding_x) {
  if (!button) {
    log_error("Cannot set padding-x of a button_widget, pointing to NULL!");
    return (result){
        .ok = false,
        .error = "Cannot set padding-x of a button_widget, pointing to NULL!"};
  }

  button->padding_x = padding_x;
  return button_widget__pack(button);
  // return (result){.ok = true};
}

result button_widget__set_padding_y(button_widget *button,
                                    const int padding_y) {
  if (!button) {
    log_error("Cannot set padding-y of a button_widget, pointing to NULL!");
    return (result){
        .ok = false,
        .error = "Cannot set padding-y of a button_widget, pointing to NULL!"};
  }

  button->padding_y = padding_y;
  return button_widget__pack(button);
  // return (result){.ok = true};
}

result button_widget__set_foreground_color(button_widget *button,
                                           const SDL_Color color) {
  if (!button) {
    log_error(
        "Cannot set foreground_color of a button_widget, pointing to NULL!");
    return (result){.ok = false,
                    .error = "Cannot set foreground_color of a button_widget, "
                             "pointing to NULL!"};
  }

  button->foreground_color = color;
  return (result){.ok = true};
}

result button_widget__set_background_color(button_widget *button,
                                           const SDL_Color color) {
  if (!button) {
    log_error(
        "Cannot set background_color of a button_widget, pointing to NULL!");
    return (result){.ok = false,
                    .error = "Cannot set background_color of a button_widget, "
                             "pointing to NULL!"};
  }

  button->background_color = color;
  return (result){.ok = true};
}

result button_widget__set_text(button_widget *button, const char *text) {
  if (!button) {
    log_error("Cannot set text of a button_widget, pointing to NULL!");
    return (result){
        .ok = false,
        .error = "Cannot set text of a button_widget, pointing to NULL!"};
  }

  if (!text) {
    log_error("Cannot set text of button_widget, with text pointing to NULL!");
    return (result){
        .ok = false,
        .error =
            "Cannot set text of button_widget, with text pointing to NULL!"};
  }

  free(button->text);
  button->text = _strdup(text);
  return button_widget__pack(button);
  // return (result){.ok = true};
}
