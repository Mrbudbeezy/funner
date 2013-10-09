#include "shared.h"

using namespace render::scene::client;

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

}

struct SceneManager::Impl
{
  SceneMap scenes; //�����

/// ���������� �� �������� �����
  void OnDestroyScene (scene_graph::Scene* scene)
  {
    if (!scene)
      return;

    scenes.erase (scene);
  }
};

/*
    ����������� / ����������
*/

SceneManager::SceneManager ()
  : impl (new Impl)
{
}

SceneManager::~SceneManager ()
{
}

/*
    ��������� �����
*/

ScenePtr SceneManager::GetScene (scene_graph::Scene& scene, Connection& connection)
{
  try
  {
    SceneMap::iterator iter = impl->scenes.find (&scene);

    if (iter != impl->scenes.end ())
      return iter->second.scene;

    ScenePtr new_scene (new Scene (scene, connection), false);

    xtl::trackable::function_type destroy_handler (xtl::bind (&Impl::OnDestroyScene, &*impl, &scene));

    iter = impl->scenes.insert_pair (&scene, SceneDesc (new_scene.get ())).first;

    try
    {
      iter->second.on_destroy_source_scene = scene.Root ().RegisterEventHandler (scene_graph::NodeEvent_BeforeDestroy, xtl::bind (&Impl::OnDestroyScene, &*impl, &scene));
      iter->second.on_destroy_target_scene = new_scene->connect_tracker (destroy_handler);

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
