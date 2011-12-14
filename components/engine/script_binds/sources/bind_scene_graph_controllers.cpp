#include "scene_graph.h"

using namespace scene_graph;

namespace
{

const char* SCENE_CONTROLLER_ALIGN_WITH_NODE_LIBRARY      = "Scene.Controllers.AlignWithNode";
const char* SCENE_CONTROLLER_LOOK_TO_NODE_POINT_LIBRARY   = "Scene.Controllers.LookToNodePoint";
const char* SCENE_CONTROLLER_MOVE_TO_NODE_POINT_LIBRARY   = "Scene.Controllers.MoveToNodePoint";
const char* SCENE_CONTROLLER_SYNC_PHYSICS_TO_NODE_LIBRARY = "Scene.Controllers.SyncPhysicsToNode";
const char* SCENE_CONTROLLER_WATER_LIBRARY                = "Scene.Controllers.Water";

}

namespace engine
{

namespace scene_graph_script_binds
{

/*
    ����������� ���������� ������ � ������������ ����
*/

void bind_controller_water_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_CONTROLLER_WATER_LIBRARY);

    //������������

  lib.Register (environment, SCENE_CONTROLLER_LIBRARY);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&Water::Create));

    //����������� ��������
    
  lib.Register ("set_Viscosity", make_invoker (&Water::SetViscosity));
  lib.Register ("get_Viscosity", make_invoker (&Water::Viscosity));
  lib.Register ("PutStorm",      make_invoker (
                                   make_invoker (&Water::PutStorm), 
                                   make_invoker<void (Water&, const math::vec3f&, float)> (xtl::bind (&Water::PutStorm, _1, _2, _3, 0.05f)),
                                   make_invoker<void (Water&, const math::vec3f&)> (xtl::bind (&Water::PutStorm, _1, _2, 0.0005f, 0.05f))
  ));
  lib.Register ("PutWorldStorm",   make_invoker (
                                   make_invoker (&Water::PutWorldStorm), 
                                   make_invoker<void (Water&, const math::vec3f&, float)> (xtl::bind (&Water::PutWorldStorm, _1, _2, _3, 0.05f)),
                                   make_invoker<void (Water&, const math::vec3f&)> (xtl::bind (&Water::PutWorldStorm, _1, _2, 0.0005f, 0.05f))
  ));

    //����������� ���� ������

  environment.RegisterType<Water> (SCENE_CONTROLLER_WATER_LIBRARY);
}

/*
    ����������� ���������� ������ � ������������ ������������
*/

void bind_controller_move_to_node_point_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_CONTROLLER_MOVE_TO_NODE_POINT_LIBRARY);

    //������������

  lib.Register (environment, SCENE_CONTROLLER_LIBRARY);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&MoveToNodePoint::Create));

    //����������� ��������

  lib.Register ("set_AccelerationHandler", make_invoker<void (MoveToNodePoint::*) (const LinearAccelerationEvaluator&)> (&MoveToNodePoint::SetAccelerationHandler));
  lib.Register ("set_TransformSpace",      make_invoker (&MoveToNodePoint::SetTransformSpace));
  lib.Register ("get_TransformSpace",      make_invoker (&MoveToNodePoint::TransformSpace));
  lib.Register ("Start",                   make_invoker<void (MoveToNodePoint::*) (Node::Pointer node, const math::vec3f&)> (&MoveToNodePoint::Start));
  lib.Register ("Stop",                    make_invoker (&MoveToNodePoint::Stop));

    //����������� ���� ������

  environment.RegisterType<MoveToNodePoint> (SCENE_CONTROLLER_MOVE_TO_NODE_POINT_LIBRARY);
}

/*
    ����������� ���������� ������ � ������������ ��������
*/

void bind_controller_look_to_node_point_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_CONTROLLER_LOOK_TO_NODE_POINT_LIBRARY);

    //������������

  lib.Register (environment, SCENE_CONTROLLER_LIBRARY);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&LookToNodePoint::Create));

    //����������� ��������

  lib.Register ("set_AccelerationHandler", make_invoker<void (LookToNodePoint::*) (const LinearAccelerationEvaluator&)> (&LookToNodePoint::SetAccelerationHandler));
  lib.Register ("Start",                   make_invoker<void (LookToNodePoint::*) (Node::Pointer node, const math::vec3f&, NodeOrt, NodeOrt)> (&LookToNodePoint::Start));
  lib.Register ("Stop",                    make_invoker (&LookToNodePoint::Stop));

    //����������� ���� ������

  environment.RegisterType<LookToNodePoint> (SCENE_CONTROLLER_LOOK_TO_NODE_POINT_LIBRARY);
}

/*
    ����������� ���������� ������ � ������������ ������������
*/

void bind_controller_align_with_node_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_CONTROLLER_ALIGN_WITH_NODE_LIBRARY);

    //������������

  lib.Register (environment, SCENE_CONTROLLER_LIBRARY);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&AlignWithNode::Create));

    //����������� ��������

  lib.Register ("set_AccelerationHandler", make_invoker<void (AlignWithNode::*) (const LinearAccelerationEvaluator&)> (&AlignWithNode::SetAccelerationHandler));
  lib.Register ("Start",                   make_invoker<void (AlignWithNode::*) (Node::Pointer node, NodeOrt, NodeOrt, NodeOrt)> (&AlignWithNode::Start));
  lib.Register ("Stop",                    make_invoker (&AlignWithNode::Stop));

    //����������� ���� ������

  environment.RegisterType<AlignWithNode> (SCENE_CONTROLLER_ALIGN_WITH_NODE_LIBRARY);
}

void bind_controller_sync_physics_to_node_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_CONTROLLER_SYNC_PHYSICS_TO_NODE_LIBRARY);

    //������������

  lib.Register (environment, SCENE_CONTROLLER_LIBRARY);

  lib.Register ("Create",           make_invoker (&SyncPhysicsToNode::Create));
  lib.Register ("get_AttachedBody", make_invoker (&SyncPhysicsToNode::AttachedBody));

  environment.RegisterType<AlignWithNode> (SCENE_CONTROLLER_SYNC_PHYSICS_TO_NODE_LIBRARY);
}

}

}
