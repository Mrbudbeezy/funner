#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::controllers;

namespace
{

const float EPS = 0.0001f;

}

/*
    �������� ���������� ����������� ������������ ������ ���� � ������ �����
*/

struct AlignWithNode::Impl
{
  Node*                node;                    //������������� ����
  AccelerationFunction acceleration_function;   //������� �������� ��������� ����
  Node::ConstPointer   target_node;             //����, �� ������� ���� ������������
  NodeOrt              target_node_axis;        //��� ����, � ������� ���� ������������
  NodeOrt              node_axis;               //��� ����, ������� ������ ���� ���������
  NodeOrt              node_rotation_axis;      //��� ����, ������ ������� ������������ ��������
  math::vec3f          current_speed;           //������� �������� ����
  xtl::auto_connection scene_attach_connection; //���������� ������� ������������� ���� � �����

  Impl (Node& in_node)
    : node (&in_node)
    , current_speed (0.f)
  {
    scene_attach_connection = node->RegisterEventHandler (NodeEvent_AfterSceneAttach, xtl::bind (&AlignWithNode::Impl::OnSceneAttach, this));
  }

  void OnSceneAttach ()
  {
    current_speed = 0.f;
  }
};

/*
    ����������� / ����������
*/

AlignWithNode::AlignWithNode (Node& node)
  : Controller (node)
  , impl (new Impl (node))
  {}

AlignWithNode::~AlignWithNode ()
{
}

/*
    �������� �����������
*/

AlignWithNode::Pointer AlignWithNode::Create (Node& node)
{
  return Pointer (new AlignWithNode (node), false);
}

/*
   ���������/��������� ������� �������� ��������� ����
*/

void AlignWithNode::SetAccelerationHandler (const AccelerationFunction& acceleration_function)
{
  impl->acceleration_function = acceleration_function;
}

const AlignWithNode::AccelerationFunction& AlignWithNode::AccelerationHandler () const
{
  return impl->acceleration_function;
}

/*
   ������ ��������
*/

void AlignWithNode::Start (Node::ConstPointer target_node, NodeOrt target_node_axis, NodeOrt node_axis, NodeOrt node_rotation_axis)
{
  impl->target_node        = target_node;
  impl->target_node_axis   = target_node_axis;
  impl->node_rotation_axis = node_rotation_axis;
  impl->node_axis          = node_axis;
}

/*
   ��������� ��������
*/

void AlignWithNode::Stop ()
{
  impl->target_node = 0;
}

/*
    ����������
*/

void AlignWithNode::Update (float dt)
{
  static const char* METHOD_NAME = "scene_graph::controllers::AlignWithNode::Update";

  if (!impl->target_node)
    return;

  if (!impl->node->Scene ())
    throw xtl::format_operation_exception (METHOD_NAME, "Can't align with node, node is not attached to any scene");

  if (!impl->target_node->Scene ())
    throw xtl::format_operation_exception (METHOD_NAME, "Can't align with node node, target node is not attached to any scene");

  if (!impl->acceleration_function)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't align with node, empty acceleration function");

  math::vec3f current_position  = impl->node->Ort (impl->node_axis, NodeTransformSpace_World),
              desired_direction = impl->target_node->Ort (impl->target_node_axis, NodeTransformSpace_World),
              acceleration      = impl->acceleration_function (impl->current_speed, desired_direction - current_position, dt);

  impl->current_speed += acceleration * dt;

  math::vec3f new_direction = current_position + impl->current_speed * dt;

  static const math::vec3f zero_vec (0.f);

  if (!math::equal (new_direction, zero_vec, EPS))
    new_direction = math::normalize (new_direction);

  impl->node->LookTo (impl->node->WorldPosition () + new_direction, impl->node_axis, impl->node_rotation_axis, NodeTransformSpace_World);
}
