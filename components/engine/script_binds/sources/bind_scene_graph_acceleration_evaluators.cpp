#include "scene_graph.h"

namespace
{

const char* SCENE_LINEAR_ACCELERATION_EVALUATOR_LIBRARY = "Scene.AccelerationEvaluators.Linear";

}

namespace engine
{

namespace scene_graph_script_binds
{

//�������� �������
LinearAccelerationEvaluator create_linear_acceleration_evaluator ()
{
  return LinearAccelerationEvaluator ();
}

/*
    ����������� ���������� ������ � �������� ��������� ���������
*/

void bind_linear_acceleration_evaluator_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_LINEAR_ACCELERATION_EVALUATOR_LIBRARY);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&create_linear_acceleration_evaluator));

    //����������� ��������

  lib.Register ("set_Acceleration", make_invoker (&LinearAccelerationEvaluator::SetAcceleration));
  lib.Register ("set_Deceleration", make_invoker (&LinearAccelerationEvaluator::SetDeceleration));
  lib.Register ("set_MaxSpeed",     make_invoker (&LinearAccelerationEvaluator::SetMaxSpeed));
  lib.Register ("get_Acceleration", make_invoker (&LinearAccelerationEvaluator::Acceleration));
  lib.Register ("get_Deceleration", make_invoker (&LinearAccelerationEvaluator::Deceleration));
  lib.Register ("get_MaxSpeed",     make_invoker (&LinearAccelerationEvaluator::MaxSpeed));

    //����������� ���� ������

  environment.RegisterType<LinearAccelerationEvaluator> (SCENE_LINEAR_ACCELERATION_EVALUATOR_LIBRARY);
}

}

}