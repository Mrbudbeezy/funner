#include "shared.h"

using namespace render::scene::client;

/*
    �������� ���������� �����
*/

struct Scene::Impl
{
  object_id_t         id;          //������������� �����
  scene_graph::Scene& scene;       //�����
  client::Connection& connection;  //����������
  SceneUpdateList&    update_list; //������ ����������

/// �����������
  Impl (scene_graph::Scene& in_scene, client::Connection& in_connection, SceneUpdateList& in_update_list, object_id_t in_id)
    : id (in_id)
    , scene (in_scene)
    , connection (in_connection)
    , update_list (in_update_list)
  {
    connection.Context ().CreateScene (id);
    connection.Context ().SetSceneName (id, scene.Name ());
  }

/// ����������
  ~Impl ()
  {
    try
    {
      connection.Context ().DestroyScene (id);
    }
    catch (...)
    {
    }
  }
};

/*
    ����������� / ����������
*/

Scene::Scene (scene_graph::Scene& scene, render::scene::client::Connection& connection, SceneUpdateList& update_list, object_id_t id)
{
  try
  {
    impl.reset (new Impl (scene, connection, update_list, id));
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

/*
    ����������
*/

Connection& Scene::Connection ()
{
  return impl->connection;
}

/*
    ������ ����������
*/

SceneUpdateList& Scene::UpdateList ()
{
  return impl->update_list;
}
