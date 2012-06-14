#include "shared.h"

using namespace scene_graph;

/*
    ���������
*/

namespace
{

const size_t LISTENER_ARRAY_RESERVE_SIZE = 16; //������������� ������ ������� ����������

}

/*
    �������� ���������� Viewport
*/

typedef stl::vector<IViewportListener*> ListenerArray;

struct Viewport::Impl: public xtl::reference_counter, public xtl::instance_counter<Viewport>
{
  stl::string          name;              //��� ������� ������
  scene_graph::Camera* camera;            //������
  stl::string          technique;         //��� ������� ����������
  Rect                 rect;              //������� ������� ������
  float                min_depth;         //����������� �������� �������
  float                max_depth;         //������������ �������� �������
  bool                 is_active;         //���� ���������� ������� ������  
  bool                 input_state;       //��������� �� ������� ������ � ��������� �����
  int                  z_order;           //������� ��������� ������� ������
  math::vec4f          background_color;  //���� ����
  bool                 has_background;    //������� ����
  common::PropertyMap  properties;        //���������� ����������
  ListenerArray        listeners;         //��������� ������� ������� ������
  xtl::auto_connection on_destroy_camera; //���� ���������� � �������� ���������� �� �������� ������

  Impl () : camera (0), min_depth (0.0f), max_depth (1.0f), is_active (true), input_state (true), z_order (INT_MAX), has_background (false)
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
    Notify (xtl::bind (&IViewportListener::OnViewportChangeName, _1, name.c_str ()));
  }
  
  void ChangeAreaNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnViewportChangeArea, _1, xtl::cref (rect), min_depth, max_depth));
  }
  
  void ChangeCameraNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnViewportChangeCamera, _1, camera));
  }
  
  void ChangeZOrderNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnViewportChangeZOrder, _1, z_order));
  }
  
  void ChangeActiveNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnViewportChangeActive, _1, is_active));
  }
  
  void ChangeInputStateNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnViewportChangeInputState, _1, input_state));
  }  
  
  void ChangeBackgroundNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnViewportChangeBackground, _1, has_background, xtl::cref (background_color)));
  }  
  
  void ChangeTechniqueNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnViewportChangeTechnique, _1, technique.c_str ()));
  }
  
  void ChangePropertiesNotify (const common::PropertyMap& properties)
  {
    Notify (xtl::bind (&IViewportListener::OnViewportChangeProperties, _1, properties));
  }  
  
  void DestroyNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnViewportDestroy, _1));    
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
    throw xtl::make_null_argument_exception ("scene_graph::Viewport::SetName", "name");
    
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
    ������� ����������
*/

void Viewport::SetTechnique (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("scene_graph::Viewport::SetTechnique", "name");
    
  if (impl->technique == name)
    return;
    
  impl->technique = name;
  
  impl->ChangeTechniqueNotify ();
}

const char* Viewport::Technique () const
{
  return impl->technique.c_str ();
}

/*
    ������� ������� ������
*/

void Viewport::SetArea (const Rect& rect)
{
  if (impl->rect == rect)
    return;

  impl->rect = rect;

  impl->ChangeAreaNotify ();
}

void Viewport::SetArea (int left, int top, int width, int height)
{
  SetArea (Rect (left, top, width, height));
}

void Viewport::SetOrigin (int left, int top)
{
  Rect new_rect = impl->rect;
  
  new_rect.x = left;
  new_rect.y  = top;
  
  SetArea (new_rect);
}

void Viewport::SetSize (int width, int height)
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
    ������� depth
*/

void Viewport::SetDepthRange (float min_depth, float max_depth)
{
  if (impl->min_depth == min_depth && impl->max_depth == max_depth)
    return;

  impl->min_depth = min_depth;
  impl->max_depth = max_depth;

  impl->ChangeAreaNotify ();  
}

void Viewport::SetMinDepth (float value)
{
  SetDepthRange (value, impl->max_depth);
}

void Viewport::SetMaxDepth (float value)
{
  SetDepthRange (impl->min_depth, value);
}

float Viewport::MinDepth () const
{
  return impl->min_depth;
}

float Viewport::MaxDepth () const
{
  return impl->max_depth;
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
    ��������� �� ������� ������ � ��������� �����
*/

void Viewport::SetInputState (bool state)
{
  if (state == impl->input_state)
    return;
    
  impl->input_state = state;
  
  impl->ChangeInputStateNotify ();
}

bool Viewport::InputState () const
{
  return impl->input_state;
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

void Viewport::SetProperties (const common::PropertyMap& properties)
{
  if (impl->properties.Id () == properties.Id ())
    return;
    
  impl->properties = properties;
  
  impl->ChangePropertiesNotify (properties);
}

const common::PropertyMap& Viewport::Properties () const
{
  return impl->properties;
}

/*
    ������ �� �����������
*/

void Viewport::AttachListener (IViewportListener* listener) const
{
  if (!listener)
    throw xtl::make_null_argument_exception ("scene_graph::Viewport::AttachListener", "listener");

  impl->listeners.push_back (listener);
}

void Viewport::DetachListener (IViewportListener* listener) const
{
  if (!listener)
    return;

  impl->listeners.erase (stl::remove (impl->listeners.begin (), impl->listeners.end (), listener), impl->listeners.end ());
}

void Viewport::DetachAllListeners () const
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
