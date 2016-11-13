#ifndef UWDG_LABEL_H
#define UWDG_LABEL_H

#include <cstring>

#include "widget.h"

// max length of a label including terminating zero
#define LABEL_LEN 40

namespace uwdg
{

class Label: public Widget
{
public:

  enum Alignment
  {
    left = justifyLeft,
    center = justifyCenter,
    right = justifyRight
  };

  Label(Widget* parent = nullptr) :
      Widget::Widget(parent)
  {
    reset();
  }

  Label(const char* s, Widget* parent = nullptr)  :
      Widget::Widget(parent)
  {
    reset();
    setText(s);
  }

  const char* text() const
  {
    return buf_;
  }

  void setText(const char* s)
  {
    strncpy(buf_, s, LABEL_LEN-1);
    buf_[LABEL_LEN-1] = 0;
    redraw();
  }

  Alignment alignment() const
  {
    return alignment_;
  }

  void setAlignment(Alignment a)
  {
    if(a != alignment())
    {
      alignment_ = a;
      redraw();
    }
  }

  virtual void draw() const override
  {
    PRINTDEBUG(("Label(@%p)::draw()\n", this));
    Widget::draw();
    gdispGDrawStringBox(display(), absX(0), absY(0), width(), height(),
                        text(), style().font, colorSet().text, (justify_t)alignment_);
  }
private:
  void reset()
  {
    alignment_ = Alignment::left;
  }
  char buf_[LABEL_LEN];
  Alignment alignment_;
};

} // namespace uwdg

#endif // UWDG_LABEL_H

