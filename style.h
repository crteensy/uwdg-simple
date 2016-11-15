#ifndef UWDG_STYLE_H
#define UWDG_STYLE_H

#include <gfx.h>

namespace uwdg
{
typedef color_t Color;
typedef font_t Font;
static constexpr Font DefaultFont = nullptr;

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
  Font font;
  static Font defaultFont;
};
} // namespace uwdg

#endif // UWDG_STYLE_H
