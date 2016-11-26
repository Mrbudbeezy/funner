#include "shared.h"

using namespace media::collada;

/*
    Описание реализации InstanceMesh
*/

struct InstanceMesh::Impl: public xtl::reference_counter
{
  stl::string            mesh;  //меш
  collada::MaterialBinds binds; //присоединённые материалы  
};

/*
    Конструкторы / деструктор / присваивание
*/

InstanceMesh::InstanceMesh ()
  : impl (new Impl, false)
  {}

InstanceMesh::InstanceMesh (Impl* in_impl)
  : impl (in_impl, false)
  {}
  
InstanceMesh::InstanceMesh (const InstanceMesh& imesh)
  : impl (imesh.impl)
  {}  

InstanceMesh::~InstanceMesh ()
{
}

InstanceMesh& InstanceMesh::operator = (const InstanceMesh& imesh)
{
  impl = imesh.impl;
  
  return *this;
}

/*
    Создание копии
*/

InstanceMesh InstanceMesh::Clone () const
{
  return InstanceMesh (new Impl (*impl));
}

/*
    Меш
*/

const char* InstanceMesh::Mesh () const
{
  return impl->mesh.c_str ();
}

void InstanceMesh::SetMesh (const char* mesh_id)
{
  if (!mesh_id)
    throw xtl::make_null_argument_exception ("media::collada::InstanceMesh::SetMesh", "mesh_id");
    
  impl->mesh = mesh_id;
}

/*
    Присоединённые материалы
*/

media::collada::MaterialBinds& InstanceMesh::MaterialBinds ()
{
  return impl->binds;
}

const media::collada::MaterialBinds& InstanceMesh::MaterialBinds () const
{
  return impl->binds;
}
