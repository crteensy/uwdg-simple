#ifndef UWDG_WIDGET_H
#define UWDG_WIDGET_H

#include <algorithm>

#include "geometry.h"
#include "style.h"
#include "debug.h"
#include "inputEvent.h"

namespace uwdg
{

class Widget
{
private:
  typedef uint8_t flag_t;
public:
  /*****************************************************************************
  * Lifetime
  *****************************************************************************/
  Widget(Widget* parent = nullptr) :
    parent_(parent),
    children_(nullptr),
    lastChild_(nullptr),
    prev_(nullptr),
    next_(nullptr),
    style_(parent != nullptr ? &parent->style() : &defaultStyle_),
    flags_(flag_visible | flag_redraw)
  {
    if(parent != nullptr)
    {
      parent->appendChild(this);
    }
    else
    {
      addRoot(this);
      setSize(gdispGGetWidth(display()), gdispGGetHeight(display()));
    }
  }

  virtual ~Widget()
  {
    PRINTDEBUG(("~Widget(%p)\n", this));
    bool hadFocus = hasFocus();
    if(hasParent())
    {
      parent()->removeChild(this);
      if(hadFocus)
      {
        parent()->giveFocus();
      }
    }
    else
    {
      removeRoot(this);
      if(rootWidgets_ != nullptr)
      {
        rootWidgets_->redraw();
        if(hadFocus)
        {
          rootWidgets_->giveFocus();
        }
      }
    }
/* removed in "simple" version
    while(hasChildren())
    {
      delete children();
    }
*/
  }


  /*****************************************************************************
  * Flags and their manipulation
  *****************************************************************************/
  void setFlag(flag_t f)
  {
    flags_ |= f;
  }

  void clearFlag(flag_t f)
  {
    flags_ &= ~f;
  }

  bool getFlag(flag_t f) const
  {
    return flags_ & f;
  }

  /*****************************************************************************
  * Root widgets
  *****************************************************************************/
  static void addRoot(Widget* w)
  {
    w->next_ = rootWidgets_;
    rootWidgets_ = w;
  }


  static void removeRoot(Widget* root)
  {
    Widget* w = rootWidgets_;
    if (w == root)
    {
      rootWidgets_ = root->next();
      return;
    }
    while((w != nullptr) && (w->next() != root))
    {
      w = w->next();
    }
    // w is now nullptr or the root we're looking for
    if(w->next() == root)
    {
      w->next_ = root->next();
    }
  }

  /*****************************************************************************
  * Parent
  *****************************************************************************/
  Widget* parent() const {return parent_;}


  bool hasParent() const {return parent() != nullptr;}


  /*****************************************************************************
  * Children
  *****************************************************************************/
  void appendChild(Widget* w)
  {
    if(hasChildren())
    {
      lastChild_->next_ = w;
      w->prev_ = lastChild_;
    }
    else
    {
      children_ = w;
    }
    lastChild_ = w;
  }


  Widget* children() const
  {
    return children_;
  }


  bool hasChildren() const
  {
    return (children() != nullptr);
  }


void removeChild(Widget* child)
{
  // remove head of list?
  if (children_ == child)
  {
    children_ = child->next_;
  }
  // next item present? update that
  if (child->next_ != nullptr)
  {
    child->next_->prev_ = child->prev_;
  }
  // previous item present? update that
  if (child->prev_ != nullptr)
  {
    child->prev_->next_ = child->next_;
  }
  // update tail
  if(lastChild_ == child)
  {
    lastChild_->next_ = nullptr;
  }
}


  /*****************************************************************************
  * Siblings
  *****************************************************************************/
  Widget* prev() const
  {
    return prev_;
  }

  Widget* next() const
  {
    return next_;
  }


  /*****************************************************************************
  * Geometry
  *****************************************************************************/
  const Rectangle& geometry() const
  {
    return geometry_;
  }

  const Point& position() const
  {
    return geometry().p0;
  }

  void moveTo(const Point& p) // (relative to parent)
  {
    geometry_.p0 = p;
    if(hasParent())
    {
      parent()->redraw();
    }
    else
    {
      redraw();
    }
  }

  virtual void onResize() {}

  void setSize(const Length& w, const Length& h)
  {
    geometry_.size.w = w;
    geometry_.size.h = h;
    onResize();
  }

  void setSize(const Size& size)
  {
    geometry_.size = size;
    onResize();
  }

  void setWidth(const Length& w)
  {
    geometry_.size.w = w;
    onResize();
  }

  void setHeight(const Length& h)
  {
    geometry_.size.h = h;
    onResize();
  }

  const Size& size() const
  {
    return geometry().size;
  }

  const Length& width() const
  {
    return size().w;
  }

  const Length& height() const
  {
    return size().h;
  }


  /*****************************************************************************
  * Styling & Appearance
  *****************************************************************************/
  static const Style& defaultStyle()
  {
    return defaultStyle_;
  }

  static void setDefaultStyle(const Style& s)
  {
    defaultStyle_ = s;
  }

  const Style& style() const
  {
    if (style_ != nullptr)
    {
//      return defaultStyle();
      return *style_;
    }
    else if (hasParent())
    {
      return parent()->style();
    }
    else
    {
      return defaultStyle();
    }
  }

  void setStyle(const Style* style) // pointer allows resetting to default, so no reference arg
  {
    style_ = style;
  }


  bool transparent() const
  {
    return getFlag(flag_transparent);
  }

  bool opaque() const
  {
    return !transparent();
  }

  void setTransparent(bool b)
  {
    if(b)
    {
      setFlag(flag_transparent);
    }
    else
    {
      clearFlag(flag_transparent);
    }
  }

  void setTransparent()
  {
    setTransparent(true);
  }

  void setOpaque()
  {
    setTransparent(false);
  }

  /*****************************************************************************
  * visibility
  *****************************************************************************/
  void setVisible(bool b)
  {
    if(b)
    {
      setFlag(flag_visible);
    }
    else
    {
      clearFlag(flag_visible);
    }
  }

  void show()
  {
    redraw();
    setVisible(true);
  }

  void hide()
  {
    setVisible(false);
  }

  bool visible() const
  {
    if(hasParent())
    {
      return (getFlag(flag_visible) && parent()->visible());
    }
    return getFlag(flag_visible);
  }

  bool hidden() const
  {
    return !visible();
  }

  /*****************************************************************************
  * drawing
  *****************************************************************************/
  virtual void draw() const
  {
    if(opaque())
    {
      gdispGDrawBox(display(), absX(0), absY(0), width(), height(), colorSet().border);
      gdispGFillArea(display(), absX(1), absY(1), width()-2, height()-2, colorSet().fill);
    }
    else
    {
      if(!hasParent())
      {
        gdispGFillArea(display(), absX(1), absY(1), width()-2, height()-2, style().background);
      }
    }
  };

  void redraw()
  {
    setFlag(flag_redraw);
    if(transparent() && hasParent())
    {
      parent()->redraw();
    }
  }


  static const Coordinate absX(const Coordinate& x)
  {
    return x + currentDrawingOffset_.x;
  }
  static const Coordinate absX(const Point& p)
  {
    return absX(p.x);
  }
  static const Coordinate absY(const Coordinate& y)
  {
    return y + currentDrawingOffset_.y;
  }
  static const Coordinate absY(const Point& p)
  {
    return absY(p.y);
  }
  static const Point absPoint(const Point& p)
  {
    return p + currentDrawingOffset_;
  }
  static const Point absPoint(const Coordinate& x, const Coordinate& y)
  {
    return absPoint(Point(x,y));
  }


  void drawWidget()
  {
//    if(moved())
//    {
//      flags_ &= ~flag_moved;
//      geometry_.p0 = newPosition_;
//    }
    if(visible())
    {
      Point offset_backup = currentDrawingOffset_;
      currentDrawingOffset_ += position();
      Rectangle clip_backup = currentClippingRect_;
      currentClippingRect_ &= Rectangle(absPoint(0,0), size());
      gdispGSetClip(GDISP,
                    currentClippingRect_.p0.x,
                    currentClippingRect_.p0.y,
                    currentClippingRect_.size.w,
                    currentClippingRect_.size.h);
      bool redrawAllChildren = false;
      if(getFlag(flag_redraw))
      {
        redrawAllChildren = true;
        draw();
      }
      Widget* p = children();
      while(p != nullptr)
      {
        if(redrawAllChildren)
        {
          p->redraw();
        }
        p->drawWidget();
        p = p->next();
      }
      clearFlag(flag_redraw);

      currentDrawingOffset_ = offset_backup;
      currentClippingRect_ = clip_backup;
      gdispGSetClip(GDISP,
                    currentClippingRect_.p0.x,
                    currentClippingRect_.p0.y,
                    currentClippingRect_.size.w,
                    currentClippingRect_.size.h);
    }
  }

  static void drawWidgets()
  {
    // draws only the root that is on top of the list
    if(rootWidgets_)
    {
      rootWidgets_->drawWidget();
    }
  }

  /*****************************************************************************
  * Focus
  *****************************************************************************/
  bool acceptsFocus() const
  {
    return (visible() && getFlag(flag_acceptsFocus));
  }
  void setAcceptsFocus(bool b)
  {
    if(b)
    {
      flags_ |= flag_acceptsFocus;
    }
    else
    {
      flags_ &= ~flag_acceptsFocus;
    }
  }

  static Widget* focus()
  {
    return getFocusP();
  }

  bool hasFocus() const
  {
    return (focus() == this);
  }

  bool giveFocus()
  {
    // first of all:
    if(acceptsFocus() && hasFocus())
    {
      PRINTDEBUG(("focus remains as it is (%p)\n", focus()));
      return true;
    }
    // try to give focus to some child
    if(hasChildren())
    {
      Widget* w = children();
      while(w != nullptr)
      {
        PRINTDEBUG(("focus: descending to child %p\n", w));
        if(w->giveFocus())
        {
          w->redraw();
          return true;
        }
        w = w->next();
      }
    }
    // no child accepted focus
    if (acceptsFocus())
    {
      if(focus() != nullptr)
      {
        focus()->redraw();
        focus()->onLooseFocus();
      }
      getFocusP() = this;
      redraw();
      onFocus();
      PRINTDEBUG(("focused %p\n", focus()));
      return true;
    }
    PRINTDEBUG(("found no focus candidate\n"));
    return false;
  }

  void focusNextChild()
  {
    // find that child whose parent is this
    Widget* p = focus();
    while(p->parent() != this)
    {
      p = p->parent();
    }
    // now p->parent() == this
    // find next child that accepts focus
    while((p->next() != nullptr))
    {
      if (p->next()->giveFocus())
      {
        return;
      }
      p = p->next();
    }
    // no next child, start over at first child
    p = children();
    while((p != nullptr) && (!p->giveFocus()))
    {
      p = p->next();
    }
  }

  void focusPrevChild()
  {
    // find that child whose parent is this
    Widget* p = focus();
    while(p->parent() != this)
    {
      p = p->parent();
    }
    // now p->parent() == this
    // find prev child that accepts focus
    while((p->prev() != nullptr))
    {
      if (p->prev()->giveFocus())
      {
        return;
      }
      p = p->prev();
    }
    // no prev child, start over at first child
    p = lastChild_;
    while((p != nullptr) && (!p->giveFocus()))
    {
      p = p->prev();
    }
  }

  virtual void onFocus()
  {
  }

  virtual void onLooseFocus()
  {
  }

  static Widget*& getFocusP()
  {
    static Widget* p_ = nullptr;
    return p_;
  }



  /*****************************************************************************
  * input events
  *****************************************************************************/
  static void dispatchInputEvent(InputEvent& event)
  {
    Widget* w = focus();
    while((w != nullptr) && (!event.accepted()))
    {
      w->onInputEvent(event);
      w = w->parent();
    }
  }

  virtual void onInputEvent(InputEvent& event)
  {
    if (event.isPress())
    {
      switch(event.type())
      {
        case InputEvent::eLeft:
        case InputEvent::eUp:
        case InputEvent::eCCW:
          focusPrevChild();
          event.accept();
          break;
        case InputEvent::eRight:
        case InputEvent::eDown:
        case InputEvent::eCW:
          focusNextChild();
          event.accept();
          break;
        case InputEvent::eEnter:
          break;
        default:
          break;
      }
    }
  }

  /*****************************************************************************
  * class management
  *****************************************************************************/
  static void init()
  {
    defaultStyle_ = {
      Black,
      { // inactive
        Black,  // fill
        Gray, // text
        Gray  // border
      },
      { // active
        Gray,  // fill
        White, // text
        White  // border
      },
      { // highlighted
        Gray,  // fill
        White, // text
        Orange  // border
      },
      { // pressed
        Yellow,  // fill
        White, // text
        White  // border
      }
    };
    defaultStyle_.font = Style::defaultFont;
    currentClippingRect_.size = Size(gdispGetWidth(), gdispGetHeight());
  }


protected:
  GDisplay* display() const {return GDISP;}
  const Style::ColorSet& colorSet() const
  {
    if(hasFocus())
    {
      return style().highlighted;
    }

    return style().active;
  }
  static Style defaultStyle_;
private:
  bool moved() const
  {
    return (flags_ & flag_moved);
  }
  Widget* parent_;
  Widget* children_;
  Widget* lastChild_;
  Widget* prev_;
  Widget* next_;
  const Style* style_;
  Rectangle geometry_;
  flag_t flags_;
  static constexpr flag_t flag_visible      = (1<<0);
  static constexpr flag_t flag_redraw       = (1<<2);
  static constexpr flag_t flag_moved        = (1<<3);
  static constexpr flag_t flag_acceptsFocus = (1<<4);
  static constexpr flag_t flag_transparent  = (1<<5);
  static constexpr flag_t flag_inactive     = (1<<6);
  static Widget* rootWidgets_;
  static Rectangle currentClippingRect_;
  static Point currentDrawingOffset_;
};
} // namespace uwdg

#endif // UWDG_WIDGET_H
