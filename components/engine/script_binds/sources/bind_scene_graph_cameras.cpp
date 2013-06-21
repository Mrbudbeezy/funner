#include "scene_graph.h"

using namespace scene_graph;

namespace engine
{

namespace scene_graph_script_binds
{

template class engine::decl_sg_cast<Camera,            Node>;
template class engine::decl_sg_cast<Camera,            Entity>;
template class engine::decl_sg_cast<OrthoCamera,       Node>;
template class engine::decl_sg_cast<OrthoCamera,       Entity>;
template class engine::decl_sg_cast<OrthoCamera,       Camera>;
template class engine::decl_sg_cast<PerspectiveCamera, Node>;
template class engine::decl_sg_cast<PerspectiveCamera, Entity>;
template class engine::decl_sg_cast<PerspectiveCamera, Camera>;

/*
    �������� �����
*/

PerspectiveCamera::Pointer create_perspective_camera ()
{
  return PerspectiveCamera::Create ();
}

OrthoCamera::Pointer create_ortho_camera ()
{
  return OrthoCamera::Create ();
}

/*
    ����������� ��������� ������ � ��������
*/

void bind_perspective_camera_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_PERSPECTIVE_CAMERA_LIBRARY);

    //������������

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&create_perspective_camera));

    //����������� ��������

  lib.Register ("set_FovX",  make_invoker<void (PerspectiveCamera&, math::anglef)> (&PerspectiveCamera::SetFovX));
  lib.Register ("set_FovY",  make_invoker<void (PerspectiveCamera&, math::anglef)> (&PerspectiveCamera::SetFovY));
  lib.Register ("set_ZNear", make_invoker (&PerspectiveCamera::SetZNear));
  lib.Register ("set_ZFar",  make_invoker (&PerspectiveCamera::SetZFar));
  lib.Register ("get_FovX",  make_invoker (&PerspectiveCamera::FovX));
  lib.Register ("get_FovY",  make_invoker (&PerspectiveCamera::FovY));
  lib.Register ("get_ZNear", make_invoker (&PerspectiveCamera::ZNear));
  lib.Register ("get_ZFar",  make_invoker (&PerspectiveCamera::ZFar));

    //����������� ����� ������

  environment.RegisterType<PerspectiveCamera> (SCENE_PERSPECTIVE_CAMERA_LIBRARY);
}

void bind_ortho_camera_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_ORTHO_CAMERA_LIBRARY);

    //������������

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&create_ortho_camera));

    //����������� ��������

  lib.Register ("set_Left",   make_invoker (&OrthoCamera::SetLeft));
  lib.Register ("set_Right",  make_invoker (&OrthoCamera::SetRight));
  lib.Register ("set_Top",    make_invoker (&OrthoCamera::SetTop));
  lib.Register ("set_Bottom", make_invoker (&OrthoCamera::SetBottom));
  lib.Register ("set_ZNear",  make_invoker (&OrthoCamera::SetZNear));
  lib.Register ("set_ZFar",   make_invoker (&OrthoCamera::SetZFar));
  lib.Register ("get_Left",   make_invoker (&OrthoCamera::Left));
  lib.Register ("get_Right",  make_invoker (&OrthoCamera::Right));
  lib.Register ("get_Top",    make_invoker (&OrthoCamera::Top));
  lib.Register ("get_Bottom", make_invoker (&OrthoCamera::Bottom));
  lib.Register ("get_ZNear",  make_invoker (&OrthoCamera::ZNear));
  lib.Register ("get_ZFar",   make_invoker (&OrthoCamera::ZFar));

    //����������� ����� ������

  environment.RegisterType<OrthoCamera> (SCENE_ORTHO_CAMERA_LIBRARY);
}

}

}
