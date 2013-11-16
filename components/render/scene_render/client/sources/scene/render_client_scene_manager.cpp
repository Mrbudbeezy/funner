#include "shared.h"

using namespace render::scene::client;

/*
    ���������
*/

namespace
{

const char* LOG_NAME = "render.scene.client"; //��� ������ ����������� ����������������

}

/*
    �������� ���������� ��������� ����
*/

namespace
{

struct SceneDesc
{
  Scene*               scene;
  xtl::auto_connection on_destroy_source_scene;
  xtl::auto_connection on_destroy_target_scene;

  SceneDesc (Scene* in_scene) : scene (in_scene) {}
};

typedef stl::hash_map<scene_graph::Scene*, SceneDesc> SceneMap;

struct NodeDesc
{
  NodePtr              node;
  xtl::auto_connection on_destroy_source_node;

  NodeDesc (const NodePtr& in_node) : node (in_node) {}
};

typedef stl::hash_map<scene_graph::Node*, NodeDesc> NodeMap;

}

struct SceneManager::Impl
{
  ClientImpl&      client;      //������
  client::Context& context;     //��������
  common::Log      log;         //����� ����������� ����������������
  SceneUpdateList  update_list; //������ ����������
  object_id_t      current_id;  //������� ��������� �������������  
  SceneMap         scenes;      //�����
  NodeMap          nodes;       //����

/// �����������
  Impl (ClientImpl& in_client, client::Context& in_context)
    : client (in_client)
    , context (in_context)
    , log (LOG_NAME)
    , current_id ()
  {
    ResetUpdateList ();
  }

/// ���������� �� �������� �����
  void OnDestroyScene (scene_graph::Scene* scene)
  {
    if (!scene)
      return;

    scenes.erase (scene);
  }

/// ���������� �� �������� ����
  void OnDestroyNode (scene_graph::Node* node)
  {
    if (!node)
      return;

    nodes.erase (node);
  }

/// ����� ������ ����������
  void ResetUpdateList ()
  {
    update_list.first = update_list.last = 0;
  }
};

/*
    ����������� / ����������
*/

SceneManager::SceneManager (ClientImpl& client, render::scene::client::Context& context)
  : impl (new Impl (client, context))
{
}

SceneManager::~SceneManager ()
{
}

/*
    ��������� �����
*/

ScenePtr SceneManager::GetScene (scene_graph::Scene& scene)
{
  try
  {
    SceneMap::iterator iter = impl->scenes.find (&scene);

    if (iter != impl->scenes.end ())
      return iter->second.scene;

    object_id_t& id = impl->current_id;

    if (!(id + 1))
      throw xtl::format_operation_exception ("", "ID pool is full for a new scene");

    ScenePtr new_scene (new Scene (scene, *this, id + 1), false);

    xtl::trackable::function_type destroy_handler (xtl::bind (&Impl::OnDestroyScene, &*impl, &scene));

    iter = impl->scenes.insert_pair (&scene, SceneDesc (new_scene.get ())).first;

    try
    {
      iter->second.on_destroy_source_scene = scene.Root ().RegisterEventHandler (scene_graph::NodeEvent_BeforeDestroy, xtl::bind (&Impl::OnDestroyScene, &*impl, &scene));
      iter->second.on_destroy_target_scene = new_scene->connect_tracker (destroy_handler);

      ++id;

      return new_scene;
    }
    catch (...)
    {
      impl->scenes.erase (iter);
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::SceneManager::GetScene");
    throw;
  }
}

/*
    ������ ����������
*/

SceneUpdateList& SceneManager::UpdateList ()
{
  return impl->update_list;
}

/*
    ������
*/

ClientImpl& SceneManager::Client ()
{
  return impl->client;
}

/*
    ��������
*/

Context& SceneManager::Context ()
{
  return impl->context;
}

/*
    ��������� ����
*/

NodePtr SceneManager::GetNode (scene_graph::Node& src_node)
{
  try
  {
    NodeMap::iterator iter = impl->nodes.find (&src_node);

    if (iter != impl->nodes.end ())
      return iter->second.node;

    NodePtr node (SceneFactory::Create (src_node, *this), false);

    iter = impl->nodes.insert_pair (&src_node, node).first;

    if (node)
    {
      try
      {
        iter->second.on_destroy_source_node = src_node.RegisterEventHandler (scene_graph::NodeEvent_BeforeDestroy, xtl::bind (&Impl::OnDestroyNode, &*impl, &src_node));
      }
      catch (...)
      {
        impl->nodes.erase (iter);
        throw;
      }
    }

    return node;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::SceneManager::GetNode");
    throw;
  }
}

/*
    ������������� ����
*/

void SceneManager::Update ()
{
  SceneObject* first = impl->update_list.first;

  impl->ResetUpdateList ();

  client::Context& context = impl->context;

  for (SceneObject* object=first; object; object=object->next_update)
  {
    try
    {
      object->Update (context);
    }
    catch (std::exception& e)
    {
      impl->log.Printf ("%s\n    at render::scene::client::SceneManager::Update", e.what ());
    }
    catch (...)
    {
      impl->log.Printf ("unknown exception\n    at render::scene::client::SceneManager::Update");
    }
  }
}
