#include "shared.h"

using namespace render;

/*
    �������� ���������� Viewport
*/

typedef stl::hash_map<stl::hash_key<const char*>, stl::string> PropertyMap;

struct Viewport::Impl: public xtl::reference_counter
{
  stl::string          name;              //��� ������� ������
  scene_graph::Camera* camera;            //������
  stl::string          path_name;         //��� ���� ����������
  size_t               path_name_hash;    //��� ����� ���� ����������
  Rect                 rect;              //������� ������� ������
  bool                 is_active;         //���� ���������� ������� ������
  int                  z_order;           //������� ��������� ������� ������
  PropertyMap          properties;        //���������� ����������
  xtl::auto_connection on_destroy_camera; //���� ���������� � �������� ���������� �� �������� ������
  
  Impl () : camera (0), is_active (true), z_order (INT_MAX)
  {
    path_name_hash = common::strhash (path_name.c_str ());
  }
  
  void SetCamera (scene_graph::Camera* in_camera)
  {
    on_destroy_camera.disconnect ();

    camera = in_camera;

    if (camera)
    {
      on_destroy_camera = camera->RegisterEventHandler (scene_graph::NodeEvent_AfterDestroy,
        xtl::bind (&Impl::SetCamera, this, (scene_graph::Camera*)0));
    }
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
    
  impl->name = name;
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
  return reinterpret_cast<size_t> (get_pointer (impl));
}

/*
    ��������� ���� ���������� ����������� ������� ������
*/

void Viewport::SetRenderPath (const char* path_name)
{
  if (!path_name)
    throw xtl::make_null_argument_exception ("render::Viewport::SetRenderPath", "path_name");
    
  impl->path_name      = path_name;
  impl->path_name_hash = common::strhash (path_name);
}

const char* Viewport::RenderPath () const
{
  return impl->path_name.c_str ();
}

size_t Viewport::RenderPathHash () const
{
  return impl->path_name_hash;
}

/*
    ������� ������� ������
*/

void Viewport::SetArea (const Rect& rect)
{
  impl->rect = rect;
}

void Viewport::SetArea (size_t left, size_t top, size_t width, size_t height)
{
  SetArea (Rect (left, top, width, height));
}

void Viewport::SetOrigin (size_t left, size_t top)
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
  impl->z_order = z_order;
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
  impl->is_active = state;
}

bool Viewport::IsActive () const
{
  return impl->is_active;
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
    iter->second = value;
  }
  else
  {
    impl->properties.insert_pair (name, value);
  }
}

const char* Viewport::GetProperty (const char* name) const
{
  if (!name)
    return "";

  PropertyMap::iterator iter = impl->properties.find (name);

  if (iter == impl->properties.end ())
    return "";

  return iter->second.c_str ();
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
