#include "shared.h"

using namespace render::scene::client;

/*
    �������� ���������� ����
*/

struct Node::Impl
{
  scene_graph::Node&   node;                  //�������� ����
  xtl::auto_connection on_update_connection;  //���������� �� ���������� �������

/// �����������
  Impl (scene_graph::Node& in_node)
    : node (in_node)
  {
  }
};

/*
    ����������� / ����������
*/

Node::Node (scene_graph::Node& node, SceneManager& scene_manager)
  : SceneObject (scene_manager)
  , impl (new Impl (node))
{
  impl->on_update_connection = node.RegisterEventHandler (scene_graph::NodeEvent_AfterUpdate, xtl::bind (&Node::UpdateNotify, static_cast<SceneObject*> (this)));
}

Node::~Node ()
{
}

/*
    �������� ����
*/

scene_graph::Node& Node::SourceNode ()
{
  return impl->node;
}

/*
    ���������� �������������
*/

void Node::UpdateCore (render::scene::client::Context& context)
{
  //?????????????
}

/*
    ��� �������
*/

const char* Node::NameCore ()
{
  return impl->node.Name ();
}
