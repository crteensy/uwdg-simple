#ifndef UWDG_STYLE_H
#define UWDG_STYLE_H

#include <gfx.h>

namespace uwdg
{
typedef color_t Color;

struct Style
{
  struct ColorSet
  {
    Color fill;
    Color text;
    Color border;
  };
  Color background;
  ColorSet inactive;
  ColorSet active;
  ColorSet highlighted;
  ColorSet pressed;
  font_t font;
  static font_t defaultFont;
};
} // namespace uwdg

#endif // UWDG_STYLE_H
