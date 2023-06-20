#include "../include/rocket_render.h"

extern cairo_t* cr;

void rocket_render__rectangle_filled(const int x,
                                     const int y,
                                     const unsigned int width,
                                     const unsigned int height,
                                     const SDL_Color color)
{
  cairo_set_source_rgba(cr,
                        (float)color.r / 255.0,
                        (float)color.g / 255.0,
                        (float)color.b / 255.0,
                        (float)color.a / 255.0);
  cairo_rectangle(cr, x, y, width, height);
  cairo_fill(cr);
}

void rocket_render__rectangle_rounded(const int x,
                                      const int y,
                                      const unsigned int width,
                                      const unsigned int height,
                                      const unsigned int radius,
                                      const SDL_Color color)
{
  cairo_set_source_rgba(cr,
                        (float)color.r / 255.0,
                        (float)color.g / 255.0,
                        (float)color.b / 255.0,
                        (float)color.a / 255.0);
  cairo_set_fill_rule(cr, CAIRO_FILL_RULE_EVEN_ODD);
  cairo_new_sub_path(cr);
  cairo_arc(cr, x + radius, y + radius, radius, M_PI, 3 * M_PI / 2);
  cairo_arc(cr, x + width - radius, y + radius, radius, 3 * M_PI / 2, 2 * M_PI);
  cairo_arc(cr, x + width - radius, y + height - radius, radius, 0, M_PI / 2);
  cairo_arc(cr, x + radius, y + height - radius, radius, M_PI / 2, M_PI);
  cairo_close_path(cr);
  cairo_fill(cr);
}

void rocket_render__rectangle_rounded_with_auto_corner_radius(
  const int x,
  const int y,
  const unsigned int width,
  const unsigned int height,
  const SDL_Color color)
{
  double aspect_ratio = (float)width / (float)height;
  double corner_radius = height / 10.0;
  double radius = corner_radius / aspect_ratio;
  // double degrees = M_PI / 180.0;
  //
  // cairo_set_line_width(cr, 0.0);
  //
  // cairo_new_sub_path(cr);
  // cairo_arc(cr, x + width - radius, y + radius, radius, -90 * degrees,
  //           0 * degrees);
  // cairo_arc(cr, x + width - radius, y + height - radius, radius, 0 * degrees,
  //           90 * degrees);
  // cairo_arc(cr, x + radius, y + height - radius, radius, 90 * degrees,
  //           180 * degrees);
  // cairo_arc(cr, x + radius, y + radius, radius, 180 * degrees, 270 *
  // degrees); cairo_close_path(cr);
  //
  cairo_set_source_rgba(cr,
                        (float)color.r / 255.0,
                        (float)color.g / 255.0,
                        (float)color.b / 255.0,
                        (float)color.a / 255.0);
  // cairo_fill_preserve(cr);
  // cairo_set_source_rgba (cr, 0.5, 0, 0, 0.5);
  // cairo_set_line_width (cr, 10.0);
  // cairo_stroke (cr);

  cairo_set_fill_rule(cr, CAIRO_FILL_RULE_EVEN_ODD);
  cairo_new_sub_path(cr);
  cairo_arc(cr, x + radius, y + radius, radius, M_PI, 3 * M_PI / 2);
  cairo_arc(cr, x + width - radius, y + radius, radius, 3 * M_PI / 2, 2 * M_PI);
  cairo_arc(cr, x + width - radius, y + height - radius, radius, 0, M_PI / 2);
  cairo_arc(cr, x + radius, y + height - radius, radius, M_PI / 2, M_PI);
  cairo_close_path(cr);
  cairo_fill(cr);
}

void rocket_render__text(const int x,
                         const int y,
                         const char* text,
                         const SDL_Color color,
                         const char* font_family,
                         const unsigned int font_size,
                         const cairo_font_slant_t font_style,
                         const cairo_font_weight_t font_weight)
{
  cairo_text_extents_t text_extents;
  cairo_text_extents(cr, text, &text_extents);

  cairo_select_font_face(cr, font_family, font_style, font_weight);
  cairo_set_font_size(cr, font_size);
  cairo_move_to(cr, x, y);
  cairo_move_to(cr, x, y + text_extents.height);
  cairo_set_source_rgba(cr,
                        (float)color.r / 255.0,
                        (float)color.g / 255.0,
                        (float)color.b / 255.0,
                        (float)color.a / 255.0);
  cairo_show_text(cr, text);
}
