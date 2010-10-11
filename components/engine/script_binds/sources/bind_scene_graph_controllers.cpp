#include "scene_graph.h"

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

}

}
