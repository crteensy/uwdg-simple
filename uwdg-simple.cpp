#include "uwdg-simple.h"

namespace uwdg
{
font_t Style::defaultFont = gdispOpenFont("UI2");
Style Widget::defaultStyle_;
Widget* Widget::rootWidgets_;
Rectangle Widget::currentClippingRect_;
Point Widget::currentDrawingOffset_;
} // namespace uwdg
