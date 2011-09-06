#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

//TODO: scene trackable

/*
    �������� ���������� ����� ����������
*/

struct Scene::Impl
{
  RenderPtr           render; //������
  scene_graph::Scene* scene;  //�������� �����
  Log                 log;    //����� ����������������
  
///�����������
  Impl (scene_graph::Scene& in_scene, const RenderPtr& in_render)
    : render (in_render)
    , scene (&in_scene)
  {
    log.Printf ("Scene created");    
  }
  
///����������
  ~Impl ()
  {
    log.Printf ("Scene destroyed");
  }
};

/*
    ����������� / ����������
*/

Scene::Scene (scene_graph::Scene& scene, const RenderPtr& render)
{
  try
  {
    if (!render)
      throw xtl::make_null_argument_exception ("", "render");    
    
    impl = new Impl (scene, render);
    
    render->RegisterScene (scene, *this);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::Scene::Scene");
    throw;
  }
}

Scene::~Scene ()
{
  try
  {
    impl->render->UnregisterScene (*impl->scene);
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}
