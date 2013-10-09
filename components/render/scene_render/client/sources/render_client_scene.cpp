#include "shared.h"

using namespace render::scene::client;

/*
    �������� ���������� �����
*/

struct Scene::Impl
{
  scene_graph::Scene& scene;      //�����
  Connection&         connection; //����������

/// �����������
  Impl (scene_graph::Scene& in_scene, Connection& in_connection)
    : scene (in_scene)
    , connection (in_connection)
  {
  }
};

/*
    ����������� / ����������
*/

Scene::Scene (scene_graph::Scene& scene, Connection& connection)  
{
  try
  {
    impl.reset (new Impl (scene, connection));
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
    �����-��������
*/

scene_graph::Scene& Scene::SourceScene ()
{
  return impl->scene;
}

/*
    �������������
*/

void Scene::Update ()
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Scene::Update");
    throw;
  }
}
