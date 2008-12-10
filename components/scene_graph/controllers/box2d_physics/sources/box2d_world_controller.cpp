#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::physics;
using namespace scene_graph::physics::box2d;

/*
    �������� ���������� ����������� ����������� ����
*/

struct WorldController::Impl
{
  b2World                world;            //���������� ���
  Node&                  root_node;        //�������� ����, �������������� ����������� ����
  Configuration::Pointer configuration;    //������������
  size_t                 iterations_count; //���������� �������� ��������� ����������� ����
  xtl::auto_connection   on_attach_child;  //���������� �� ������� ��������� �������
  xtl::auto_connection   on_detach_child;  //���������� �� ������� �������� �������
  
///�����������
  Impl (Node& root, const b2AABB& bound_box, const Configuration::Pointer& in_configuration)
    : world (bound_box, in_configuration->GetGravity (), in_configuration->IsDoSleep ()),
      root_node (root),
      configuration (in_configuration),
      iterations_count (in_configuration->GetIterationsCount ())
  {
    on_attach_child = root.RegisterEventHandler (NodeSubTreeEvent_AfterBind,    xtl::bind (&Impl::OnAttachChild, this, _2));
    on_detach_child = root.RegisterEventHandler (NodeSubTreeEvent_BeforeUnbind, xtl::bind (&Impl::OnDetachChild, this, _2));
  }  
  
///���������� � ��������� ������ ����
  void OnAttachChild (Node& node)
  {
    if (node.Parent () != &root_node)
      return;
      
    Body* body = dynamic_cast<Body*> (&node);
    
    if (!body)
      return;
      
    printf ("create body '%s'\n", body->Name ());    
  }
    
///���������� �� �������� ����
  void OnDetachChild (Node& node)
  {
    if (node.Parent () != &root_node)
      return;
  }
};

/*
    ����������� / ����������
*/

WorldController::WorldController (Node& root, const bound_volumes::aaboxf& bound_box, const Configuration::Pointer& configuration)
{
  try
  {
      //�������������� ����������
      
    if (!configuration)
      throw xtl::make_null_argument_exception ("", "configuration");

    const math::vec3f &vmin = bound_box.minimum (), &vmax = bound_box.maximum ();

    b2AABB box = {b2Vec2 (vmin.x, vmin.y), b2Vec2 (vmax.x, vmax.y)};

    impl = new Impl (root, box, configuration);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("scene_graph::physics::box2d::WorldController::WorldController");
    throw;
  }
}

WorldController::~WorldController ()
{
}

/*
    ���������� ����
*/

void WorldController::Update (float dt)
{
  printf ("WorldController::Update(%.2f)\n", dt);
  impl->world.Step (dt, impl->iterations_count);
}
