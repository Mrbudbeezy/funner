#include "shared.h"

using namespace scene_graph;
using namespace stl;
using namespace common;

/*
    �������� ���������� VisualModel
*/

struct VisualModel::Impl
{
  string mesh_name; //��� ����  
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
    
  impl->mesh_name = name;
}

const char* VisualModel::MeshName () const
{
  return impl->mesh_name.c_str ();
}

/*
    �����, ���������� ��� ��������� �������
*/

void VisualModel::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
