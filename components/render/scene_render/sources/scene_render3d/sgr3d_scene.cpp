#include "shared.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4355) // this used in base initializer list
#endif

using namespace render;
using namespace render::scene_render3d;

namespace
{

/*
    ������� ��������
*/

struct RenderableFactory: public xtl::visitor<Renderable*, scene_graph::VisualModel>
{
  Scene& scene;  
  
  RenderableFactory (Scene& in_scene) : scene (in_scene) {}
  
  Renderable* visit (scene_graph::VisualModel& entity)
  {
    try
    {
      return new VisualModel (scene, entity);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene_render3d::RenderableFactory::visit(scene_graph::VisualModel&)");
      throw;
    }
  }
  
  Renderable* CreateRenderable (scene_graph::Entity& entity)
  {
    try
    {
      return (*this)(entity);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene_render3d::RenderableFactory::CreateRenderable");
      throw;
    }
  }
};

}

/*
    �������� ���������� ����� ����������
*/

typedef stl::hash_map<scene_graph::Entity*, RenderablePtr> RenderableMap;

struct Scene::Impl
{
  RenderPtr            render;                     //������
  scene_graph::Scene*  scene;                      //�������� �����
  xtl::auto_connection scene_destroyed_connection; //���������� � ������������ ������� �������� �����
  RenderableMap        renderables;                //����� ��������
  RenderableFactory    factory;                    //������� ��������
  Log                  log;                        //����� ����������������

///�����������
  Impl (Scene& owner, scene_graph::Scene& in_scene, const RenderPtr& in_render)
    : render (in_render)
    , scene (&in_scene)
    , scene_destroyed_connection (in_scene.Root ().RegisterEventHandler (scene_graph::NodeEvent_BeforeDestroy, xtl::bind (&Impl::OnDestroyScene, this)))
    , factory (owner)
  {
    log.Printf ("Scene created");
  }
  
///����������
  ~Impl ()
  {
    log.Printf ("Scene destroyed");
  }
  
///���������� �� �������� �����
  void OnDestroyScene ()
  {
    if (scene)
      render->UnregisterScene (*scene);    
      
    scene = 0;
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
    
    impl = new Impl (*this, scene, render);
    
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
    if (impl->scene)
      impl->render->UnregisterScene (*impl->scene);
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

/*
    ����������� ��������
*/

void Scene::RegisterRenderable (scene_graph::Entity& entity, Renderable& renderable)
{
  impl->renderables.insert_pair (&entity, &renderable);
}

void Scene::UnregisterRenderable (scene_graph::Entity& entity)
{
  impl->renderables.erase (&entity);
}

/*
    ��������� ������� �����
*/

RenderablePtr Scene::GetRenderable (scene_graph::Entity& entity)
{
  try
  {
    RenderableMap::iterator iter = impl->renderables.find (&entity);
    
    if (iter != impl->renderables.end ())
      return iter->second;
      
    return RenderablePtr (impl->factory.CreateRenderable (entity), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::Scene::GetRenderable");
    throw;
  }
}
