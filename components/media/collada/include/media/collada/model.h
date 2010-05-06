#ifndef MEDIALIB_COLLADA_MODEL_HEADER
#define MEDIALIB_COLLADA_MODEL_HEADER

#include <xtl/functional_fwd>
#include <stl/auto_ptr.h>
#include <media/collada/collection.h>
#include <media/collada/library.h>
#include <common/serializer_manager.h>

namespace media
{

namespace collada
{

//forward declarations
class Animation;
class Effect;
class Image;
class Material;
class Mesh;
class Morph;
class Skin;
class Light;
class Camera;
class Node;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef ICollection<Animation> AnimationList;

typedef ILibrary<Effect>    EffectLibrary;
typedef ILibrary<Image>     ImageLibrary;
typedef ILibrary<Material>  MaterialLibrary;
typedef ILibrary<Mesh>      MeshLibrary;
typedef ILibrary<Morph>     MorphLibrary;
typedef ILibrary<Skin>      SkinLibrary;
typedef ILibrary<Light>     LightLibrary;
typedef ILibrary<Camera>    CameraLibrary;
typedef ILibrary<Node>      NodeLibrary;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Model
{
  public:
    typedef xtl::function<void (const char*)> LogHandler;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Model  ();
    Model  (const char* file_name);
    Model  (const char* file_name, const LogHandler& log_func);
    Model  (const Model&);
    ~Model ();
    
    Model& operator = (const Model&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name   () const;
    void        Rename (const char* new_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Load (const char* file_name);
    void Load (const char* file_name, const LogHandler& log);
    void Save (const char* file_name);
    void Save (const char* file_name, const LogHandler& log);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* ActiveSceneName    () const;
    void        SetActiveSceneName (const char* scene_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
          collada::AnimationList&     Animations ();
          collada::MaterialLibrary&   Materials  ();
          collada::EffectLibrary&     Effects    ();
          collada::ImageLibrary&      Images     ();
          collada::MeshLibrary&       Meshes     ();
          collada::MorphLibrary&      Morphs     ();
          collada::SkinLibrary&       Skins      ();
          collada::LightLibrary&      Lights     ();
          collada::CameraLibrary&     Cameras    ();
          collada::NodeLibrary&       Nodes      ();
          collada::NodeLibrary&       Scenes     ();
    const collada::AnimationList&     Animations () const;
    const collada::MaterialLibrary&   Materials  () const;
    const collada::EffectLibrary&     Effects    () const;
    const collada::ImageLibrary&      Images     () const;
    const collada::MeshLibrary&       Meshes     () const;
    const collada::MorphLibrary&      Morphs     () const;
    const collada::SkinLibrary&       Skins      () const;
    const collada::LightLibrary&      Lights     () const;
    const collada::CameraLibrary&     Cameras    () const;
    const collada::NodeLibrary&       Nodes      () const;
    const collada::NodeLibrary&       Scenes     () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Model&);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Model&, Model&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////    
enum ModelSelect
{
  ModelSelect_ActiveScene = 1,    //������� �������, �������� � �������� �����
  ModelSelect_Scenes      = 2,    //������� �������, �������� � ���������� ����
  ModelSelect_Nodes       = 4,    //������� �������, �������� � ���������� �����
  ModelSelect_Cameras     = 8,    //������� �������, �������� � ���������� �����
  ModelSelect_Lights      = 16,   //������� �������, �������� � ���������� ���������� �����
  ModelSelect_Skins       = 32,   //������� �������, �������� � ���������� ������
  ModelSelect_Morphs      = 64,   //������� �������, �������� � ���������� ������
  ModelSelect_Meshes      = 128,  //������� �������, �������� � ���������� �����
  ModelSelect_Images      = 256,  //������� �������, �������� � ���������� ��������
  ModelSelect_Effects     = 512,  //������� �������, �������� � ���������� ��������
  ModelSelect_Materials   = 1024, //������� �������, �������� � ���������� ����������
  ModelSelect_Animations  = 2048, //������� �������, �������� � ���������� ��������
  
  ModelSelect_Default     = ModelSelect_ActiveScene,
  ModelSelect_All         = ~0
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������ ������������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////    
void remove_unused_resources (Model& model, size_t select_flags = ModelSelect_Default, const char* force_select = "");

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::ResourceSerializerManager<void (const char* file_name, Model& model, const Model::LogHandler& log),
  void (const char* file_name, const Model& model, const Model::LogHandler& log)> ModelManager;

}

}

#endif
