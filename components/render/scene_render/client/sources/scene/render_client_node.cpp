#include "shared.h"

using namespace render::scene::client;

/*
    �������� ���������� ����
*/

struct Node::Impl
{  
  scene_graph::Node&    node;                          //�������� ����
  SceneManager&         scene_manager;                 //�������� ����
  interchange::NodeType node_type;                     //��� ����
  object_id_t           id;                            //������������� �������
  xtl::auto_connection  on_update_connection;          //���������� �� ���������� �������  
  xtl::auto_connection  on_update_world_tm_connection; //���������� �� ���������� ������� ������� ��������������
  size_t                name_hash;                     //��� ����� ����  
  bool                  need_update_world_tm;          //��������� ���������� ������� ��������������

/// �����������
  Impl (scene_graph::Node& in_node, SceneManager& in_scene_manager, interchange::NodeType in_node_type)
    : node (in_node)
    , scene_manager (in_scene_manager)
    , node_type (in_node_type)
    , id (scene_manager.Client ().AllocateId (ObjectType_Node))
    , name_hash (0)
    , need_update_world_tm (true)
  {
    scene_manager.Context ().CreateNode (id, node_type);
  }

/// ����������
  ~Impl ()
  {
    try
    {
      scene_manager.Context ().DestroyNode (id);
    }
    catch (...)
    {
    }
  }

/// ���������� �� ���������� ������� ��������������
  void UpdateWorldMatrixNotify ()
  {
    need_update_world_tm = true;
  }
};

/*
    ����������� / ����������
*/

Node::Node (scene_graph::Node& node, SceneManager& scene_manager, render::scene::interchange::NodeType node_type)
  : SceneObject (scene_manager)
{
  try
  {
    impl.reset (new Impl (node, scene_manager, node_type));

    impl->on_update_connection          = node.RegisterEventHandler (scene_graph::NodeEvent_AfterUpdate, xtl::bind (&Node::UpdateNotify, static_cast<SceneObject*> (this)));
    impl->on_update_world_tm_connection = node.RegisterEventHandler (scene_graph::NodeEvent_AfterWorldTransformUpdate, xtl::bind (&Impl::UpdateWorldMatrixNotify, &*impl));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Node::Node");
    throw;
  }
}

Node::~Node ()
{
}

/*
    ������������� ���� / ��� ����
*/

object_id_t Node::Id () const
{
  return impl->id;
}

render::scene::interchange::NodeType Node::Type () const
{
  return impl->node_type;
}

/*
    �������� ����
*/

scene_graph::Node& Node::SourceNode () const
{
  return impl->node;
}

/*
    �������� �����
*/

SceneManager& Node::Scenes () const
{
  return impl->scene_manager;
}

/*
    ���������� �������������
*/

void Node::UpdateCore (render::scene::client::Context& context)
{
  try
  {
      //���������� ����� ����

    size_t name_hash = impl->node.NameHash ();

    if (impl->name_hash != impl->node.NameHash ())
    {
      context.SetNodeName (impl->id, impl->node.Name ());

      impl->name_hash = name_hash;
    }

      //���������� ������� ��������������

    if (impl->need_update_world_tm)
    {
      context.SetNodeWorldMatrix (impl->id, impl->node.WorldTM ());

      impl->need_update_world_tm = false;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Node::UpdateCore");
    throw;
  }
}

/*
    ��� �������
*/

const char* Node::NameCore ()
{
  return impl->node.Name ();
}
