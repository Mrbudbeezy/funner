#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::controllers;

namespace
{

const float EPS = 0.0001f;

}

/*
    Описание реализации контроллера выравнивания ориентации на точку узла
*/

struct LookToNodePoint::Impl: public xtl::instance_counter<LookToNodePoint>
{
  Node*                node;                    //передвигаемый узел
  AccelerationFunction acceleration_function;   //функция рассчета ускорения узла
  Node::ConstPointer   target_node;             //преследуемый узел
  math::vec3f          target_point;            //целевая точка в системе координат преследуемого узла
  NodeOrt              look_axis;               //ось узла, которая должна быть направлена на точку
  NodeOrt              rotation_axis;           //ось узла, вокруг которой производится вращение
  math::vec3f          current_speed;           //текущая скорость узла
  xtl::auto_connection scene_attach_connection; //соединения события присоединения узла к сцене

  Impl (Node& in_node)
    : node (&in_node)
    , look_axis (NodeOrt_Z)
    , rotation_axis (NodeOrt_Y)
    , current_speed (0.f)
  {
    scene_attach_connection = node->RegisterEventHandler (NodeEvent_AfterSceneAttach, xtl::bind (&LookToNodePoint::Impl::OnSceneAttach, this));
  }

  void OnSceneAttach ()
  {
    current_speed = 0.f;
  }
};

/*
    Конструктор / деструктор
*/

LookToNodePoint::LookToNodePoint (Node& node)
  : Controller (node, ControllerTimeMode_Delta)
  , impl (new Impl (node))
  {}

LookToNodePoint::~LookToNodePoint ()
{
}

/*
    Создание контроллера
*/

LookToNodePoint::Pointer LookToNodePoint::Create (Node& node)
{
  return Pointer (new LookToNodePoint (node), false);
}

/*
   Установка/получение функции рассчета ускорения узла
*/

void LookToNodePoint::SetAccelerationHandler (const AccelerationFunction& acceleration_function)
{
  impl->acceleration_function = acceleration_function;
}

const LookToNodePoint::AccelerationFunction& LookToNodePoint::AccelerationHandler () const
{
  return impl->acceleration_function;
}

/*
   Запуск движения
*/

void LookToNodePoint::Start (const Node& node, const math::vec3f& node_space_position, NodeOrt look_axis, NodeOrt rotation_axis)
{
  impl->target_node   = &node;
  impl->target_point  = node_space_position;
  impl->rotation_axis = rotation_axis;
  impl->look_axis     = look_axis;
}

/*
   Остановка движения
*/

void LookToNodePoint::Stop ()
{
  impl->target_node = 0;
}

/*
    Обновление
*/

void LookToNodePoint::Update (const TimeValue& time_value)
{
  static const char* METHOD_NAME = "scene_graph::controllers::LookToNodePoint::Update";
  
  float dt = time_value.cast<float> ();    

  if (!impl->node->Scene ())
    throw xtl::format_operation_exception (METHOD_NAME, "Can't look to node, node is not attached to any scene");

  if (impl->target_node && !impl->target_node->Scene ())
    throw xtl::format_operation_exception (METHOD_NAME, "Can't look to node, target node is not attached to any scene");

  if (!impl->acceleration_function)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't look to node, empty acceleration function");

  math::vec3f current_position (impl->node->Ort (impl->look_axis, NodeTransformSpace_World)), acceleration;

  static const math::vec3f zero_vec (0.f);

  if (impl->target_node)
  {
    math::vec3f desired_direction = impl->target_node->WorldTM () * impl->target_point - impl->node->WorldTM () * impl->node->PivotPosition ();

    desired_direction = math::equal (desired_direction, zero_vec, EPS) ? 0.f : math::normalize (desired_direction);

    acceleration = impl->acceleration_function (impl->current_speed, desired_direction - current_position, dt);
  }
  else
    acceleration = impl->acceleration_function (impl->current_speed, 0, dt);

  impl->current_speed += acceleration * dt;

  math::vec3f new_direction = current_position + impl->current_speed * dt;

  if (!math::equal (new_direction, zero_vec, EPS))
    new_direction = math::normalize (new_direction);

  impl->node->LookTo (impl->node->WorldPosition () + new_direction, impl->look_axis, impl->rotation_axis, NodeTransformSpace_World);
}
