#include "shared.h"

using namespace scene_graph;
using namespace stl;
using namespace common;

/*
    �������� ���������� StaticMesh
*/

struct StaticMesh::Impl: public xtl::instance_counter<StaticMesh>
{
  string mesh_name;       //��� ����  
  size_t mesh_name_hash;  //��� ����� ����
  
///�����������
  Impl () : mesh_name_hash (0xffffffff) {}
};

/*
    ����������� / ����������
*/

StaticMesh::StaticMesh ()
  : impl (new Impl)
  {}

StaticMesh::~StaticMesh ()
{
  delete impl;
}

/*
    �������� ������
*/

StaticMesh::Pointer StaticMesh::Create ()
{
  return Pointer (new StaticMesh, false);
}

/*
    ��������� ����� ���� ������
*/

void StaticMesh::SetMeshName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("scene_graph::StaticMesh::SetMeshName", "name");
    
  impl->mesh_name      = name;
  impl->mesh_name_hash = strhash (name);
}

const char* StaticMesh::MeshName () const
{
  return impl->mesh_name.c_str ();
}

size_t StaticMesh::MeshNameHash () const
{
  return impl->mesh_name_hash;
}

/*
    �����, ���������� ��� ��������� �������
*/

void StaticMesh::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}

/*
    ���������� �������
*/

void StaticMesh::BindProperties (common::PropertyBindingMap& bindings)
{
  Entity::BindProperties (bindings);

  bindings.AddProperty ("Mesh", xtl::bind (&StaticMesh::MeshName, this), xtl::bind (&StaticMesh::SetMeshName, this, _1));
}
