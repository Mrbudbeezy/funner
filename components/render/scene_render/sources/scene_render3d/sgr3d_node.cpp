#include "shared.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4355) // this used in base initializer list
#endif

using namespace render;
using namespace render::scene_render3d;

/*
    �������� ���������� ���������������� �������
*/

struct Node::Impl
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
      scene.UnregisterEntity (*entity);
      
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

Node::Node (Scene& scene, scene_graph::Entity& entity)
{
  try  
  {    
    impl = new Impl (scene, entity);
    
    scene.RegisterEntity (entity, *this);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::Node::Node");
    throw;
  }
}

Node::~Node ()
{
  try
  {
    if (impl->entity)
      impl->scene.UnregisterEntity (*impl->entity);
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

/*
    �����
*/

void Node::Visit (IVisitor& visitor)
{
  VisitCore (visitor);
}

void Node::VisitCore (IVisitor& visitor)
{
  visitor.Visit (*this);
}

/*
    ���������� ���������
*/

void Node::Update ()
{
  if (!impl->entity)
    return;

  if (impl->need_update)
  {
    UpdateCore ();

    impl->need_update = false;
  }
}

void Node::UpdateCore ()
{
}
