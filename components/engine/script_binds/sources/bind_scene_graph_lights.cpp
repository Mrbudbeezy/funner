#include "scene_graph.h"

namespace engine
{

namespace scene_graph_script_binds
{

/*
    �������� ���������� �����
*/

DirectLight::Pointer create_direct_light ()
{
  return DirectLight::Create ();
}

SpotLight::Pointer create_spot_light ()
{
  return SpotLight::Create ();
}

PointLight::Pointer create_point_light ()
{
  return PointLight::Create ();
}

/*
    ����������� ��������� ������ � ����������� �����
*/

void bind_direct_light_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_DIRECT_LIGHT_LIBRARY);

    //������������

  lib.Register (environment, SCENE_LIGHT_LIBRARY);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&create_direct_light));

    //����������� ��������

  lib.Register ("set_Radius", make_invoker (&DirectLight::SetRadius));
  lib.Register ("get_Radius", make_invoker (&DirectLight::Radius));

    //����������� ����� ������

  environment.RegisterType<DirectLight> (SCENE_DIRECT_LIGHT_LIBRARY);
}

void bind_spot_light_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_SPOT_LIGHT_LIBRARY);

    //������������

  lib.Register (environment, SCENE_LIGHT_LIBRARY);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&create_spot_light));

    //����������� ��������

  lib.Register ("set_Angle",    make_invoker<void (SpotLight&, math::anglef)> (&SpotLight::SetAngle));
  lib.Register ("set_Exponent", make_invoker (&SpotLight::SetExponent));
  lib.Register ("get_Angle",    make_invoker<math::anglef (SpotLight&)> (&SpotLight::Angle));
  lib.Register ("get_Exponent", make_invoker (&SpotLight::Exponent));

    //����������� ����� ������

  environment.RegisterType<SpotLight> (SCENE_SPOT_LIGHT_LIBRARY);
}

void bind_point_light_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_POINT_LIGHT_LIBRARY);

    //������������

  lib.Register (environment, SCENE_LIGHT_LIBRARY);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&create_point_light));

    //����������� ����� ������

  environment.RegisterType<PointLight> (SCENE_POINT_LIGHT_LIBRARY);
}

void bind_light_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_LIGHT_LIBRARY);

    //������������

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

    //����������� ��������

  lib.Register ("set_LightColor",  make_invoker (&Light::SetLightColor));
  lib.Register ("set_Attenuation", make_invoker (&Light::SetAttenuation));
  lib.Register ("set_Range",       make_invoker (&Light::SetRange));
  lib.Register ("get_LightColor",  make_invoker (&Light::LightColor));
  lib.Register ("get_Attenuation", make_invoker (&Light::Attenuation));
  lib.Register ("get_Range",       make_invoker (&Light::Range));

    //����������� ����� ������

  environment.RegisterType<Light> (SCENE_LIGHT_LIBRARY);
}

}

}
