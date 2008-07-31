#include "shared.h"

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
const char* SCENE_STATIC_NODE_SEARCH_MODE_LIBRARY     = "static.Scene_NodeSearchMode";
const char* SCENE_SCENE_LIBRARY                       = "Scene.Scene";
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
const char* SCENE_SPRITE_LIBRARY                      = "Scene.Sprite";
const char* SCENE_VISUAL_MODEL_LIBRARY                = "Scene.VisualModel";

/*
    �������� �����
*/

shared_ptr<Scene> create_scene ()
{
  return shared_ptr<Scene> (new Scene ());
}

Node::Pointer get_scene_root (Scene& scene)
{
  return &scene.Root ();
}

/*
   ����������� ���������� ������ �� ������
*/

void bind_scene_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_SCENE_LIBRARY);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&create_scene));

    //����������� ��������

  lib.Register ("set_Name", make_invoker (&Scene::SetName));
  lib.Register ("get_Name", make_invoker (&Scene::Name));

  lib.Register ("get_Root", make_invoker (&get_scene_root));

  lib.Register ("get_EntitiesCount", make_invoker (&Scene::EntitiesCount));

  environment.RegisterType<Scene> (SCENE_SCENE_LIBRARY);
}

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
  InvokerRegistry& node_search_mode_lib     = environment.CreateLibrary (SCENE_STATIC_NODE_SEARCH_MODE_LIBRARY);

  node_bind_mode_lib.Register       ("get_AddRef",         make_const (NodeBindMode_AddRef));
  node_bind_mode_lib.Register       ("get_Capture",        make_const (NodeBindMode_Capture));
  node_bind_mode_lib.Register       ("get_Default",        make_const (NodeBindMode_Default));
  node_transform_space_lib.Register ("get_Local",          make_const (NodeTransformSpace_Local));
  node_transform_space_lib.Register ("get_Parent",         make_const (NodeTransformSpace_Parent));
  node_transform_space_lib.Register ("get_World",          make_const (NodeTransformSpace_World));
  node_traverse_mode_lib.Register   ("get_TopToBottom",    make_const (NodeTraverseMode_TopToBottom));
  node_traverse_mode_lib.Register   ("get_BottomToTop",    make_const (NodeTraverseMode_BottomToTop));
  node_traverse_mode_lib.Register   ("get_Default",        make_const (NodeTraverseMode_Default));
  node_search_mode_lib.Register     ("get_OnNextSublevel", make_const (NodeSearchMode_OnNextSublevel));
  node_search_mode_lib.Register     ("get_OnAllSublevels", make_const (NodeSearchMode_OnAllSublevels));
  node_search_mode_lib.Register     ("get_Default",        make_const (NodeSearchMode_Default));
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

  lib.Register ("get_Name",                  make_invoker (&Node::Name));
  lib.Register ("get_Position",              make_invoker (&Node::Position));
  lib.Register ("get_WorldPosition",         make_invoker (&Node::WorldPosition));
  lib.Register ("get_Orientation",           make_invoker (&Node::Orientation));
  lib.Register ("get_WorldOrientation",      make_invoker (&Node::WorldOrientation));
  lib.Register ("get_Scale",                 make_invoker (implicit_cast<const vec3f& (Node::*) () const> (&Node::Scale)));
  lib.Register ("get_WorldScale",            make_invoker (&Node::WorldScale));
  lib.Register ("get_OrientationInherit",    make_invoker (&Node::IsOrientationInherited));
  lib.Register ("get_ScaleInherit",          make_invoker (&Node::IsScaleInherited));
  lib.Register ("get_LocalTM",               make_invoker (&Node::LocalTM));
  lib.Register ("get_WorldTM",               make_invoker (&Node::WorldTM));
  lib.Register ("get_ParentTM",              make_invoker (&Node::ParentTM));
  lib.Register ("get_IsInUpdateTransaction", make_invoker (&Node::IsInUpdateTransaction));

  lib.Register ("Parent",     make_invoker (implicit_cast<Node::Pointer (Node::*) ()> (&Node::Parent)));
  lib.Register ("FirstChild", make_invoker (implicit_cast<Node::Pointer (Node::*) ()> (&Node::FirstChild)));
  lib.Register ("LastChild",  make_invoker (implicit_cast<Node::Pointer (Node::*) ()> (&Node::LastChild)));
  lib.Register ("PrevChild",  make_invoker (implicit_cast<Node::Pointer (Node::*) ()> (&Node::PrevChild)));
  lib.Register ("NextChild",  make_invoker (implicit_cast<Node::Pointer (Node::*) ()> (&Node::NextChild)));

  lib.Register ("BindToParent", make_invoker (make_invoker (&Node::BindToParent),
    make_invoker<void (Node&, Node&, NodeBindMode)> (xtl::bind (&Node::BindToParent, _1, _2, _3, NodeTransformSpace_Local)),
    make_invoker<void (Node&, Node&)> (xtl::bind (&Node::BindToParent, _1, _2, NodeBindMode_Default, NodeTransformSpace_Local))));

  lib.Register ("BindToScene", make_invoker (make_invoker (&Node::BindToScene),
    make_invoker<void (Node&, Scene&, NodeBindMode)> (xtl::bind (&Node::BindToScene, _1, _2, _3, NodeTransformSpace_Local)),
    make_invoker<void (Node&, Scene&)> (xtl::bind (&Node::BindToScene, _1, _2, NodeBindMode_Default, NodeTransformSpace_Local))));

  lib.Register ("Unbind",      make_invoker (&Node::Unbind));
  lib.Register ("UnbindChild", make_invoker (make_invoker (implicit_cast<void (Node::*) (const char*, NodeTransformSpace)> (&Node::UnbindChild)), 
                                             make_invoker (implicit_cast<void (Node::*) (const char*, NodeSearchMode, NodeTransformSpace)> (&Node::UnbindChild)),
                                             make_invoker<void (Node&, const char*)> (xtl::bind (implicit_cast<void (Node::*) (const char*, NodeTransformSpace)> (&Node::UnbindChild), _1, _2, NodeTransformSpace_Local)),
                                             make_invoker<void (Node&, const char*, NodeSearchMode)> (xtl::bind (implicit_cast<void (Node::*) (const char*, NodeSearchMode, NodeTransformSpace)> (&Node::UnbindChild), _1, _2, _3, NodeTransformSpace_Local))));
  lib.Register ("UnbindAllChildren", make_invoker (&Node::UnbindAllChildren));
  
  lib.Register ("FindChild", make_invoker (make_invoker<Node::Pointer (Node&, const char*)> (xtl::bind (implicit_cast<Node::Pointer (Node::*) (const char*, NodeSearchMode)> (&Node::FindChild), _1, _2, NodeSearchMode_Default)),
                                           make_invoker (implicit_cast<Node::Pointer (Node::*) (const char*, NodeSearchMode)> (&Node::FindChild))));

  lib.Register ("ObjectTM", make_invoker (&Node::ObjectTM));

  lib.Register ("Translate", make_invoker (make_invoker (implicit_cast<void (Node::*) (const vec3f&, NodeTransformSpace)> (&Node::Translate)),
                 make_invoker (implicit_cast<void (Node::*) (float, float, float, NodeTransformSpace)> (&Node::Translate)),
                 make_invoker<void (Node&, const vec3f&)> (xtl::bind (implicit_cast<void (Node::*) (const vec3f&, NodeTransformSpace)> (&Node::Translate), _1, _2, NodeTransformSpace_Parent)),
                 make_invoker<void (Node&, float, float, float)> (xtl::bind (implicit_cast<void (Node::*) (float, float, float, NodeTransformSpace)> (&Node::Translate), _1, _2, _3, _4, NodeTransformSpace_Parent))));
  lib.Register ("Rotate", make_invoker (make_invoker (implicit_cast<void (Node::*) (const quatf&, NodeTransformSpace)> (&Node::Rotate)),
                 make_invoker (implicit_cast<void (Node::*) (float, float, float, NodeTransformSpace)> (&Node::Rotate)),
                 make_invoker (implicit_cast<void (Node::*) (float, float, float, float, NodeTransformSpace)> (&Node::Rotate)),
                 make_invoker<void (Node&, const quatf&)> (xtl::bind (implicit_cast<void (Node::*) (const quatf&, NodeTransformSpace)> (&Node::Rotate), _1, _2, NodeTransformSpace_Parent)),
                 make_invoker<void (Node&, float, float, float)> (xtl::bind (implicit_cast<void (Node::*) (float, float, float, NodeTransformSpace)> (&Node::Rotate), _1, _2, _3, _4, NodeTransformSpace_Parent)),
                 make_invoker<void (Node&, float, float, float, float)> (xtl::bind (implicit_cast<void (Node::*) (float, float, float, float, NodeTransformSpace)> (&Node::Rotate), _1, _2, _3, _4, _5, NodeTransformSpace_Parent))));
  lib.Register ("Rescale", make_invoker (make_invoker (implicit_cast<void (Node::*) (const vec3f&)> (&Node::Scale)),
                                         make_invoker (implicit_cast<void (Node::*) (float, float, float)> (&Node::Scale))));

  lib.Register ("BeginUpdate", make_invoker (&Node::BeginUpdate));
  lib.Register ("EndUpdate",   make_invoker (&Node::EndUpdate));

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

  lib.Register ("set_WireColor",  make_invoker (implicit_cast<void (Entity::*) (const vec3f&)> (&Entity::SetWireColor)));
  lib.Register ("get_WireColor",  make_invoker (&Entity::WireColor));

  lib.Register ("SetWireColor", make_invoker (implicit_cast<void (Entity::*) (float, float, float)> (&Entity::SetWireColor)));

  lib.Register ("BoundBox", make_invoker (&Entity::BoundBox));
  lib.Register ("WorldBoundBox", make_invoker (&Entity::WorldBoundBox));
  
  lib.Register ("IsInfiniteBounds", make_invoker (&Entity::IsInfiniteBounds));

  lib.Register ("ChildrenBoundBox",      make_invoker (&Entity::ChildrenBoundBox));
  lib.Register ("FullBoundBox",          make_invoker (&Entity::FullBoundBox));
  lib.Register ("WorldChildrenBoundBox", make_invoker (&Entity::WorldChildrenBoundBox));
  lib.Register ("WorldFullBoundBox",     make_invoker (&Entity::WorldFullBoundBox));

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

  lib.Register ("set_Gain", make_invoker (&SoundEmitter::SetGain));
  lib.Register ("get_Gain", make_invoker (&SoundEmitter::Gain));

  lib.Register ("get_SoundDeclarationName", make_invoker (&SoundEmitter::SoundDeclarationName));

  lib.Register ("Play", make_invoker (&SoundEmitter::Play));
  lib.Register ("Stop", make_invoker (&SoundEmitter::Stop));

    //����������� ����� ������

  environment.RegisterType<SoundEmitter> (SCENE_SOUND_EMITTER_LIBRARY);
}

/*
    �������� �������
*/

Sprite::Pointer create_sprite ()
{
  return Sprite::Create ();
}

/*
   ����������� ���������� ������ � ���������
*/

void bind_sprite_library (Environment& environment, InvokerRegistry& entity_lib)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_SPRITE_LIBRARY);

    //������������

  lib.Register (entity_lib);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&create_sprite));

    //����������� ��������

  lib.Register ("set_Color",    make_invoker (implicit_cast<void (Sprite::*) (const vec4f&)> (&Sprite::SetColor)));
  lib.Register ("set_Material", make_invoker (&Sprite::SetMaterial));
  lib.Register ("set_Alpha",    make_invoker (&Sprite::SetAlpha));
  lib.Register ("set_Frame",    make_invoker (&Sprite::SetFrame));
  lib.Register ("get_Color",    make_invoker (&Sprite::Color));
  lib.Register ("get_Material", make_invoker (&Sprite::Material));
  lib.Register ("get_Alpha",    make_invoker (&Sprite::Alpha));
  lib.Register ("get_Frame",    make_invoker <int (Sprite&)> (&Sprite::Frame));

  lib.Register ("SetColor", make_invoker (make_invoker (implicit_cast<void (Sprite::*) (float, float, float, float)> (&Sprite::SetColor)),
                                          make_invoker (implicit_cast<void (Sprite::*) (float, float, float)>        (&Sprite::SetColor))));

    //����������� ����� ������

  environment.RegisterType<Sprite> (SCENE_SPRITE_LIBRARY);
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

  environment.RegisterType<VisualModel> (SCENE_VISUAL_MODEL_LIBRARY);
}

}

namespace script
{

namespace binds
{

/*
    ����������� ���������� ������ �� ������
*/

void bind_scene_graph_library (Environment& environment)
{
    //����������� ���������
  
  bind_scene_library (environment);

  InvokerRegistry& node_class_lib   = bind_node_library (environment);
  InvokerRegistry& entity_class_lib = bind_entity_library (environment, node_class_lib);

  bind_perspective_camera_library (environment, entity_class_lib);
  bind_ortho_camera_library       (environment, entity_class_lib);
  bind_light_library              (environment, entity_class_lib);
  bind_box_helper_library         (environment, entity_class_lib);
  bind_listener_library           (environment, entity_class_lib);
  bind_sound_emitter_library      (environment, entity_class_lib);
  bind_sprite_library             (environment, entity_class_lib);
  bind_visual_model_library       (environment, entity_class_lib);
}

}

}
