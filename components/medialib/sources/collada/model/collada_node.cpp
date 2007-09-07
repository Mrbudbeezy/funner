#include <media/collada/scene.h>
#include <stl/string>

#include "collection.h"

using namespace media::collada;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

/*
    ��������� �����
*/

namespace
{

class ConstructableInstanceMesh: public InstanceMesh
{
  public:
    ConstructableInstanceMesh (media::collada::Mesh& mesh) : InstanceMesh (mesh) {}
    ~ConstructableInstanceMesh () {}  
};

}

class InstanceMeshCollection: public Collection<InstanceMesh, ConstructableInstanceMesh, true>
{
  public:
    InstanceMeshCollection (Entity& owner) : Collection<InstanceMesh, ConstructableInstanceMesh, true> (owner) {}
  
    InstanceMesh& Create (Mesh& mesh)
    {
      ConstructableInstanceMesh* instance_mesh = new ConstructableInstanceMesh (mesh);
      
      try
      {
        InsertCore (*instance_mesh);

        return *instance_mesh;
      }
      catch (...)
      {
        delete instance_mesh;
        throw;
      }    
    }
};

/*
    �������� ���������� ����
*/

typedef Collection<Node>       NodeListImpl;
typedef Collection<Light>      LightListImpl;
typedef Collection<Camera>     CameraListImpl;

struct Node::Impl
{
  stl::string            sid;     //������������� ���� � �������� ��������
  stl::string            name;    //��� ����
  math::mat4f            tm;      //������� �������������� ����
  NodeListImpl           nodes;   //��������� ����
  LightListImpl          lights;  //��������� �����
  CameraListImpl         cameras; //������
  InstanceMeshCollection meshes;  //����

  Impl (Entity& entity) : nodes (entity), lights (entity), cameras (entity), meshes (entity) {}
};

/*
    ����������� / ����������
*/

Node::Node (ModelImpl* owner, const char* id)
  : Entity (owner, id), impl (new Impl (*this))
  {}

Node::~Node ()
{
  delete impl;
}

/*
    ������������� ���� � �������� ������������� ����
*/

const char* Node::SubId () const
{
  return impl->sid.c_str ();
}

void Node::SetSubId (const char* sid)
{
  if (!sid)
    common::RaiseNullArgument ("media::collada::Node::SetSubId", "sid");
    
  impl->sid = sid;
}

/*
    ��� ����
*/

const char* Node::Name () const
{
  return impl->name.c_str ();
}

void Node::SetName (const char* name)
{
  if (!name)
    common::RaiseNullArgument ("media::collada::Node::SetName", "name");
    
  impl->name = name;
}
  
/*
    �������������� ����
*/

const math::mat4f& Node::Transform () const
{
  return impl->tm;
}

void Node::SetTransform (const math::mat4f& tm)
{
  impl->tm = tm;
}

/*
    ��������� ����
*/

Node::NodeList& Node::Nodes ()
{
  return impl->nodes;
}

const Node::NodeList& Node::Nodes () const
{
  return impl->nodes;
}

Node::LightList& Node::Lights ()
{
  return impl->lights;
}

const Node::LightList& Node::Lights () const
{
  return impl->lights;
}

Node::CameraList& Node::Cameras ()
{
  return impl->cameras;
}

const Node::CameraList& Node::Cameras () const
{
  return impl->cameras;
}

Node::MeshList& Node::Meshes ()
{
  return impl->meshes;
}

const Node::MeshList& Node::Meshes () const
{
  return impl->meshes;
}
