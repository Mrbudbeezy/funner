#include "shared.h"

using namespace render::obsolete;

/*
    ���������
*/

namespace
{

const size_t LISTENER_ARRAY_RESERVE_SIZE = 16; //������������� ������ ������� ����������

/*
    ���������� ����������
*/

struct RenderProperty: public IViewportProperty, public xtl::reference_counter
{
  stl::string name, value;

  const char* Name  () const { return name.c_str (); }
  const char* Value () const { return value.c_str (); }

  RenderProperty (const char* in_name, const char* in_value) : name (in_name), value (in_value) {}
};

}

/*
    �������� ���������� Viewport
*/

typedef stl::vector<IViewportListener*>                        ListenerArray;
typedef xtl::intrusive_ptr<RenderProperty>                     PropertyPtr;
typedef stl::hash_map<stl::hash_key<const char*>, PropertyPtr> PropertyMap;

struct Viewport::Impl: public xtl::reference_counter
{
  stl::string          name;              //��� ������� ������
  scene_graph::Camera* camera;            //������
  stl::string          path_name;         //��� ���� ����������
  Rect                 rect;              //������� ������� ������
  bool                 is_active;         //���� ���������� ������� ������  
  int                  z_order;           //������� ��������� ������� ������
  math::vec4f          background_color;  //���� ����
  bool                 has_background;    //������� ����
  PropertyMap          properties;        //���������� ����������
  ListenerArray        listeners;         //��������� ������� ������� ������
  xtl::auto_connection on_destroy_camera; //���� ���������� � �������� ���������� �� �������� ������

  Impl () : camera (0), is_active (true), z_order (INT_MAX), has_background (false)
  {
    listeners.reserve (LISTENER_ARRAY_RESERVE_SIZE);
  }

  ~Impl ()
  {
    DestroyNotify ();
  }

  void SetCamera (scene_graph::Camera* in_camera)
  {
    if (camera == in_camera)
      return;
    
    on_destroy_camera.disconnect ();

    camera = in_camera;

    if (camera)
    {
      on_destroy_camera = camera->RegisterEventHandler (scene_graph::NodeEvent_AfterDestroy,
        xtl::bind (&Impl::SetCamera, this, (scene_graph::Camera*)0));
    }

    ChangeCameraNotify ();
  }

  template <class Fn>
  void Notify (Fn fn)
  {
    for (ListenerArray::iterator volatile iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
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
    
  void ChangeNameNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnChangeName, _1, name.c_str ()));
  }
  
  void ChangeAreaNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnChangeArea, _1, xtl::cref (rect)));
  }
  
  void ChangeCameraNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnChangeCamera, _1, camera));
  }
  
  void ChangeZOrderNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnChangeZOrder, _1, z_order));
  }
  
  void ChangeActiveNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnChangeActive, _1, is_active));
  }
  
  void ChangeBackgroundNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnChangeBackground, _1, has_background, xtl::cref (background_color)));
  }  
  
  void ChangeRenderPathNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnChangeRenderPath, _1, path_name.c_str ()));
  }
  
  void ChangePropertyNotify (const char* name, const char* new_value)
  {
    Notify (xtl::bind (&IViewportListener::OnChangeProperty, _1, name, new_value));
  }  
  
  void DestroyNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnDestroy, _1));    
  }
};

/*
    ������������ / ���������� / ������������
*/

Viewport::Viewport ()
  : impl (new Impl)
{
}

Viewport::Viewport (const Viewport& vp)
  : impl (vp.impl)
{
  addref (impl);
}

Viewport::~Viewport ()
{
  release (impl);
}

Viewport& Viewport::operator = (const Viewport& vp)
{
  Viewport (vp).Swap (*this);
  
  return *this;
}

/*
    ��� ������� ������
*/

void Viewport::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::Viewport::SetName", "name");
    
  if (name == impl->name)
    return;
    
  impl->name = name;
  
  impl->ChangeNameNotify ();
}

const char* Viewport::Name () const
{
  return impl->name.c_str ();
}

/*
    ������������� ������� ������
*/

size_t Viewport::Id () const
{
  return reinterpret_cast<size_t> (impl);
}

/*
    ��������� ���� ���������� ����������� ������� ������
*/

void Viewport::SetRenderPath (const char* path_name)
{
  if (!path_name)
    throw xtl::make_null_argument_exception ("render::Viewport::SetRenderPath", "path_name");
    
  if (impl->path_name == path_name)
    return;
    
  impl->path_name = path_name;
  
  impl->ChangeRenderPathNotify ();
}

const char* Viewport::RenderPath () const
{
  return impl->path_name.c_str ();
}

/*
    ������� ������� ������
*/

void Viewport::SetArea (const Rect& rect)
{
  if (impl->rect.left == rect.left && impl->rect.top == rect.top && impl->rect.width == rect.width && impl->rect.height == rect.height)
    return;

  impl->rect = rect;

  impl->ChangeAreaNotify ();
}

void Viewport::SetArea (int left, int top, size_t width, size_t height)
{
  SetArea (Rect (left, top, width, height));
}

void Viewport::SetOrigin (int left, int top)
{
  Rect new_rect = impl->rect;
  
  new_rect.left = left;
  new_rect.top  = top;
  
  SetArea (new_rect);
}

void Viewport::SetSize (size_t width, size_t height)
{
  Rect new_rect = impl->rect;
  
  new_rect.width  = width;
  new_rect.height = height;

  SetArea (new_rect);
}

const Rect& Viewport::Area () const
{
  return impl->rect;
}

/*
    ���������� �������� ��������� �������� ������ (��������� �� ���������� ������� �� ���������� ������)
*/

void Viewport::SetZOrder (int z_order)
{
  if (z_order == impl->z_order)
    return;

  impl->z_order = z_order;
  
  impl->ChangeZOrderNotify ();
}

int Viewport::ZOrder () const
{
  return impl->z_order;
}

/*
    ������, ��������� � �������� ������ (�������� �������� - weak-reference)
*/

void Viewport::SetCamera (scene_graph::Camera* camera)
{
  impl->SetCamera (camera);
}

const scene_graph::Camera* Viewport::Camera () const
{
  return impl->camera;
}

scene_graph::Camera* Viewport::Camera ()
{
  return impl->camera;
}

/*
    ���������� ����������� ������� ������ (��������� ������� �� ��������� � ���������)
*/

void Viewport::SetActive (bool state)
{
  if (state == impl->is_active)
    return;

  impl->is_active = state;
  
  impl->ChangeActiveNotify ();
}

bool Viewport::IsActive () const
{
  return impl->is_active;
}

/*
    ��������� ����
*/

void Viewport::SetBackgroundColor (const math::vec4f& color)
{
  if (impl->background_color == color)
    return;

  impl->background_color = color;
  
  impl->ChangeBackgroundNotify ();
}

void Viewport::SetBackgroundColor (float red, float green, float blue, float alpha)
{
  SetBackgroundColor (math::vec4f (red, green, blue, alpha));
}

const math::vec4f& Viewport::BackgroundColor () const
{
  return impl->background_color;
}

void Viewport::SetBackgroundState (bool state)
{
  if (state == impl->has_background)
    return;

  impl->has_background = state;

  impl->ChangeBackgroundNotify ();
}

bool Viewport::BackgroundState () const
{
  return impl->has_background;
}

/*
    ��������� ���������� ����������
*/

void Viewport::SetProperty (const char* name, const char* value)
{
  static const char* METHOD_NAME = "render::Viewport::SetProperty";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  if (!value)
    throw xtl::make_null_argument_exception (METHOD_NAME, "value");
    
  PropertyMap::iterator iter = impl->properties.find (name);
  
  if (iter != impl->properties.end ())
  {
    if (iter->second->value == value)
      return;

    iter->second->value = value;
  }
  else
  {
    impl->properties.insert_pair (name, PropertyPtr (new RenderProperty (name, value), false));
  }
  
  impl->ChangePropertyNotify (name, value);
}

const char* Viewport::GetProperty (const char* name) const
{
  if (!name)
    return "";

  PropertyMap::iterator iter = impl->properties.find (name);

  if (iter == impl->properties.end ())
    return "";

  return iter->second->value.c_str ();
}

bool Viewport::HasProperty (const char* name) const
{
  if (!name)
    return false;

  PropertyMap::iterator iter = impl->properties.find (name);

  return iter != impl->properties.end ();
}

void Viewport::RemoveProperty (const char* name)
{
  if (!name)
    return;
    
  impl->properties.erase (name);
}

void Viewport::RemoveAllProperties ()
{  
  impl->properties.clear ();
}

/*
    ������������ ���������� ����������
*/

namespace
{

struct ViewportPropertySelector
{
  const IViewportProperty& operator () (const PropertyMap::value_type& value) const { return *value.second; }
};

}

Viewport::PropertyIterator Viewport::CreatePropertyIterator () const
{
  return PropertyIterator (impl->properties.begin (), impl->properties.begin (), impl->properties.end (), ViewportPropertySelector ());
}

/*
    ������ �� �����������
*/

void Viewport::AttachListener (IViewportListener* listener)
{
  if (!listener)
    throw xtl::make_null_argument_exception ("render::Viewport::AttachListener", "listener");

  impl->listeners.push_back (listener);
}

void Viewport::DetachListener (IViewportListener* listener)
{
  if (!listener)
    return;

  impl->listeners.erase (stl::remove (impl->listeners.begin (), impl->listeners.end (), listener), impl->listeners.end ());
}

void Viewport::DetachAllListeners ()
{
  impl->listeners.clear ();
}

/*
    �����
*/

void Viewport::Swap (Viewport& vp)
{
  stl::swap (impl, vp.impl);
}

namespace render
{
    
void swap (Viewport& vp1, Viewport& vp2)
{
  vp1.Swap (vp2);
}

}
