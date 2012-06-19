#include "shared.h"

using namespace scene_graph;
using namespace input;

/*
    ��������� / ��������
*/

InputPort::InputPort (Viewport& in_viewport, bool& in_z_order_changed)
  : viewport (in_viewport)
  , z_order_changed (in_z_order_changed)
  , is_active (viewport.IsActive () && viewport.InputState ())
  , need_update (true)
  , touch_size (1.0f)
  , touch_size_space (InputTransformSpace_Default)
{
  static const size_t PLANES_COUNT = 6;

  touch_frustum.reserve (PLANES_COUNT);

  z_order_changed = true;    

  OnViewportChangeCamera (viewport.Camera ());
  
  viewport.AttachListener (this);
}

InputPort::~InputPort ()
{
  viewport.DetachListener (this);
}

/*
    ��ࠡ��稪� ᮡ�⨩
*/

void InputPort::OnViewportChangeArea (const Rect&, float, float)
{
  need_update = true;
}

void InputPort::OnSceneChanged ()
{
  need_update = true;
  
  Camera* camera = viewport.Camera ();
  Scene*  scene  = camera ? camera->Scene () : (Scene*)0;

  if (!scene)
  {    
    input_scene = InputScenePtr ();    
    return;
  }
  
  input_scene = InputSceneCollectionSingleton::Instance ()->GetScene (*scene);
}

void InputPort::OnViewportChangeCamera (Camera* new_camera)
{
  need_update = true;
  
  on_camera_updated.disconnect ();
  on_scene_changed.disconnect ();
  
  OnSceneChanged ();  

  if (!new_camera)
    return;

  on_camera_updated = new_camera->RegisterEventHandler (NodeEvent_AfterUpdate, xtl::bind (&InputPort::OnCameraUpdated, this));
  on_scene_changed  = new_camera->RegisterEventHandler (NodeEvent_AfterSceneChange, xtl::bind (&InputPort::OnSceneChanged, this));
}

void InputPort::OnCameraUpdated ()
{
  need_update = true;
}

void InputPort::OnViewportChangeZOrder (int)
{
  z_order_changed = true;
}

void InputPort::OnViewportChangeActive (bool)
{
  is_active = viewport.IsActive () && viewport.InputState ();
}

void InputPort::OnViewportChangeInputState (bool)
{
  is_active = viewport.IsActive () && viewport.InputState ();
}

/*
    ������ ��
*/

void InputPort::SetTouchSize (float size, InputTransformSpace space)
{
  touch_size       = size;
  touch_size_space = space;
  need_update      = true;
}

/*
    ���������� ��ࠬ��஢ ������ �����
*/

void InputPort::Update ()
{  
  if (!need_update)
    return;        
    
  try
  {    
    Camera* camera = viewport.Camera ();
    
    if (!camera || !input_scene)
    {
      need_update = false;
      return;
    }
    
    const scene_graph::Rect& viewport_rect = viewport.Area ();

    math::vec3f viewport_offset (float (viewport_rect.left ()), float (viewport_rect.top ()), 0.0f),
                viewport_scale (1.0f / float (viewport_rect.right () - viewport_rect.left ()), 1.0f / float (viewport_rect.bottom () - viewport_rect.top ()), 1.0f);
                
    view_proj_tm           = camera->ProjectionMatrix () * math::inverse (camera->WorldTM ());                
    normalized_position_tm = math::translate (math::vec3f (-1.0f)) * math::scale (2.0f * viewport_scale) * math::translate (-viewport_offset);
    position_tm            = math::inverse (view_proj_tm) * normalized_position_tm;
    
    switch (touch_size_space)
    {
      case InputTransformSpace_Screen:
        touch_scale = math::vec3f (1.0f) / (math::vec3f (touch_size, touch_size, 1.0f) * viewport_scale);
        break;
      case InputTransformSpace_Camera:
      {
        math::vec4f normalized_scale = camera->ProjectionMatrix () * math::vec4f (touch_size, touch_size, 0.0f, 0.0f);
        
        touch_scale = abs (math::vec3f (2.0f / normalized_scale.x, 2.0f / normalized_scale.y, 1.0f));

        break;
      }
    }    

    need_update = false;
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::InputPort::Update");
    throw;
  }
}

/*
    ��ࠡ�⪠ ᮡ��� ������
*/

void InputPort::OnTouch (const TouchEvent& event, bool& touch_catched)
{
  try
  {        
    if (need_update)
      Update ();      
      
      //��ॢ�� ���न���� � ��஢�� ��⥬� ���न���

    math::vec4f source_position (event.position.x, event.position.y, 0.0f, 1.0f),
                world_position = position_tm * source_position;

    world_position /= world_position.w;    
    
      //����祭�� ��ࠬ��� ��
      
    math::vec4f normalized_position = normalized_position_tm * source_position;      

    normalized_position /= normalized_position.w;
    
    math::mat4f touch_tm = math::scale (touch_scale) * math::translate (math::vec3f (-normalized_position.x, -normalized_position.y)) * view_proj_tm;

    touch_frustum.clear ();

    add_frustum (touch_tm, touch_frustum);
    
      //�����饭�� �業� � ������������� ᮡ���
      
    input_scene->OnTouch (event, math::vec3f (world_position), touch_frustum, touch_catched);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::InputPort::OnTouch");
    throw;
  }
}

/*
    ���� ���ﭨ� ����⨩
*/

void InputPort::ResetTouchState ()
{
  if (!input_scene)
    return;
    
  input_scene->ResetTouchState ();
}
