#include "scene_graph.h"

using namespace scene_graph;

namespace engine
{

namespace scene_graph_script_binds
{

/*
    �������� ��������
*/

helpers::Box::Pointer create_box_helper ()
{
  return helpers::Box::Create ();
}

/*
   ����������� ���������� ������ � ���������
*/

void set_bound_box (helpers::Box& entity, const bound_volumes::aaboxf& box)
{
  entity.SetBoundBox (box);
}

void bind_box_helper_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_BOX_HELPER_LIBRARY);

    //������������

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&create_box_helper));

    //����������� ��������

  lib.Register ("set_BoundBox", make_invoker (&set_bound_box));

    //����������� ����� ������

  environment.RegisterType<helpers::Box> (SCENE_BOX_HELPER_LIBRARY);
}

}

}
