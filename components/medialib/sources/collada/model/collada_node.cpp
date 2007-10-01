#include "shared.h"

using namespace media::collada;

/*
    �������� ���������� ����
*/

typedef CollectionImpl<Node>         NodeListImpl;
typedef CollectionImpl<Light>        LightListImpl;
typedef CollectionImpl<Camera>       CameraListImpl;
typedef CollectionImpl<InstanceMesh> InstanceMeshListImpl;

struct Node::Impl: public xtl::reference_counter
{
  stl::string          id;      //������������� ����
  stl::string          sid;     //������������� ���� � �������� ��������
  stl::string          name;    //��� ����
  math::mat4f          tm;      //������� �������������� ����
  NodeListImpl         nodes;   //��������� ����
  LightListImpl        lights;  //��������� �����
  CameraListImpl       cameras; //������
  InstanceMeshListImpl meshes;  //����
};

/*
    ������������ / ���������� / ������������
*/

Node::Node ()
  : impl (new Impl, false)
  {}
  
Node::Node (const Node& node, media::CloneMode mode)
  : impl (media::clone (node.impl, mode, "media::collada::Node::Node"))
  {}

Node::~Node ()
{
}

Node& Node::operator = (const Node& node)
{
  impl = node.impl;

  return *this;
}

/*
    ������������� ����
*/

const char* Node::Id () const
{
  return impl->id.c_str ();
}

void Node::SetId (const char* id)
{
  if (!id)
    common::RaiseNullArgument ("media::collada::Node::SetId", "id");
    
  impl->id = id;
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
