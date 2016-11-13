#ifndef UWDG_GEOMETRY_H
#define UWDG_GEOMETRY_H

#include <gfx.h>
#ifdef max
  #define UWDG_GEOMETRY_H_RESTORE_MAX 1
  #pragma push_macro("max")
  #pragma push_macro("min")
  #undef max
  #undef min
#endif
#include <algorithm>

namespace uwdg
{

typedef coord_t Coordinate;

typedef uint16_t Length;

struct Point
{
  Point() : x(0), y(0) {}
  Point(const Coordinate& a, const Coordinate& b) : x(a), y(b) {}
  Point& operator+=(const Point& rhs) {x += rhs.x; y += rhs.y; return *this;}
  Point& operator-=(const Point& rhs) {x -= rhs.x; y -= rhs.y; return *this;}
  const Point operator+(const Point& rhs) const {return Point(*this)+= rhs;}
  const Point operator-(const Point& rhs) const {return Point(*this)-= rhs;}
  Coordinate x;
  Coordinate y;
};

struct Size
{
  Size() : w(0), h(0) {}
  Size(const Length& w_, const Length& h_) : w(w_), h(h_) {}
  Length w;
  Length h;
};

struct Rectangle
{
  Rectangle() {}
  Rectangle(const Point& p, const Size& s) : p0(p), size(s) {}
  // intersect
  Rectangle& operator &= (const Rectangle& rhs)
  {
    Coordinate left = std::max(p0.x, rhs.p0.x);
    Coordinate right = std::min(p0.x + size.w, rhs.p0.x + rhs.size.w);
    Coordinate top = std::max(p0.y, rhs.p0.y);
    Coordinate bottom = std::min(p0.y + size.h, rhs.p0.y + rhs.size.h);
    p0.x = left;
    p0.y = top;
    size.w = std::max(0, right-left);
    size.h = std::max(0, bottom-top);
    return *this;
  }
  const Rectangle operator&(const Rectangle& rhs) const {return Rectangle(*this) &= rhs;}

  Point p0;
  Size size;
};

} // namespace uwdg

#ifdef UWDG_GEOMETRY_H_RESTORE_MAX
  #pragma pop_macro("min")
  #pragma pop_macro("max")
  #undef UWDG_GEOMETRY_H_RESTORE_MAX
#endif // UWDG_GEOMETRY_H_RESTORE_MAX
#endif // UWDG_GEOMETRY_H

