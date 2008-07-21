#include "shared.h"

using namespace render;

namespace
{

/*
    ���������
*/

const size_t VIEWPORT_ARRAY_RESERVE_SIZE = 4;   //������������� ������ ������� �������� ������
const size_t LISTENER_ARRAY_RESERVE_SIZE = 4;   //������������� ������ ������� ����������

}

/*
    �������� ���������� �������� �����
*/

typedef stl::vector<Viewport>           ViewportArray;
typedef xtl::com_ptr<IDesktopListener>  ListenerPtr;
typedef stl::vector<ListenerPtr>        ListenerArray;

struct Desktop::Impl: public xtl::reference_counter
{
  stl::string   name;             //��� �������� �����
  math::vec4f   background_color; //���� ����
  ViewportArray viewports;        //������� ������
  ListenerArray listeners;        //���������

  Impl ()
  {
    viewports.reserve (VIEWPORT_ARRAY_RESERVE_SIZE);
    listeners.reserve (LISTENER_ARRAY_RESERVE_SIZE);
  }
  
  ~Impl ()
  {
    DestroyNotify ();
  }
  
  template <class Fn>
  void Notify (Fn fn)
  {
    for (ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
    {
      try
      {
        fn (*iter);
      }
      catch (...)
      {
        //���������� ���� ����������
      }
    }        
  }  
  
  void ChangeNameNotify (const char* new_name)
  {
    Notify (xtl::bind (&IDesktopListener::OnChangeName, _1, new_name));
  }
  
  void ChangeBackgroundColorNotify (const math::vec4f& color)
  {
    Notify (xtl::bind (&IDesktopListener::OnChangeBackgroundColor, _1, xtl::cref (color)));
  }
  
  void AttachViewportNotify (render::Viewport& viewport)
  {
    Notify (xtl::bind (&IDesktopListener::OnAttachViewport, _1, xtl::ref (viewport)));
  }
  
  void DetachViewportNotify (render::Viewport& viewport)
  {
    Notify (xtl::bind (&IDesktopListener::OnDetachViewport, _1, xtl::ref (viewport)));
  }  

  void DestroyNotify ()
  {
    Notify (xtl::bind (&IDesktopListener::OnDestroy, _1));    
  }  
};

/*
    ������������ / ���������� / ������������
*/

Desktop::Desktop ()
  : impl (new Impl)
{
}

Desktop::Desktop (const Desktop& desktop)
  : impl (desktop.impl)
{
  addref (impl);
}

Desktop::~Desktop ()
{
  release (impl);
}

Desktop& Desktop::operator = (const Desktop& desktop)
{
  Desktop (desktop).Swap (*this);

  return *this;
}

/*
    ������������� �������� �����
*/

size_t Desktop::Id () const
{
  return reinterpret_cast<size_t> (get_pointer (impl));
}

/*
    ���
*/

void Desktop::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::Desktop::SetName", "name");
    
  if (name == impl->name)
    return;
    
  impl->name = name;
  
  impl->ChangeNameNotify (impl->name.c_str ());
}

const char* Desktop::Name () const
{
  return impl->name.c_str ();
}

/*
    ���� ����
*/

void Desktop::SetBackgroundColor (const math::vec4f& color)
{
  if (color == impl->background_color)
    return;

  impl->background_color = color;
  
  impl->ChangeBackgroundColorNotify (impl->background_color);
}

void Desktop::SetBackgroundColor (float red, float green, float blue, float alpha)
{
  SetBackgroundColor (math::vec4f (red, green, blue, alpha));
}

const math::vec4f& Desktop::BackgroundColor () const
{
  return impl->background_color;
}

/*
    ���������� / �������� �������� ������
*/

void Desktop::Attach (const render::Viewport& viewport)
{
  size_t viewport_id = viewport.Id ();

  for (ViewportArray::iterator iter=impl->viewports.begin (), end=impl->viewports.end (); iter!=end; ++iter)
    if (iter->Id () == viewport_id)
      return; //������� ������ ��� ���������

  impl->viewports.push_back (viewport);

  impl->AttachViewportNotify (impl->viewports.back ());
}

void Desktop::Detach (const render::Viewport& viewport)
{
  size_t viewport_id = viewport.Id ();

  for (ViewportArray::iterator iter=impl->viewports.begin (), end=impl->viewports.end (); iter!=end;)
    if (iter->Id () == viewport_id)
    {
      ViewportArray::iterator next = iter;

      ++next;

      impl->DetachViewportNotify (*iter);

      impl->viewports.erase (iter);

      iter = next;
    }
    else ++iter;
}

void Desktop::DetachAllViewports ()
{
  for (ViewportArray::iterator iter=impl->viewports.begin (), end=impl->viewports.end (); iter!=end; ++iter)
    impl->DetachViewportNotify (*iter);

  impl->viewports.clear ();
}

/*
    ���������� �������� ������
*/

size_t Desktop::ViewportsCount () const
{
  return impl->viewports.size ();
}

/*
    ��������� ������� ������
*/

render::Viewport& Desktop::Viewport (size_t index)
{
  if (index >= impl->viewports.size ())
    throw xtl::make_range_exception ("render::Desktop::Viewport", "index", index, impl->viewports.size ());

  return impl->viewports [index];
}

const render::Viewport& Desktop::Viewport (size_t index) const
{
  return const_cast<Desktop&> (*this).Viewport (index);
}

/*
    ������ �� �����������
*/

void Desktop::AttachListener (IDesktopListener* listener)
{
  if (!listener)
    throw xtl::make_null_argument_exception ("render::Desktop::AttachListener", "listener");

  impl->listeners.push_back (listener);
}

void Desktop::DetachListener (IDesktopListener* listener)
{
  if (!listener)
    return;

  impl->listeners.erase (stl::remove (impl->listeners.begin (), impl->listeners.end (), listener), impl->listeners.end ());
}

void Desktop::DetachAllListeners ()
{
  impl->listeners.clear ();
}

/*
    �����
*/

void Desktop::Swap (Desktop& desktop)
{
  stl::swap (impl, desktop.impl);
}

namespace render
{

void swap (Desktop& desktop1, Desktop& desktop2)
{
  desktop1.Swap (desktop2);
}

}
