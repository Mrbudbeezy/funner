#include "shared.h"

#include <xtl/implicit_cast.h>

#include <sg/camera.h>
#include <sg/light.h>
#include <sg/helper.h>
#include <sg/listener.h>
#include <sg/sound_emitter.h>
#include <sg/visual_model.h>

using namespace script;
using namespace scene_graph;
using namespace math;
using namespace xtl;

namespace
{

/*
    ��������� (����� ���������)
*/

const char* SCENE_STATIC_NODE_BIND_MODE_LIBRARY       = "static.Scene_NodeBindMode";
const char* SCENE_STATIC_NODE_TRANSFORM_SPACE_LIBRARY = "static.Scene_NodeTransformSpace";
const char* SCENE_STATIC_NODE_TRAVERSE_MODE_LIBRARY   = "static.Scene_NodeTraverseMode";
const char* SCENE_NODE_LIBRARY                        = "Scene.Node";
const char* SCENE_ENTITY_LIBRARY                      = "Scene.Entity";
const char* SCENE_PERSPECTIVE_CAMERA_LIBRARY          = "Scene.PerspectiveCamera";
const char* SCENE_ORTHO_CAMERA_LIBRARY                = "Scene.OrthoCamera";
const char* SCENE_LIGHT_LIBRARY                       = "Scene.Light";
const char* SCENE_DIRECT_LIGHT_LIBRARY                = "Scene.DirectLight";
const char* SCENE_SPOT_LIGHT_LIBRARY                  = "Scene.SpotLight";
const char* SCENE_POINT_LIGHT_LIBRARY                 = "Scene.PointLight";
const char* SCENE_BOX_HELPER_LIBRARY                  = "Scene.BoxHelper";
const char* SCENE_LISTENER_LIBRARY                    = "Scene.Listener";
const char* SCENE_SOUND_EMITTER_LIBRARY               = "Scene.SoundEmitter";
const char* SCENE_VISUAL_MODEL_LIBRARY                = "Scene.VisualModel";

/*
    �������� ����� �����
*/

Node::Pointer create_node ()
{
  return Node::Create ();
}

/*
   ����������� ���������� ������ � ������ �����
*/

void bind_static_node_library (Environment& environment)
{
  InvokerRegistry& node_bind_mode_lib       = environment.CreateLibrary (SCENE_STATIC_NODE_BIND_MODE_LIBRARY);
  InvokerRegistry& node_transform_space_lib = environment.CreateLibrary (SCENE_STATIC_NODE_TRANSFORM_SPACE_LIBRARY);
  InvokerRegistry& node_traverse_mode_lib   = environment.CreateLibrary (SCENE_STATIC_NODE_TRAVERSE_MODE_LIBRARY);

  node_bind_mode_lib.Register       ("get_AddRef",      make_const (NodeBindMode_AddRef));
  node_bind_mode_lib.Register       ("get_Capture",     make_const (NodeBindMode_Capture));
  node_transform_space_lib.Register ("get_Local",       make_const (NodeTransformSpace_Local));
  node_transform_space_lib.Register ("get_Parent",      make_const (NodeTransformSpace_Parent));
  node_transform_space_lib.Register ("get_World",       make_const (NodeTransformSpace_World));
  node_traverse_mode_lib.Register   ("get_TopToBottom", make_const (NodeTraverseMode_TopToBottom));
  node_traverse_mode_lib.Register   ("get_BottomToTop", make_const (NodeTraverseMode_BottomToTop));
}
  
InvokerRegistry& bind_node_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_NODE_LIBRARY);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&create_node));

    //����������� ����������� ����������

  bind_static_node_library (environment);

    //����������� ��������

  lib.Register ("set_Name",               make_invoker (&Node::SetName));  
  lib.Register ("set_Position",           make_invoker (implicit_cast<void (Node::*) (const vec3f&)> (&Node::SetPosition)));
  lib.Register ("SetPosition",            make_invoker (implicit_cast<void (Node::*) (float, float, float)> (&Node::SetPosition)));
  lib.Register ("ResetPosition",          make_invoker (&Node::ResetPosition));
  lib.Register ("set_Orientation",        make_invoker (implicit_cast<void (Node::*) (const quatf&)> (&Node::SetOrientation)));
  lib.Register ("SetOrientation",         make_invoker (implicit_cast<void (Node::*) (float, float, float, float)> (&Node::SetOrientation)));
  lib.Register ("SetEulerOrientation",    make_invoker (implicit_cast<void (Node::*) (float, float, float)> (&Node::SetOrientation)));
  lib.Register ("ResetOrientation",       make_invoker (&Node::ResetOrientation));
  lib.Register ("set_Scale",              make_invoker (implicit_cast<void (Node::*) (const vec3f&)> (&Node::SetScale)));
  lib.Register ("SetScale",               make_invoker (implicit_cast<void (Node::*) (float, float, float)> (&Node::SetScale)));
  lib.Register ("ResetScale",             make_invoker (&Node::ResetScale));
  lib.Register ("set_OrientationInherit", make_invoker (&Node::SetOrientationInherit));
  lib.Register ("set_ScaleInherit",       make_invoker (&Node::SetScaleInherit));

  lib.Register ("get_Name",               make_invoker (&Node::Name));
  lib.Register ("get_Position",           make_invoker (&Node::Position));
  lib.Register ("get_WorldPosition",      make_invoker (&Node::WorldPosition));
  lib.Register ("get_Orientation",        make_invoker (&Node::Orientation));
  lib.Register ("get_WorldOrientation",   make_invoker (&Node::WorldOrientation));
  lib.Register ("get_Scale",              make_invoker (implicit_cast<const vec3f& (Node::*) () const> (&Node::Scale)));
  lib.Register ("get_WorldScale",         make_invoker (&Node::WorldScale));
  lib.Register ("get_OrientationInherit", make_invoker (&Node::IsOrientationInherited));
  lib.Register ("get_ScaleInherit",       make_invoker (&Node::IsScaleInherited));
  lib.Register ("get_LocalTM",            make_invoker (&Node::LocalTM));
  lib.Register ("get_WorldTM",            make_invoker (&Node::WorldTM));
  lib.Register ("get_ParentTM",           make_invoker (&Node::ParentTM));

  lib.Register ("Parent",     make_invoker (implicit_cast<Node::Pointer (Node::*) ()> (&Node::Parent)));
  lib.Register ("FirstChild", make_invoker (implicit_cast<Node::Pointer (Node::*) ()> (&Node::FirstChild)));
  lib.Register ("LastChild",  make_invoker (implicit_cast<Node::Pointer (Node::*) ()> (&Node::LastChild)));
  lib.Register ("PrevChild",  make_invoker (implicit_cast<Node::Pointer (Node::*) ()> (&Node::PrevChild)));
  lib.Register ("NextChild",  make_invoker (implicit_cast<Node::Pointer (Node::*) ()> (&Node::NextChild)));

  lib.Register ("BindToParent",      make_invoker (&Node::BindToParent));
  lib.Register ("Unbind",            make_invoker (&Node::Unbind));
//  lib.Register ("UnbindChild",       make_invoker (&Node::UnbindChild)));
  lib.Register ("UnbindAllChildren", make_invoker (&Node::UnbindAllChildren));
  
  lib.Register ("ObjectTM", make_invoker (&Node::ObjectTM));

  lib.Register ("Rescale", make_invoker (implicit_cast<void (Node::*) (const vec3f&)> (&Node::Scale)));

    //����������� ����� ������

  environment.RegisterType<Node> (SCENE_NODE_LIBRARY);

  return lib;
}

/*
   ����������� ���������� ������ � ��������� �����
*/

InvokerRegistry& bind_entity_library (Environment& environment, InvokerRegistry& node_lib)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_ENTITY_LIBRARY);

    //������������

  lib.Register (node_lib);

    //����������� ��������

  lib.Register ("set_Color",  make_invoker (implicit_cast<void (Entity::*) (const vec3f&)> (&Entity::SetColor)));
  lib.Register ("get_Color",  make_invoker (&Entity::Color));

    //����������� ����� ������

  environment.RegisterType<Entity> (SCENE_ENTITY_LIBRARY);

  return lib;
}

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

void bind_perspective_camera_library (Environment& environment, InvokerRegistry& entity_lib)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_PERSPECTIVE_CAMERA_LIBRARY);

    //������������

  lib.Register (entity_lib);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&create_perspective_camera));

    //����������� ��������

  lib.Register ("set_FovX",  make_invoker (&PerspectiveCamera::SetFovX));
  lib.Register ("set_FovY",  make_invoker (&PerspectiveCamera::SetFovY));
  lib.Register ("set_ZNear", make_invoker (&PerspectiveCamera::SetZNear));
  lib.Register ("set_ZFar",  make_invoker (&PerspectiveCamera::SetZFar));
  lib.Register ("get_FovX",  make_invoker (&PerspectiveCamera::FovX));
  lib.Register ("get_FovY",  make_invoker (&PerspectiveCamera::FovY));
  lib.Register ("get_ZNear", make_invoker (&PerspectiveCamera::ZNear));
  lib.Register ("get_ZFar",  make_invoker (&PerspectiveCamera::ZFar));

    //����������� ����� ������

  environment.RegisterType<PerspectiveCamera> (SCENE_PERSPECTIVE_CAMERA_LIBRARY);
}

void bind_ortho_camera_library (Environment& environment, InvokerRegistry& entity_lib)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_ORTHO_CAMERA_LIBRARY);

    //������������

  lib.Register (entity_lib);

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

void bind_direct_light_library (Environment& environment, InvokerRegistry& light_lib)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_DIRECT_LIGHT_LIBRARY);

    //������������

  lib.Register (light_lib);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&create_direct_light));

    //����������� ��������

  lib.Register ("set_Radius", make_invoker (&DirectLight::SetRadius));
  lib.Register ("get_Radius", make_invoker (&DirectLight::Radius));

    //����������� ����� ������

  environment.RegisterType<DirectLight> (SCENE_DIRECT_LIGHT_LIBRARY);
}

void bind_spot_light_library (Environment& environment, InvokerRegistry& light_lib)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_SPOT_LIGHT_LIBRARY);

    //������������

  lib.Register (light_lib);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&create_spot_light));

    //����������� ��������

  lib.Register ("set_Angle",    make_invoker (&SpotLight::SetAngle));
  lib.Register ("set_Exponent", make_invoker (&SpotLight::SetExponent));
  lib.Register ("get_Angle",    make_invoker (&SpotLight::Angle));
  lib.Register ("get_Exponent", make_invoker (&SpotLight::Exponent));

    //����������� ����� ������

  environment.RegisterType<SpotLight> (SCENE_SPOT_LIGHT_LIBRARY);
}

void bind_point_light_library (Environment& environment, InvokerRegistry& light_lib)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_POINT_LIGHT_LIBRARY);

    //������������

  lib.Register (light_lib);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&create_point_light));

    //����������� ����� ������

  environment.RegisterType<PointLight> (SCENE_POINT_LIGHT_LIBRARY);
}

void bind_light_library (Environment& environment, InvokerRegistry& entity_lib)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_LIGHT_LIBRARY);

    //������������

  lib.Register (entity_lib);

    //����������� ��������

  lib.Register ("set_LightColor",  make_invoker (&Light::SetLightColor));
  lib.Register ("set_Attenuation", make_invoker (&Light::SetAttenuation));
  lib.Register ("set_Range",       make_invoker (&Light::SetRange));
  lib.Register ("get_LightColor",  make_invoker (&Light::LightColor));
  lib.Register ("get_Attenuation", make_invoker (&Light::Attenuation));
  lib.Register ("get_Range",       make_invoker (&Light::Range));

    //����������� ���������

  bind_direct_light_library (environment, lib);
  bind_spot_light_library   (environment, lib);
  bind_point_light_library  (environment, lib);

    //����������� ����� ������

  environment.RegisterType<Light> (SCENE_LIGHT_LIBRARY);
}

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

void bind_box_helper_library (Environment& environment, InvokerRegistry& entity_lib)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_BOX_HELPER_LIBRARY);

    //������������

  lib.Register (entity_lib);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&create_box_helper));

    //����������� ����� ������

  environment.RegisterType<helpers::Box> (SCENE_BOX_HELPER_LIBRARY);
}

/*
    �������� ���������
*/

Listener::Pointer create_listener ()
{
  return Listener::Create ();
}

/*
   ����������� ���������� ������ � �����������
*/

void bind_listener_library (Environment& environment, InvokerRegistry& entity_lib)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_LISTENER_LIBRARY);

    //������������

  lib.Register (entity_lib);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&create_listener));

    //����������� ��������

  lib.Register ("set_Gain", make_invoker (&Listener::SetGain));
  lib.Register ("get_Gain", make_invoker (&Listener::Gain));

    //����������� ����� ������

  environment.RegisterType<Listener> (SCENE_LISTENER_LIBRARY);
}

/*
    �������� ��������� �����
*/

SoundEmitter::Pointer create_sound_emitter (const char* sound_declaration_name)
{
  return SoundEmitter::Create (sound_declaration_name);
}

/*
   ����������� ���������� ������ � ����������� �����
*/

void bind_sound_emitter_library (Environment& environment, InvokerRegistry& entity_lib)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_SOUND_EMITTER_LIBRARY);

    //������������

  lib.Register (entity_lib);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&create_sound_emitter));

    //����������� ��������

  lib.Register ("Play", make_invoker (&SoundEmitter::Play));
  lib.Register ("Stop", make_invoker (&SoundEmitter::Stop));

    //����������� ����� ������

  environment.RegisterType<SoundEmitter> (SCENE_SOUND_EMITTER_LIBRARY);
}

/*
    �������� ������
*/

VisualModel::Pointer create_visual_model ()
{
  return VisualModel::Create ();
}

/*
   ����������� ���������� ������ � ��������
*/

void bind_visual_model_library (Environment& environment, InvokerRegistry& entity_lib)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_VISUAL_MODEL_LIBRARY);

    //������������

  lib.Register (entity_lib);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&create_visual_model));

    //����������� ��������

  lib.Register ("set_MeshName", make_invoker (&VisualModel::SetMeshName));
  lib.Register ("get_MeshName", make_invoker (&VisualModel::MeshName));

    //����������� ����� ������

  environment.RegisterType<VisualModel> (SCENE_VISUAL_MODEL_LIBRARY);
}

}

namespace script
{

/*
    ����������� ���������� ������ �� ������
*/

void bind_scene_graph_library (Environment& environment)
{
    //����������� ���������
  
  InvokerRegistry& node_class_lib   = bind_node_library (environment);
  InvokerRegistry& entity_class_lib = bind_entity_library (environment, node_class_lib);

  bind_perspective_camera_library (environment, entity_class_lib);
  bind_ortho_camera_library       (environment, entity_class_lib);
  bind_light_library              (environment, entity_class_lib);
  bind_box_helper_library         (environment, entity_class_lib);
  bind_listener_library           (environment, entity_class_lib);
  bind_sound_emitter_library      (environment, entity_class_lib);
  bind_visual_model_library       (environment, entity_class_lib);
}

}
