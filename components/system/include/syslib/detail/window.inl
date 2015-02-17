/*
    Color
*/

inline Color::Color ()
  : red (0), green (0), blue (0)
  {}
  
inline Color::Color (unsigned char in_red, unsigned char in_green, unsigned char in_blue)
  : red (in_red), green (in_green), blue (in_blue)
  {}

/*
    Point
*/

inline Point::Point ()
  : x (0), y (0)
  {}

inline Point::Point (unsigned int in_x, unsigned int in_y)
  : x (in_x), y (in_y)
  {}

/*
    Rect
*/  

inline Rect::Rect ()
{
  left = top = right = bottom = 0;
}

inline Rect::Rect (unsigned int in_left, unsigned int in_top, unsigned int in_right, unsigned int in_bottom)
{
  left   = in_left;
  top    = in_top;
  right  = in_right;
  bottom = in_bottom;
}
