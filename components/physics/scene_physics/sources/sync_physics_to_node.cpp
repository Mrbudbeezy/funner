#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::controllers;
using namespace physics;

/*
   ���������� ��������������� ������������� ���� � ������������ � �������������� ����
*/

struct SyncPhysicsToNode::Impl
{
  Node*                node;
  physics::RigidBody   body;
  bool                 needs_update;
  xtl::auto_connection body_update_connection;

  Impl (Node& in_node, const physics::RigidBody& in_body)
    : node (&in_node)
    , body (in_body)
    , needs_update (true)
  {
    body_update_connection = body.RegisterTransformUpdateCallback (xtl::bind (&SyncPhysicsToNode::Impl::OnTransformUpdate, this));
  }

  void OnTransformUpdate ()
  {
    needs_update = true;
  }

  void Update ()
  {
    if (!needs_update)
      return;

    const Transform& body_transform = body.WorldTransform ();

    node->SetWorldPosition    (body_transform.position);
    node->SetWorldOrientation (body_transform.orientation);

    needs_update = false;
  }
};

/*
   ����������� / ����������
*/

SyncPhysicsToNode::SyncPhysicsToNode (Node& node, const physics::RigidBody& body)
  : Controller (node)
  , impl (new Impl (node, body))
  {}

SyncPhysicsToNode::~SyncPhysicsToNode ()
{
}

/*
   �������� �����������
*/

SyncPhysicsToNode::Pointer SyncPhysicsToNode::Create (Node& node, const physics::RigidBody& body)
{
  return Pointer (new SyncPhysicsToNode (node, body), false);
}

/*
   ����, � ������� ������ ����������
*/

const physics::RigidBody& SyncPhysicsToNode::AttachedBody () const
{
  return impl->body;
}

/*
   ����������
*/

void SyncPhysicsToNode::Update (const TimeValue&)
{
  impl->Update ();
}
