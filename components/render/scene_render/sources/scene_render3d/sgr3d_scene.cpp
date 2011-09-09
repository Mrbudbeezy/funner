#include "shared.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4355) // this used in base initializer list
#endif

using namespace render;
using namespace render::scene_render3d;

      //TODO: ������ �� ����������� ����� ��������!!!!!!!!!!!!!!!!!

namespace
{

/*
    ������� ��������
*/

struct EntityFactory: public xtl::visitor<void, scene_graph::VisualModel, scene_graph::PointLight, scene_graph::DirectLight, scene_graph::SpotLight>
{
  Scene& scene;
  Node*  result;
  
  EntityFactory (Scene& in_scene) : scene (in_scene), result () {}
  
  void visit (scene_graph::PointLight& entity)
  {
    try
    {
      result = new Light (scene, entity);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene_render3d::EntityFactory::visit(scene_graph::PointLight&)");
      throw;
    }
  }  
  
  void visit (scene_graph::SpotLight& entity)
  {
    try
    {
      result = new Light (scene, entity);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene_render3d::EntityFactory::visit(scene_graph::SpotLight&)");
      throw;
    }
  }  
  
  void visit (scene_graph::DirectLight& entity)
  {
    try
    {
      result = new Light (scene, entity);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene_render3d::EntityFactory::visit(scene_graph::DirectLight&)");
      throw;
    }
  }  
  
  void visit (scene_graph::VisualModel& entity)
  {
    try
    {
      result = new VisualModel (scene, entity);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene_render3d::EntityFactory::visit(scene_graph::VisualModel&)");
      throw;
    }
  }
  
  Node* CreateNode (scene_graph::Node& entity)
  {
    try
    {
      result = 0;
      
      entity.Accept (*this);
      
      return result;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene_render3d::EntityFactory::CreateNode");
      throw;
    }
  }
};

}

/*
    �������� ���������� ����� ����������
*/

typedef stl::hash_map<scene_graph::Node*, NodePtr> EntityMap;

struct Scene::Impl
{
  RenderPtr            render;                     //������
  scene_graph::Scene*  scene;                      //�������� �����
  xtl::auto_connection scene_destroyed_connection; //���������� � ������������ ������� �������� �����
  EntityMap            entities;                   //����� ��������
  EntityFactory        factory;                    //������� ��������
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

void Scene::RegisterEntity (scene_graph::Node& scene_node, Node& node)
{
  impl->entities.insert_pair (&scene_node, &node);
}

void Scene::UnregisterEntity (scene_graph::Node& scene_node)
{
  impl->entities.erase (&scene_node);
}

/*
    ��������� ������� �����
*/

Node* Scene::GetEntity (scene_graph::Node& entity)
{
  try
  {
    EntityMap::iterator iter = impl->entities.find (&entity);
    
    if (iter != impl->entities.end ())
      return &*iter->second;
      
    NodePtr node (impl->factory.CreateNode (entity), false); //����� ������������� �������� � ����� entities    
    
    if (!node)
    {
      //TODO: ������ �� ����������� ����� ��������!!!!!!!!!!!!!!!!!
      
      return 0;
    }
    
    return &*node;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::Scene::GetEntity");
    throw;
  }
}

/*
    ����� �����
*/

namespace
{

struct SceneCollector: public scene_graph::INodeTraverser
{
  public:
    SceneCollector (Scene& in_scene, CollectionVisitor& in_visitor)
      : scene (in_scene)
      , visitor (in_visitor)
    {
    }
  
    void operator () (scene_graph::Node& scene_node)
    {
      Node* node = scene.GetEntity (scene_node);
      
      if (!node)
        return;                

      node->Visit (visitor);
    }
    
  private:
    Scene&             scene;
    CollectionVisitor& visitor;
};

}

void Scene::Traverse (const bound_volumes::plane_listf& frustum, TraverseResults& results, size_t filter)
{
  results.Clear ();

  if (!impl->scene)
    return;

  CollectionVisitor visitor (results, filter);

  SceneCollector collector (*this, visitor);

  impl->scene->Traverse (frustum, collector);
}

void Scene::Traverse (const bound_volumes::plane_listf& frustum, TraverseResults& results)
{
  Traverse (frustum, results, Collect_All);
}
