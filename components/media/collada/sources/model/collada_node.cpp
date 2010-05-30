#include "shared.h"

using namespace media::collada;

/*
    �������� ���������� ����
*/

typedef media::CollectionImpl<Node, ICollection<Node> >                             NodeListImpl;
typedef media::CollectionImpl<Light,ICollection<Light> >                            LightListImpl;
typedef media::CollectionImpl<Camera, ICollection<Camera> >                         CameraListImpl;
typedef media::CollectionImpl<InstanceMesh, ICollection<InstanceMesh> >             InstanceMeshListImpl;
typedef media::CollectionImpl<InstanceController, ICollection<InstanceController> > InstanceControllerListImpl;

struct Node::Impl: public xtl::reference_counter
{
  stl::string                id;             //������������� ����
  stl::string                sid;            //������������� ���� � �������� ��������
  stl::string                name;           //��� ����
  math::mat4f                tm;             //������� �������������� ����
  math::vec3f                rotation_pivot; //����� ���������
  math::vec3f                scale_pivot;    //����� ���������������
  NodeListImpl               nodes;          //��������� ����
  LightListImpl              lights;         //��������� �����
  CameraListImpl             cameras;        //������
  InstanceMeshListImpl       meshes;         //����
  InstanceControllerListImpl controllers;    //�����������
};

/*
    ������������ / ���������� / ������������
*/

Node::Node ()
  : impl (new Impl, false)
  {}

Node::Node (Impl* in_impl)
  : impl (in_impl, false)
  {}  

Node::Node (const Node& node)
  : impl (node.impl)
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
    �������� �����
*/

Node Node::Clone () const
{
  return Node (new Impl (*impl));
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
    throw xtl::make_null_argument_exception ("media::collada::Node::SetId", "id");
    
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
    throw xtl::make_null_argument_exception ("media::collada::Node::SetSubId", "sid");
    
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
    throw xtl::make_null_argument_exception ("media::collada::Node::SetName", "name");
    
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
    ������ �������� � ���������������
*/

const math::vec3f& Node::RotationPivot () const
{
  return impl->rotation_pivot;
}

const math::vec3f& Node::ScalePivot () const
{
  return impl->scale_pivot;
}

void Node::SetRotationPivot (const math::vec3f& pivot)
{
  impl->rotation_pivot = pivot;
}

void Node::SetScalePivot (const math::vec3f& pivot)
{
  impl->scale_pivot = pivot;
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

Node::ControllerList& Node::Controllers ()
{
  return impl->controllers;
}

const Node::ControllerList& Node::Controllers () const
{
  return impl->controllers;
}

/*
    ����� ������� �� Sub-ID
*/

const Node* Node::FindChild (const char* sub_id) const
{
  if (!sub_id)
    return 0;
    
  for (size_t i=0, count=impl->nodes.Size (); i<count; i++)
  {
    const Node& child = impl->nodes [i];
    
    if (!::strcmp (child.Id (), sub_id))
      return &child;
      
    const Node* result = child.FindChild (sub_id);
    
    if (result)
      return result;
  }
  
  return 0;
}

Node* Node::FindChild (const char* sub_id)
{
  return const_cast<Node*> (const_cast<const Node&> (*this).FindChild (sub_id));
}
