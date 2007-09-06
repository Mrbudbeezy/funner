#include <media/collada/model.h>

#include <common/exception.h>
#include <stl/string>

#include "shared.h"
#include "library.h"

using namespace medialib::collada;
using namespace common;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

/*
    �������� ���������� ������
*/

namespace medialib
{

namespace collada
{

struct ModelImpl
{
  stl::string       name;         //��� ������
  stl::string       active_scene; //��� �������� �����
  Library<Effect>   effects;      //���������� ��������
  Library<Material> materials;    //���������� ����������
  Library<Mesh>     meshes;       //���������� �����
  Library<Morph>    morphs;       //���������� ��������
  Library<Skin>     skins;        //���������� ������
  Library<Node>     nodes;        //���������� �����  
  Library<Scene>    scenes;       //���������� ����
  Library<Light>    lights;       //���������� ���������� �����
  Library<Camera>   cameras;      //���������� �����
  
  ModelImpl () : effects (this), materials (this), meshes (this), skins (this), morphs (this), nodes (this), scenes (this),
                 lights (this), cameras (this) {}
};

}

}

/*
    �������
*/

namespace
{

void default_collada_log (const char*)
{
}

}

/*
    ������������ / ����������
*/

Model::Model  ()
  : impl (new ModelImpl) {}

Model::Model (const char* file_name)
  : impl (new ModelImpl)
{
  ModelSystemSingleton::Instance ().Load (file_name, *this, &default_collada_log);
}

Model::Model (const char* file_name, const LogFunction& log)
  : impl (new ModelImpl)
{
  ModelSystemSingleton::Instance ().Load (file_name, *this, log);
}
  
Model::~Model ()
{
}

/*
    ��� ������
*/

const char* Model::Name () const
{
  return impl->name.c_str ();
}

void Model::Rename (const char* new_name)
{
  if (!new_name)
    RaiseNullArgument ("medialib::collada::Model::Rename", "new_name");
    
  impl->name = new_name;
}

/*
    ��� �������� �����
*/

const char* Model::ActiveSceneName () const
{
  return impl->active_scene.c_str ();
}

void Model::SetActiveSceneName (const char* scene_name)
{
  if (!scene_name)
    RaiseNullArgument ("medialib::collada::Model::SetActiveSceneName", "scene_name");
    
  impl->active_scene = scene_name;
}

/*
    ����������
*/

MaterialLibrary& Model::Materials ()
{
  return impl->materials;
}

const MaterialLibrary& Model::Materials () const
{
  return impl->materials;
}

EffectLibrary& Model::Effects ()
{
  return impl->effects;
}

const EffectLibrary& Model::Effects () const
{
  return impl->effects;
}

MeshLibrary& Model::Meshes ()
{
  return impl->meshes;
}

const MeshLibrary& Model::Meshes () const
{
  return impl->meshes;
}

MorphLibrary& Model::Morphs ()
{
  return impl->morphs;
}

const MorphLibrary& Model::Morphs () const
{
  return impl->morphs;
}

SkinLibrary& Model::Skins ()
{
  return impl->skins;
}

const SkinLibrary& Model::Skins () const
{
  return impl->skins;
}

LightLibrary& Model::Lights ()
{
  return impl->lights;
}

const LightLibrary& Model::Lights () const
{
  return impl->lights;
}

CameraLibrary& Model::Cameras ()
{
  return impl->cameras;
}

const CameraLibrary& Model::Cameras () const
{
  return impl->cameras;
}

NodeLibrary& Model::Nodes ()
{
  return impl->nodes;
}

const NodeLibrary& Model::Nodes () const
{
  return impl->nodes;
}

SceneLibrary& Model::Scenes ()
{
  return impl->scenes;
}

const SceneLibrary& Model::Scenes () const
{
  return impl->scenes;
}

/*
    �����
*/

void Model::Swap (Model& model)
{
  stl::swap (model.impl, impl);
}

namespace medialib
{

namespace collada
{

void swap (Model& a, Model& b)
{
  a.Swap (b);
}

}

}

/*
    �������� / ����������
*/

void Model::Load (const char* file_name)
{
  Model (file_name).Swap (*this);
}

void Model::Load (const char* file_name, const LogFunction& log)
{
  Model (file_name, log).Swap (*this);
}

void Model::Save (const char* file_name)
{
  ModelSystemSingleton::Instance ().Save (file_name, *this, &default_collada_log);
}

void Model::Save (const char* file_name, const LogFunction& log)
{
  ModelSystemSingleton::Instance ().Save (file_name, *this, log);
}

/*
    ����������� ����� ������
*/

namespace medialib
{

namespace collada
{

const char* get_model_name (ModelImpl* impl)
{
  return impl->name.c_str ();
}

}

}
