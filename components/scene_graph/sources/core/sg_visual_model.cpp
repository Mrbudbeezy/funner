#include "shared.h"

using namespace scene_graph;
using namespace stl;
using namespace common;

/*
    �������� ���������� VisualModel
*/

struct VisualModel::Impl
{
  string mesh_name;       //��� ����  
  size_t mesh_name_hash;  //��� ����� ����
  
///�����������
  Impl () : mesh_name_hash (0xffffffff) {}
};

/*
    ����������� / ����������
*/

VisualModel::VisualModel ()
  : impl (new Impl)
  {}

VisualModel::~VisualModel ()
{
  delete impl;
}

/*
    �������� ������
*/

VisualModel::Pointer VisualModel::Create ()
{
  return Pointer (new VisualModel, false);
}

/*
    ��������� ����� ���� ������
*/

void VisualModel::SetMeshName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("scene_graph::VisualModel::SetMeshName", "name");
    
  impl->mesh_name      = name;
  impl->mesh_name_hash = strhash (name);
}

const char* VisualModel::MeshName () const
{
  return impl->mesh_name.c_str ();
}

size_t VisualModel::MeshNameHash () const
{
  return impl->mesh_name_hash;
}

/*
    �����, ���������� ��� ��������� �������
*/

void VisualModel::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
