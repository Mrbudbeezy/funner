#include "shared.h"

using namespace common;
using namespace syslib;

/*
    �������� ���������� �������
*/

struct WindowCursor::Impl: public xtl::reference_counter
{
  stl::string        image_name;   //��� �������
  Platform::cursor_t handle;       //���������� �������
  
  Impl () : handle (0) {}
  
  Impl (const char* in_image_name, int hotspot_x, int hotspot_y)
    : image_name (in_image_name)
    , handle (Platform::CreateCursor (in_image_name, hotspot_x, hotspot_y))
  {
  }
  
  ~Impl ()
  {
    if (!handle)
      return;

    Platform::DestroyCursor (handle);
  }
};

/*
    ������������ / ���������� / ������������
*/

WindowCursor::WindowCursor ()
  : impl (new Impl)
{
}

WindowCursor::WindowCursor (const char* image_name, int hotspot_x, int hotspot_y)
{
  try
  {
    if (!image_name)
      throw xtl::make_null_argument_exception ("", "image_name");
      
    impl = new Impl (image_name, hotspot_x, hotspot_y);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowCursor::WindowCursor");
    throw;
  }
}

WindowCursor::WindowCursor (const WindowCursor& cursor)
  : impl (cursor.impl)
{
  addref (impl);
}

WindowCursor::~WindowCursor ()
{
  release (impl);
}

WindowCursor& WindowCursor::operator = (const WindowCursor& cursor)
{
  WindowCursor (cursor).Swap (*this);

  return *this;
}

/*
    ��� �������
*/

const char* WindowCursor::Image () const
{
  return impl->image_name.c_str ();
}

/*
    ��������� ��������������� ����������� �������
*/

const void* WindowCursor::Handle () const
{
  return impl->handle;
}

/*
    �����
*/

void WindowCursor::Swap (WindowCursor& cursor)
{
  stl::swap (impl, cursor.impl);
}

namespace syslib
{

void swap (WindowCursor& cursor1, WindowCursor& cursor2)
{
  cursor1.Swap (cursor2);
}

}
