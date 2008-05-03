#include "shared.h"

using namespace media::collada;
using namespace common;

/*
    ���������� Mesh
*/

typedef CollectionImpl<Surface> SurfaceCollection;

struct Mesh::Impl: public xtl::reference_counter
{
  SurfaceCollection surfaces; //����������� ����
  stl::string       id;       //������������� ����  
};

/*
    �����������
*/

Mesh::Mesh ()
  : impl (new Impl, false)
  {}
  
Mesh::Mesh (const Mesh& mesh, media::CloneMode mode)
  : impl (media::clone (mesh.impl, mode, "media::collada::Mesh::Mesh"))
  {}
  
Mesh::~Mesh ()
{
}

Mesh& Mesh::operator = (const Mesh& mesh)
{
  impl = mesh.impl;
  
  return *this;
}

/*
    ������������� ����
*/

const char* Mesh::Id () const
{
  return impl->id.c_str ();
}

void Mesh::SetId (const char* id)
{
  if (!id)
    RaiseNullArgument ("media::collada::Mesh::SetId", "id");
    
  impl->id = id;
}

/*
    ������ ������������
*/

Mesh::SurfaceList& Mesh::Surfaces ()
{
  return impl->surfaces;
}

const Mesh::SurfaceList& Mesh::Surfaces () const
{
  return impl->surfaces;
}
