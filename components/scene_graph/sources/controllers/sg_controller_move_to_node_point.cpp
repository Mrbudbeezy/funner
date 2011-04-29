#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::controllers;

/*
    �������� ���������� ����������� ���������� �� ������ ����
*/

struct MoveToNodePoint::Impl
{
  Node*                node;                    //������������� ����
  AccelerationFunction acceleration_function;   //������� �������� ��������� ����
  Node::ConstPointer   target_node;             //������������ ����
  math::vec3f          target_point;            //������� ����� � ������� ��������� ������������� ����
  math::vec3f          current_speed;           //������� �������� ����
  NodeTransformSpace   transform_space;         //������������ �������� ��������
  xtl::auto_connection scene_attach_connection; //���������� ������� ������������� ���� � �����

  Impl (Node& in_node)
    : node (&in_node)
    , current_speed (0.f)
    , transform_space (NodeTransformSpace_Local)
  {
    scene_attach_connection = node->RegisterEventHandler (NodeEvent_AfterSceneAttach, xtl::bind (&MoveToNodePoint::Impl::OnSceneAttach, this));
  }

  void OnSceneAttach ()
  {
    current_speed = 0.f;
  }
};

/*
    ����������� / ����������
*/

MoveToNodePoint::MoveToNodePoint (Node& node)
  : Controller (node)
  , impl (new Impl (node))
  {}

MoveToNodePoint::~MoveToNodePoint ()
{
}

/*
    �������� �����������
*/

MoveToNodePoint::Pointer MoveToNodePoint::Create (Node& node)
{
  return Pointer (new MoveToNodePoint (node), false);
}

/*
   ���������/��������� ������� �������� ��������� ����
*/

void MoveToNodePoint::SetAccelerationHandler (const AccelerationFunction& acceleration_function)
{
  impl->acceleration_function = acceleration_function;
}

const MoveToNodePoint::AccelerationFunction& MoveToNodePoint::AccelerationHandler () const
{
  return impl->acceleration_function;
}

/*
   ��������� / ��������� ������������ �������� ��������
*/

void MoveToNodePoint::SetTransformSpace (NodeTransformSpace transform_space)
{
  switch (transform_space)
  {
    case NodeTransformSpace_Local:
    case NodeTransformSpace_World:
      impl->transform_space = transform_space;
      return;
    default:
      throw xtl::make_argument_exception ("scene_graph::controllers::MoveToNodePoint", "transform_space", transform_space);
  }
}

NodeTransformSpace MoveToNodePoint::TransformSpace () const
{
  return impl->transform_space;
}

/*
   ������ ��������
*/

void MoveToNodePoint::Start (Node::ConstPointer node, const math::vec3f& node_space_position)
{
  impl->target_node  = node;
  impl->target_point = node_space_position;
}

/*
   ��������� ��������
*/

void MoveToNodePoint::Stop ()
{
  impl->target_node = 0;
}

/*
    ����������
*/

void MoveToNodePoint::Update (float dt)
{
  static const char* METHOD_NAME = "scene_graph::controllers::MoveToNodePoint::Update";

  if (!impl->node->Scene ())
    throw xtl::format_operation_exception (METHOD_NAME, "Can't move node, node is not attached to any scene");

  if (impl->target_node && !impl->target_node->Scene ())
    throw xtl::format_operation_exception (METHOD_NAME, "Can't move node, target node is not attached to any scene");

  if (!impl->acceleration_function)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't move node, empty acceleration function");

  math::vec3f current_position = impl->node->Position (),
              acceleration;

  if (impl->target_node)
  {
    math::vec3f move_direction = (impl->target_node->WorldPosition () + impl->target_node->WorldTM () * impl->target_point) - impl->node->WorldPosition ();

    if (impl->transform_space == NodeTransformSpace_Local)
      move_direction = math::inverse (impl->node->WorldTM ()) * move_direction;

    acceleration = impl->acceleration_function (impl->current_speed, move_direction, dt);
  }
  else
    acceleration = impl->acceleration_function (impl->current_speed, 0, dt);

  impl->node->Translate (impl->current_speed * dt + acceleration * dt * dt / 2.f, impl->transform_space);

  impl->current_speed += acceleration * dt;
}
