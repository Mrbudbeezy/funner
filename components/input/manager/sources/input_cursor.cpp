#include "shared.h"

using namespace input;

/*
    �������� ���������� �������
*/

typedef stl::list<ICursorListener*> ListenerList;

struct Cursor::Impl: public xtl::reference_counter
{
  math::vec2f  position;   //��������� �������
  ListenerList listeners;  //��������� ������� �������
  bool         visible;    //����� �� ������
  stl::string  image_name; //��� ��������
  
  Impl () : visible (true) {}
  
  ~Impl ()
  {
    for (ListenerList::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
    {
      try
      {
        (*iter)->OnDestroy ();
      }
      catch (...)
      {
        //���������� ���� ����������
      }
    }
  }  
};

/*
    ������������ / ���������� / ������������
*/

Cursor::Cursor ()
  : impl (new Impl)
{
}

Cursor::Cursor (const Cursor& cursor)
  : impl (cursor.impl)
{
  addref (impl);
}

Cursor::~Cursor ()
{
  release (impl);
}

Cursor& Cursor::operator = (const Cursor& cursor)
{
  Cursor (cursor).Swap (*this);
  
  return *this;
}

/*
    ��������� ��������� �������
*/

void Cursor::SetPosition (float x, float y)
{
  SetPosition (math::vec2f (x, y));
}

void Cursor::SetPosition (const math::vec2f& position)
{
  if (position == impl->position)
    return;

  impl->position = position;
  
  for (ListenerList::iterator iter=impl->listeners.begin (), end=impl->listeners.end (); iter!=end; ++iter)
  {
    try
    {
      (*iter)->OnChangePosition (position);
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }
}

math::vec2f& Cursor::Position () const
{
  return impl->position;
}

float Cursor::GetX () const
{
  return impl->position.x;
}

float Cursor::GetY () const
{
  return impl->position.y;
}

/*
    ���������� ���������� �������
*/

bool Cursor::IsVisible () const
{
  return impl->visible;
}

void Cursor::SetVisible (bool state)
{
  if (state == impl->visible)
    return;

  impl->visible = state;
  
  for (ListenerList::iterator iter=impl->listeners.begin (), end=impl->listeners.end (); iter!=end; ++iter)
  {
    try    
    {
      (*iter)->OnChangeVisible (state);
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }
}

/*
    ��������� ����������� �������
*/

void Cursor::SetImage (const char* image_name)
{
  if (!image_name)
    throw xtl::make_null_argument_exception ("input::Cursor::SetImage", "image_name");
    
  impl->image_name = image_name;
  
  for (ListenerList::iterator iter=impl->listeners.begin (), end=impl->listeners.end (); iter!=end; ++iter)
  {
    try    
    {
      (*iter)->OnChangeImage (image_name);
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }  
}

const char* Cursor::Image () const
{
  return impl->image_name.c_str ();
}

/*
    ���������� ���������� ������� �������
*/

void Cursor::Attach (ICursorListener* listener)
{
  impl->listeners.push_back (listener);
}

void Cursor::Detach (ICursorListener* listener)
{
  impl->listeners.remove (listener);
}

/*
    �����
*/

void Cursor::Swap (Cursor& cursor)
{
  stl::swap (impl, cursor.impl);
}

namespace input
{

void swap (Cursor& cursor1, Cursor& cursor2)
{
  cursor1.Swap (cursor2);
}

}
