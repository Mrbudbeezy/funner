#include "shared.h"

using namespace render::scene::server;

/*
    �������� ���������� ������
*/

struct Screen::Impl: public xtl::reference_counter
{
  stl::string name;             //��� ������
  bool        active;           //���������� ������
  Rect        area;             //������� ������
  bool        background_state; //��������� ������� ����
  math::vec4f background_color; //���� ������� ����

/// �����������
  Impl (const char* in_name)
    : name (in_name)
    , active (true)
    , area (0, 0, 100, 100)
    , background_state (true)
    , background_color (0, 0, 0, 1.0f)
  {
  }
};

/*
    ������������ / ���������� / ������������
*/

Screen::Screen (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    impl = new Impl (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Screen::Screen");
    throw;
  }
}

Screen::Screen (const Screen& s)
  : impl (s.impl)
{
  addref (impl);
}

Screen::~Screen ()
{
  release (impl);
}

Screen& Screen::operator = (const Screen& s)
{
  Screen tmp (s);

  stl::swap (impl, tmp.impl);

  return *this;
}

/*
    ��� ������
*/

const char* Screen::Name () const
{
  return impl->name.c_str ();
}

/*
    ���������� ������
*/

void Screen::SetActive (bool active)
{
  impl->active = active;
}

bool Screen::IsActive () const
{
  return impl->active;
}

/*
    ������� ������
*/

void Screen::SetScreenArea (const Rect& rect)
{
  impl->area = rect;
}

const Rect& Screen::ScreenArea () const
{
  return impl->area;
}

/*
    ������ ���
*/

void Screen::SetBackground (bool state, const math::vec4f& color)
{
  impl->background_state = state;
  impl->background_color = color;
}

bool Screen::BackgroundState () const
{
  return impl->background_state;
}

const math::vec4f& Screen::BackgroundColor () const
{
  return impl->background_color;
}

/*
    ���������� � ��������� ������
*/

/*
    ����������
*/

void Screen::Update ()
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Screen::Update");
    throw;
  }
}
