#include "shared.h"

using namespace scene_graph;
using namespace input;

/*
    ����������� / ����������
*/

namespace
{

struct SceneCreationTraverser: public INodeTraverser
{
  InputScene& scene;
  
  SceneCreationTraverser (InputScene& in_scene) : scene (in_scene) {}
  
  void operator () (Node& node)
  {
    scene_graph::InputZoneModel* zone = dynamic_cast<scene_graph::InputZoneModel*> (&node);

    if (!zone)
      return;  
      
    scene.GetEntity (*zone);      
  }
};

}

InputScene::InputScene (scene_graph::Scene& in_scene)
  : scene (in_scene)
{
  on_entity_created_connection = scene.Root ().RegisterEventHandler (NodeSubTreeEvent_AfterBind, xtl::bind (&InputScene::OnInputZoneCreated, this, _2));
  
  SceneCreationTraverser traverser (*this);
    
  scene.Traverse (traverser);  
}

InputScene::~InputScene ()
{
  entities.clear ();
  
  DetachAll ();
}

/*
    ����� ��������� �������
*/

void InputScene::ResetTouchState ()
{
  entities.clear ();
}

/*
    ��������� ������� �����
*/

InputEntityPtr InputScene::GetEntity (const scene_graph::InputZoneModel& zone)
{
  try
  {
    EntityMap::iterator iter = entities.find (&zone);
    
    if (iter != entities.end ())
      return iter->second.entity;

    EntityDesc& desc = entities [&zone];
    
    try
    {
      desc.entity               = InputEntityPtr (new InputEntity (zone, *this), false);
      desc.on_unbind_connection = zone.RegisterEventHandler (NodeEvent_BeforeSceneDetach, xtl::bind (&InputScene::OnInputZoneDestroyed, this, &zone));
      
      return desc.entity;
    }
    catch (...)
    {
      entities.erase (&zone);
      throw;
    }      
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::InputScene::GetEntity");
    throw;
  }
}

/*
    ���������� � �������� ���� �����
*/

void InputScene::OnInputZoneCreated (Node& node)
{
  try
  {
    InputZoneModel* zone = dynamic_cast<InputZoneModel*> (&node);
  
    if (!zone)
      return;
    
    GetEntity (*zone);
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::InputScene::OnInputZoneCreated");
    throw;
  }
}

/*
    ���������� �� �������� ���� �����
*/

void InputScene::OnInputZoneDestroyed (const scene_graph::InputZoneModel* zone)
{
  entities.erase (zone);
}

/*
    ��������� ������� �������
*/

namespace
{

struct TouchTraverser: public INodeTraverser
{
  InputScene&        scene;
  const math::vec3f& touch_world_position;
  const math::vec3f& touch_world_direction;
  const frustum&     touch_frustum;
  bool               intersected;
  bool               quasi_intersected;
  float              min_distance;
  InputZoneModel*    input_zone;
  math::vec2f        input_zone_intersection_point;
  size_t             input_zone_index;
  
  TouchTraverser (InputScene&        in_scene,
                  const math::vec3f& in_touch_world_position,
                  const math::vec3f& in_touch_world_direction,
                  const frustum&     in_touch_frustum)
    : scene (in_scene)
    , touch_world_position (in_touch_world_position)
    , touch_world_direction (in_touch_world_direction)
    , touch_frustum (in_touch_frustum)
    , intersected (false)
    , quasi_intersected (false)
    , min_distance (0.0f)
    , input_zone ()
    , input_zone_index ()
  {
  }

  void operator () (Node& node)
  {
    scene_graph::InputZoneModel* zone = dynamic_cast<scene_graph::InputZoneModel*> (&node);

    if (!zone || !zone->IsActive ())
      return;

    float       ray_intersection_distance = 0.0f;
    math::vec2f intersection_point;
    math::vec3f ray_to_zone_offset;
    size_t      zone_index = 0;

    bool zone_intersection = zone->IsIntersected (NodeTransformSpace_World, touch_world_position, touch_world_position + touch_world_direction, zone_index, 
      ray_intersection_distance, ray_to_zone_offset, intersection_point);          

    if (zone_intersection)
    {
      if (intersected)
      {
        if (ray_intersection_distance >= min_distance)
          return;
      }
      else
      {
        intersected = true;
      }      
      
      min_distance                  = ray_intersection_distance;
      input_zone                    = zone;
      input_zone_intersection_point = intersection_point;
    }
    else if (!intersected)
    {
      if (ray_intersection_distance < 0.0f || ray_intersection_distance > 1.0f) //check for z distance
        return;        
      
      float ray_to_zone_distance = length (ray_to_zone_offset);
      
      if (quasi_intersected)
      {        
        if (ray_to_zone_distance >= min_distance)
          return;
      }
      else
      {
        quasi_intersected = true;
      }
      
      math::vec3f nearest_point = touch_world_position + touch_world_direction * ray_intersection_distance + ray_to_zone_offset;
      
      if (!contains (touch_frustum, nearest_point)) //check if nearest point to ray is in frustum
        return;

      min_distance                  = ray_to_zone_distance;
      input_zone                    = zone;
      input_zone_intersection_point = intersection_point;
      input_zone_index              = zone_index;
    }
  }
};

}

void InputScene::OnTouch (InputPort& input_port, const TouchEvent& event, const math::vec3f& touch_world_position, const math::vec3f& touch_world_direction, const frustum& touch_frustum, bool& touch_catched)
{
  try
  {        
      //����� ����, ������������ �������� ����
    
    TouchTraverser traverser (*this, touch_world_position, touch_world_direction, touch_frustum);
    
    scene.Traverse (touch_frustum, traverser);
    
    if (traverser.input_zone && !touch_catched)
    {
      touch_catched = true;
    
        //��������� �������, ���������������� ����
      
      InputEntityPtr entity = GetEntity (*traverser.input_zone);
    
      if (entity)
      {          
          //�������� ������� ���������������� �������      
          
        entity->OnTouch (input_port, event, touch_world_position, traverser.input_zone_index, traverser.input_zone_intersection_point);
      }
    }
    
      //����������
      
    BroadcastTouch (input_port, event, touch_world_position);        
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::InputScene::OnTouch(const TouchEvent&,const math::vec3f&,const frustum&,bool&)");
    throw;
  }  
}
    
/*
    ����������� ���������� ����������
*/

void InputScene::RegisterBroadcastListener (InputEventListener& listener)
{
  listener.Attach (*this);
}

/*
    �������� ���� �������, ��������� � ��������� �������� �����
*/

void InputScene::RemoveAllTouches (InputPort& input_port)
{
  List::RemoveAllTouches (input_port);
}
