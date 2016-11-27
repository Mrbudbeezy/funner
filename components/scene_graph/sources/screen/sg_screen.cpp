#include "shared.h"

using namespace scene_graph;

namespace
{

/*
    Константы
*/

const size_t VIEWPORT_ARRAY_RESERVE_SIZE = 4; //резервируемый размер массива областей вывода

}

/*
    Описание реализации рабочего стола
*/

typedef stl::vector<Viewport>       ViewportArray;
typedef stl::list<IScreenListener*> ListenerList;

struct Screen::Impl: public xtl::reference_counter, public xtl::instance_counter<Screen>
{
  stl::string   name;             //имя рабочего стола
  Rect          area;             //рабочая область
  math::vec4f   background_color; //цвет фона
  bool          has_background;   //есть ли фон
  ViewportArray viewports;        //области вывода
  ListenerList  listeners;        //слушатели

  Impl () : area (0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT), has_background (true)
  {
    viewports.reserve (VIEWPORT_ARRAY_RESERVE_SIZE);
  }
  
  ~Impl ()
  {
    DestroyNotify ();
  }
  
  template <class Fn>
  void Notify (Fn fn)
  {
    for (ListenerList::iterator iter=listeners.begin (); iter!=listeners.end ();)
    {
      ListenerList::iterator next = iter;

      ++next;

      try
      {        
        fn (*iter);        
      }
      catch (...)
      {
        //подавление всех исключений
      }

      iter = next;
    }        
  }  
  
  void ChangeNameNotify ()
  {
    Notify (xtl::bind (&IScreenListener::OnScreenChangeName, _1, name.c_str ()));
  }
  
  void ChangeAreaNotify ()
  {
    Notify (xtl::bind (&IScreenListener::OnScreenChangeArea, _1, area));
  }
  
  void ChangeBackgroundNotify ()
  {
    Notify (xtl::bind (&IScreenListener::OnScreenChangeBackground, _1, has_background, xtl::cref (background_color)));
  }
  
  void AttachViewportNotify (scene_graph::Viewport& viewport)
  {
    Notify (xtl::bind (&IScreenListener::OnScreenAttachViewport, _1, xtl::ref (viewport)));
  }
  
  void DetachViewportNotify (scene_graph::Viewport& viewport)
  {
    Notify (xtl::bind (&IScreenListener::OnScreenDetachViewport, _1, xtl::ref (viewport)));
  }  

  void DestroyNotify ()
  {
    Notify (xtl::bind (&IScreenListener::OnScreenDestroy, _1));    
  }  
};

/*
    Конструкторы / деструктор / присваивание
*/

Screen::Screen ()
  : impl (new Impl)
{
}

Screen::Screen (const Screen& Screen)
  : impl (Screen.impl)
{
  addref (impl);
}

Screen::~Screen ()
{
  release (impl);
}

Screen& Screen::operator = (const Screen& screen)
{
  Screen (screen).Swap (*this);

  return *this;
}

/*
    Идентификатор рабочего стола
*/

size_t Screen::Id () const
{
  return reinterpret_cast<size_t> (impl);
}

/*
    Имя
*/

void Screen::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("scene_graph::Screen::SetName", "name");
    
  if (name == impl->name)
    return;
    
  impl->name = name;
  
  impl->ChangeNameNotify ();
}

const char* Screen::Name () const
{
  return impl->name.c_str ();
}

/*
    Рабочая область
*/

void Screen::SetArea (const Rect& rect)
{
  if (impl->area == rect)
    return;

  impl->area = rect;

  impl->ChangeAreaNotify ();
}

void Screen::SetArea (int left, int top, int width, int height)
{
  SetArea (Rect (left, top, width, height));
}

void Screen::SetOrigin (int left, int top)
{
  Rect new_rect = impl->area;
  
  new_rect.x = left;
  new_rect.y = top;
  
  SetArea (new_rect);
}

void Screen::SetSize (int width, int height)
{
  Rect new_rect = impl->area;
  
  new_rect.width  = width;
  new_rect.height = height;

  SetArea (new_rect);
}

const Rect& Screen::Area () const
{
  return impl->area;
}

/*
    Управление фоном
*/

void Screen::SetBackgroundColor (const math::vec4f& color)
{
  if (color == impl->background_color)
    return;

  impl->background_color = color;

  impl->ChangeBackgroundNotify ();
}

void Screen::SetBackgroundColor (float red, float green, float blue, float alpha)
{
  SetBackgroundColor (math::vec4f (red, green, blue, alpha));
}

const math::vec4f& Screen::BackgroundColor () const
{
  return impl->background_color;
}

void Screen::SetBackgroundState (bool state)
{
  if (state == impl->has_background)
    return;

  impl->has_background = state;

  impl->ChangeBackgroundNotify ();
}

bool Screen::BackgroundState () const
{
  return impl->has_background;
}

/*
    Добавление / удаление областей вывода
*/

void Screen::Attach (const scene_graph::Viewport& viewport)
{
  size_t viewport_id = viewport.Id ();

  for (ViewportArray::iterator iter=impl->viewports.begin (), end=impl->viewports.end (); iter!=end; ++iter)
    if (iter->Id () == viewport_id)
      return; //область вывода уже добавлена

  impl->viewports.push_back (viewport);

  impl->AttachViewportNotify (impl->viewports.back ());
}

void Screen::Detach (const scene_graph::Viewport& viewport)
{
  size_t viewport_id = viewport.Id ();

  for (ViewportArray::iterator iter=impl->viewports.begin (); iter!=impl->viewports.end ();)
    if (iter->Id () == viewport_id)
    {
      impl->DetachViewportNotify (*iter);

      impl->viewports.erase (iter);
    }
    else ++iter;
}

void Screen::DetachAllViewports ()
{
  for (ViewportArray::iterator iter=impl->viewports.begin (), end=impl->viewports.end (); iter!=end; ++iter)
    impl->DetachViewportNotify (*iter);

  impl->viewports.clear ();
}

/*
    Количество областей вывода
*/

size_t Screen::ViewportsCount () const
{
  return impl->viewports.size ();
}

/*
    Получение области вывода
*/

scene_graph::Viewport& Screen::Viewport (size_t index)
{
  if (index >= impl->viewports.size ())
    throw xtl::make_range_exception ("scene_graph::Screen::Viewport", "index", index, impl->viewports.size ());

  return impl->viewports [index];
}

const scene_graph::Viewport& Screen::Viewport (size_t index) const
{
  return const_cast<Screen&> (*this).Viewport (index);
}

/*
    Работа со слушателями
*/

void Screen::AttachListener (IScreenListener* listener) const
{
  if (!listener)
    throw xtl::make_null_argument_exception ("scene_graph::Screen::AttachListener", "listener");

  impl->listeners.push_back (listener);
}

void Screen::DetachListener (IScreenListener* listener) const
{
  if (!listener)
    return;

  impl->listeners.erase (stl::remove (impl->listeners.begin (), impl->listeners.end (), listener), impl->listeners.end ());
}

void Screen::DetachAllListeners () const
{
  impl->listeners.clear ();
}

/*
    Обмен
*/

void Screen::Swap (Screen& screen)
{
  stl::swap (impl, screen.impl);
}

namespace render
{

void swap (Screen& screen1, Screen& screen2)
{
  screen1.Swap (screen2);
}

}
