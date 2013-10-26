#include "shared.h"

using namespace render::scene::client;

/*
    �������� ���������� �����
*/

struct Scene::Impl
{
  object_id_t          id;                      //������������� �����
  scene_graph::Scene&  scene;                   //�����
  SceneManager&        scene_manager;           //�������� ����
  xtl::auto_connection on_node_bind_connection; //���������� � ��������� ������ ���� � �����

/// �����������
  Impl (scene_graph::Scene& in_scene, SceneManager& in_scene_manager, object_id_t in_id)
    : id (in_id)
    , scene (in_scene)
    , scene_manager (in_scene_manager)
  {
    on_node_bind_connection = scene.Root ().RegisterEventHandler (scene_graph::NodeSubTreeEvent_AfterBind, xtl::bind (&Impl::OnNodeBinded, this, _2));

    scene_manager.Context ().CreateScene (id);
    scene_manager.Context ().SetSceneName (id, scene.Name ());
  }

/// ����������
  ~Impl ()
  {
    try
    {
      scene_manager.Context ().DestroyScene (id);
    }
    catch (...)
    {
    }
  }

/// ���������� � ��������� ������ ���� � �����
  void OnNodeBinded (scene_graph::Node& src_node)
  {
    try
    {
      NodePtr node = scene_manager.GetNode (src_node);

      if (!node)
        return;

      ///???????????????? attach to scene
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::client::Scene::Impl::OnNodeBinded");
      throw;
    }
  }
};

/*
    ����������� / ����������
*/

Scene::Scene (scene_graph::Scene& scene, SceneManager& scene_manager, object_id_t id)
{
  try
  {
    impl.reset (new Impl (scene, scene_manager, id));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Scene::Scene");
    throw;
  }
}

Scene::~Scene ()
{
}

/*
    ������������� �����
*/

object_id_t Scene::Id ()
{
  return impl->id;
}

/*
    �����-��������
*/

scene_graph::Scene& Scene::SourceScene ()
{
  return impl->scene;
}
