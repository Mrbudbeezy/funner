#include "shared.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4355) // this used in base initializer list
#endif

using namespace render;
using namespace render::scene_render3d;

/*
    �������� ���������� ���������������� �������
*/

struct Renderable::Impl
{
  Scene&               scene;                        //�����, ������� ����������� ������
  scene_graph::Entity* entity;                       //�������� ������
  xtl::auto_connection on_entity_destroy_connection; //���������� �� �������� �������
  xtl::auto_connection on_entity_update_connection;  //���������� �� ���������� �������
  bool                 need_update;                  //������ ������� ����������

///�����������
  Impl (Scene& in_scene, scene_graph::Entity& in_entity)
    : scene (in_scene)
    , entity (&in_entity)
    , on_entity_destroy_connection (entity->RegisterEventHandler (scene_graph::NodeEvent_BeforeDestroy, xtl::bind (&Impl::OnDestroyEntity, this)))
    , on_entity_update_connection (entity->RegisterEventHandler (scene_graph::NodeEvent_AfterUpdate, xtl::bind (&Impl::OnUpdateEntity, this)))
    , need_update (true)
  {
  }

///���������� �� �������� �������
  void OnDestroyEntity ()
  {
    if (entity)
      scene.UnregisterRenderable (*entity);
      
    entity = 0;
  }

///���������� �� ���������� �������
  void OnUpdateEntity ()
  {
    need_update = true;
  }
};

/*
    ����������� / ����������
*/

Renderable::Renderable (Scene& scene, scene_graph::Entity& entity)
{
  try  
  {    
    impl = new Impl (scene, entity);
    
    scene.RegisterRenderable (entity, *this);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::Renderable::Renderable");
    throw;
  }
}

Renderable::~Renderable ()
{
  try
  {
    if (impl->entity)
      impl->scene.UnregisterRenderable (*impl->entity);
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

/*
    ���������
*/

void Renderable::Draw (Frame& frame)
{
  if (!impl->entity)
    return;

  if (impl->need_update)
  {
    UpdateCore (frame);

    impl->need_update = false;
  }

  DrawCore (frame);  
}
