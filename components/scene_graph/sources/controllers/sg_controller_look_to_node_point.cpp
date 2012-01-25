#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::controllers;

namespace
{

const float EPS = 0.0001f;

}

/*
    �������� ���������� ����������� ������������ ���������� �� ����� ����
*/

struct LookToNodePoint::Impl
{
  Node*                node;                    //������������� ����
  AccelerationFunction acceleration_function;   //������� �������� ��������� ����
  Node::ConstPointer   target_node;             //������������ ����
  math::vec3f          target_point;            //������� ����� � ������� ��������� ������������� ����
  NodeOrt              look_axis;               //��� ����, ������� ������ ���� ���������� �� �����
  NodeOrt              rotation_axis;           //��� ����, ������ ������� ������������ ��������
  math::vec3f          current_speed;           //������� �������� ����
  xtl::auto_connection scene_attach_connection; //���������� ������� ������������� ���� � �����

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
    ����������� / ����������
*/

LookToNodePoint::LookToNodePoint (Node& node)
  : Controller (node, ControllerTimeMode_Delta)
  , impl (new Impl (node))
  {}

LookToNodePoint::~LookToNodePoint ()
{
}

/*
    �������� �����������
*/

LookToNodePoint::Pointer LookToNodePoint::Create (Node& node)
{
  return Pointer (new LookToNodePoint (node), false);
}

/*
   ���������/��������� ������� �������� ��������� ����
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
   ������ ��������
*/

void LookToNodePoint::Start (Node::ConstPointer node, const math::vec3f& node_space_position, NodeOrt look_axis, NodeOrt rotation_axis)
{
  impl->target_node   = node;
  impl->target_point  = node_space_position;
  impl->rotation_axis = rotation_axis;
  impl->look_axis     = look_axis;
}

/*
   ��������� ��������
*/

void LookToNodePoint::Stop ()
{
  impl->target_node = 0;
}

/*
    ����������
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
