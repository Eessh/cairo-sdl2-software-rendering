#include "include/button.h"
#include "include/result.h"
#include <stdbool.h>
#include <stdio.h>
#define SDL_MAIN_HANDLED
#include "SDL2-2.26.5/x86_64-w64-mingw32/include/SDL2/SDL.h"
#include "SDL2-2.26.5/x86_64-w64-mingw32/include/SDL2/SDL_syswm.h"
#include "cairo-windows-1.17.2/include/cairo.h"
#include "include/rocket_render.h"
#include "log-boii/log_boii.h"

SDL_Surface *sdl_surface = NULL;
cairo_surface_t *cr_surface = NULL;
cairo_t *cr = NULL;
button_widget *button = NULL;

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

void draw(SDL_Window *window) {
  // White background with SDL2 API
  SDL_FillRect(sdl_surface, NULL, SDL_MapRGB(sdl_surface->format, 16, 16, 16));

  // rocket_render__rectangle_filled(0, 0, sdl_surface->w, sdl_surface->h,
  //                                 (SDL_Color){255, 255, 255, 255});

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

  cairo_select_font_face(cr, "Fantasque Sans Mono", CAIRO_FONT_SLANT_NORMAL,
                         CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, 24);
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
      case SDL_QUIT:
        log_info("Quit event occured.");
        done = true;
        free_resources(window);
        return 0;
      case SDL_WINDOWEVENT:
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
