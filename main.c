#include "SDL2-2.26.5/x86_64-w64-mingw32/include/SDL2/SDL_events.h"
#include "SDL2-2.26.5/x86_64-w64-mingw32/include/SDL2/SDL_keyboard.h"
#include "include/button.h"
#include "include/result.h"
#include <stdbool.h>
#include <stdio.h>
#define SDL_MAIN_HANDLED
#include "SDL2-2.26.5/x86_64-w64-mingw32/include/SDL2/SDL.h"
#include "SDL2-2.26.5/x86_64-w64-mingw32/include/SDL2/SDL_syswm.h"
#include "cairo-windows-1.17.2/include/cairo-ft.h"
#include "cairo-windows-1.17.2/include/cairo.h"
#include "freetype/freetype/freetype.h"
#include "freetype/ft2build.h"
#include "include/rocket_render.h"
#include "log-boii/log_boii.h"

SDL_Surface *sdl_surface = NULL;
cairo_surface_t *cr_surface = NULL;
cairo_t *cr = NULL;
button_widget *button = NULL;

FT_Library value;
FT_Face face;

void free_resources(SDL_Window *window);

/// Animators
void button_widget__animator(button_widget *button) {
  button_widget__set_x(button, button_widget__get_x(button).value + 1);
  button_widget__set_y(button, button_widget__get_y(button).value + 1);
}

cairo_surface_t *load_scaled_image(const char *image_file_path,
                                   const float scale_factor) {
  if (!image_file_path) {
    return NULL;
  }

  cairo_surface_t *image = cairo_image_surface_create_from_png(image_file_path);
  if (!image) {
    log_error("Unable to load image!\n");
    return NULL;
  }
  int w = cairo_image_surface_get_width(image);
  int h = cairo_image_surface_get_height(image);
  int target_w = scale_factor * w;
  int target_h = scale_factor * h;

  cairo_surface_t *scaled_surface =
      cairo_image_surface_create(CAIRO_FORMAT_ARGB32, target_w, target_h);
  cairo_t *context = cairo_create(scaled_surface);

  cairo_scale(context, scale_factor, scale_factor);
  cairo_set_source_surface(context, image, 0, 0);
  cairo_paint(context);

  cairo_surface_destroy(image);
  cairo_destroy(context);

  return scaled_surface;
}

unsigned utf8_encode(char *utf8, uint32_t codepoint) {
  if (codepoint <= 0x7F) {
    // Plain ASCII
    utf8[0] = (char)codepoint;
    utf8[1] = 0;
    return 1;
  } else if (codepoint <= 0x07FF) {
    // 2-byte unicode
    utf8[0] = (char)(((codepoint >> 6) & 0x1F) | 0xC0);
    utf8[1] = (char)(((codepoint >> 0) & 0x3F) | 0x80);
    utf8[2] = 0;
    return 2;
  } else if (codepoint <= 0xFFFF) {
    // 3-byte unicode
    utf8[0] = (char)(((codepoint >> 12) & 0x0F) | 0xE0);
    utf8[1] = (char)(((codepoint >> 6) & 0x3F) | 0x80);
    utf8[2] = (char)(((codepoint >> 0) & 0x3F) | 0x80);
    utf8[3] = 0;
    return 3;
  } else if (codepoint <= 0x10FFFF) {
    // 4-byte unicode
    utf8[0] = (char)(((codepoint >> 18) & 0x07) | 0xF0);
    utf8[1] = (char)(((codepoint >> 12) & 0x3F) | 0x80);
    utf8[2] = (char)(((codepoint >> 6) & 0x3F) | 0x80);
    utf8[3] = (char)(((codepoint >> 0) & 0x3F) | 0x80);
    utf8[4] = 0;
    return 4;
  } else {
    // error - use replacement character
    utf8[0] = (char)0xEF;
    utf8[1] = (char)0xBF;
    utf8[2] = (char)0xBD;
    utf8[3] = 0;
    return 0;
  }
}

// Took from Lite-XL repository
// for recording the exact keypresses
// example: for remapped keys by the user
static const char *numpad[] = {"end",    "down",  "pagedown", "left",
                               "",       "right", "home",     "up",
                               "pageup", "ins",   "delete"};

static const char *get_key_name(const SDL_Event *e, char *buf) {
  SDL_Scancode scancode = e->key.keysym.scancode;
  /* Is the scancode from the keypad and the number-lock off?
  ** We assume that SDL_SCANCODE_KP_1 up to SDL_SCANCODE_KP_9 and
  *SDL_SCANCODE_KP_0
  ** and SDL_SCANCODE_KP_PERIOD are declared in SDL2 in that order. */
  if (scancode >= SDL_SCANCODE_KP_1 && scancode <= SDL_SCANCODE_KP_1 + 10 &&
      !(e->key.keysym.mod & KMOD_NUM)) {
    return numpad[scancode - SDL_SCANCODE_KP_1];
  } else {
    /* We need to correctly handle non-standard layouts such as dvorak.
       Therefore, if a Latin letter(code<128) is pressed in the current layout,
       then we transmit it as it is. But we also need to support shortcuts in
       other languages, so for non-Latin characters(code>128) we pass the
       scancode based name that matches the letter in the QWERTY layout.

       In SDL, the codes of all special buttons such as control, shift, arrows
       and others, are masked with SDLK_SCANCODE_MASK, which moves them outside
       the unicode range (>0x10FFFF). Users can remap these buttons, so we need
       to return the correct name, not scancode based. */
    if ((e->key.keysym.sym < 128) || (e->key.keysym.sym & SDLK_SCANCODE_MASK))
      strcpy(buf, SDL_GetKeyName(e->key.keysym.sym));
    else
      strcpy(buf, SDL_GetScancodeName(scancode));
    char *temp = buf;
    while (*temp) {
      *temp = tolower(*temp);
      temp++;
    }
    return buf;
  }
}

void draw(SDL_Window *window) {
  // White background with SDL2 API
  SDL_FillRect(sdl_surface, NULL, SDL_MapRGB(sdl_surface->format, 16, 16, 16));

  rocket_render__rectangle_rounded(20, 20, 8, 100, 4,
                                   (SDL_Color){255, 0, 0, 164});

  double xc = 220.0;
  double yc = 240.0;
  double radius = 200.0;
  double angle1 = 45.0 * (M_PI / 180.0);
  double angle2 = 180.0 * (M_PI / 180.0);

  cairo_set_source_rgba(cr, 0, 0, 0, 1.0);
  cairo_set_line_width(cr, 10.0);
  cairo_arc(cr, xc, yc, radius, angle1, angle2);
  cairo_stroke(cr);

  cairo_set_source_rgba(cr, 1, 0.2, 0.2, 0.6);
  cairo_set_line_width(cr, 6.0);

  cairo_arc(cr, xc, yc, 10.0, 0, 2 * M_PI);
  cairo_fill(cr);

  cairo_arc(cr, xc, yc, radius, angle1, angle1);
  cairo_line_to(cr, xc, yc);
  cairo_arc(cr, xc, yc, radius, angle2, angle2);
  cairo_line_to(cr, xc, yc);
  cairo_stroke(cr);

  // cairo_font_options_t *font_options = cairo_font_options_create();
  // cairo_get_font_options(cr, font_options);
  // cairo_font_options_set_antialias(font_options, CAIRO_ANTIALIAS_SUBPIXEL);
  // cairo_font_options_set_subpixel_order(font_options,
  // CAIRO_SUBPIXEL_ORDER_RGB); cairo_set_font_options(cr, font_options);
  // cairo_font_options_destroy(font_options);

  const char text[] = "Hola!";

  cairo_text_extents_t text_extents;
  cairo_text_extents(cr, text, &text_extents);

  // cairo_select_font_face(cr, "JetBrainsMono Nerd Font",
  // CAIRO_FONT_SLANT_NORMAL,
  //                        CAIRO_FONT_WEIGHT_NORMAL);
  // cairo_set_font_size(cr, 24);

  const char *filename =
      "assets/fonts/JetBrains Mono Regular Nerd Font Complete.ttf";
  if (!FT_Init_FreeType(&value)) {
    log_error("Unable to init FT!");
  }
  if (!FT_New_Face(value, filename, 0, &face)) {
    log_error("Unable to load font!");
  }
  cairo_font_face_t *ct = cairo_ft_font_face_create_for_ft_face(face, 0);
  cairo_set_font_face(cr, ct);
  cairo_set_font_size(cr, 18);

  cairo_move_to(cr, 50, 50);
  cairo_move_to(cr, 50, 50 + text_extents.height);
  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_show_text(cr, text);

  cairo_surface_t *scaled_rocket =
      load_scaled_image("assets/images/rocket.png", 0.4);
  cairo_set_source_surface(cr, scaled_rocket, 200, 0);
  cairo_paint(cr);
  cairo_surface_destroy(scaled_rocket);
  cairo_set_source_surface(cr, cr_surface, 0, 0);

  if (button)
    button_widget__render(button);

  char unicode_text[4] = "\0\0\0\0";
  utf8_encode(unicode_text, 60199);
  cairo_move_to(cr, 100, 100);
  cairo_show_text(cr, unicode_text);

  for (unsigned i = 0; i < 4; i++)
    unicode_text[i] = '\0';
  utf8_encode(unicode_text, 60321);
  cairo_move_to(cr, 125, 100);
  cairo_show_text(cr, unicode_text);

  for (unsigned i = 0; i < 4; i++)
    unicode_text[i] = '\0';
  utf8_encode(unicode_text, 60341);
  cairo_move_to(cr, 150, 100);
  cairo_show_text(cr, unicode_text);

  for (unsigned i = 0; i < 4; i++)
    unicode_text[i] = '\0';
  utf8_encode(unicode_text, 62227);
  cairo_move_to(cr, 175, 100);
  cairo_show_text(cr, unicode_text);

  for (unsigned i = 0; i < 4; i++)
    unicode_text[i] = '\0';
  utf8_encode(unicode_text, 58879);
  cairo_move_to(cr, 200, 100);
  cairo_show_text(cr, unicode_text);

  for (unsigned i = 0; i < 4; i++)
    unicode_text[i] = '\0';
  utf8_encode(unicode_text, 58878);
  cairo_move_to(cr, 225, 100);
  cairo_show_text(cr, unicode_text);

  SDL_BlitSurface(sdl_surface, NULL, SDL_GetWindowSurface(window), NULL);
  SDL_UpdateWindowSurface(window);
}

int main(int argc, char *argv[]) {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_DisplayMode dm;
  SDL_GetCurrentDisplayMode(0, &dm);

  SDL_Window *window = SDL_CreateWindow(
      "Rocket Racoon", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640,
      480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

  // Setting window icon
  SDL_Surface *rocket_icon = SDL_LoadBMP("assets/images/rocket.bmp");
  if (!rocket_icon) {
    log_error("Unable to load rocket.bmp icon!");
  }
  SDL_SetWindowIcon(window, rocket_icon);
  SDL_FreeSurface(rocket_icon);

  // Setting dark theme
#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
  SDL_SysWMinfo wmi;
  SDL_VERSION(&wmi.version);
  SDL_GetWindowWMInfo(window, &wmi);
  HWND hwnd = wmi.info.win.window;

  HMODULE uxtheme = LoadLibraryA("uxtheme.dll");
  HMODULE dwm = LoadLibraryA("dwmapi.dll");

  if (uxtheme && dwm) {
    typedef HRESULT (*SetWindowThemePTR)(HWND, const wchar_t *,
                                         const wchar_t *);
    SetWindowThemePTR SetWindowTheme =
        (SetWindowThemePTR)GetProcAddress(uxtheme, "SetWindowTheme");

    typedef HRESULT (*DwmSetWindowAttributePTR)(HWND, DWORD, LPCVOID, DWORD);
    DwmSetWindowAttributePTR DwmSetWindowAttribute =
        (DwmSetWindowAttributePTR)GetProcAddress(dwm, "DwmSetWindowAttribute");

    if (SetWindowTheme && DwmSetWindowAttribute) {
      SetWindowTheme(hwnd, L"DarkMode_Explorer", NULL);

      BOOL darkMode = 1;
      if (!DwmSetWindowAttribute(hwnd, 20, &darkMode, sizeof darkMode)) {
        DwmSetWindowAttribute(hwnd, 19, &darkMode, sizeof darkMode);
      }
    }
  }
#endif

  sdl_surface = SDL_GetWindowSurface(window);
  cr_surface = cairo_image_surface_create_for_data(
      (unsigned char *)sdl_surface->pixels, CAIRO_FORMAT_RGB24, sdl_surface->w,
      sdl_surface->h, sdl_surface->pitch);

  cairo_surface_set_device_scale(cr_surface, 1, 1);

  cr = cairo_create(cr_surface);
  //
  // printf("sdl_surface->w=%d\n"
  //        "sdl_surface->h=%d\n"
  //        "sdl_surface->pitch=%d\n",
  //        sdl_surface->w, sdl_surface->h, sdl_surface->pitch);
  // printf("sdl_surface->format->format=%s\n",
  //        SDL_GetPixelFormatName(sdl_surface->format->format));

  draw(window);

  result_button_widget_ptr _ = button_widget__new("Button");
  if (!_.ok) {
    log_error("Unable to create button!");
  }
  button = _.value;
  button_widget__set_x(button, 100);
  button_widget__set_y(button, 150);
  button_widget__set_background_color(button, (SDL_Color){64, 64, 64, 255});
  button_widget__set_animator(button, button_widget__animator);

  bool done = false, redraw = true;
  unsigned int wait_time = 500;

  while (!done) {
    double frame_start_time =
        SDL_GetPerformanceCounter() / (double)SDL_GetPerformanceFrequency();
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT: {
        log_info("Quit event occured.");
        done = true;
        free_resources(window);
        return 0;
      }
      case SDL_WINDOWEVENT: {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
          log_info("Window resized.");
          cairo_destroy(cr);
          cairo_surface_destroy(cr_surface);
          sdl_surface = SDL_GetWindowSurface(window);
          cr_surface = cairo_image_surface_create_for_data(
              (unsigned char *)sdl_surface->pixels, CAIRO_FORMAT_RGB24,
              sdl_surface->w, sdl_surface->h, sdl_surface->pitch);
          cairo_surface_set_device_scale(cr_surface, 1, 1);
          cr = cairo_create(cr_surface);
          log_debug("New dimensions: (%d, %d)", sdl_surface->w, sdl_surface->h);
          redraw = true;
        }
        break;
      }
      case SDL_KEYDOWN: {
        char buf[16];
        log_info("Key Pressed: %s", get_key_name(&event, buf));
        break;
      }
      default:
        break;
      }

      button_widget__process_sdl_event(button, event);
    }

    result_bool _ = button_widget__update(button);
    if (!_.ok) {
      log_error("%s", _.error);
      exit(1);
    }
    if (_.value) {
      redraw = true;
    }

    if (redraw) {
      draw(window);
    }

    if (!redraw) {
      SDL_WaitEventTimeout(NULL, wait_time);
    }

    if (redraw) {
      redraw = false;
    }

    double frame_end_time =
        SDL_GetPerformanceCounter() / (double)SDL_GetPerformanceFrequency();
    double time_elapsed = frame_end_time - frame_start_time;
    log_info("Frame time: %lf", time_elapsed);
    SDL_Delay((1 / (60 - time_elapsed)) * 1000);
  }

  free_resources(window);

  return 0;
}

void free_resources(SDL_Window *window) {
  button_widget__free(button);

  cairo_destroy(cr);
  cairo_surface_destroy(cr_surface);

  SDL_DestroyWindow(window);

  SDL_Quit();
}
