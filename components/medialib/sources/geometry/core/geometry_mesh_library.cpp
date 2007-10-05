#include "shared.h"

using namespace media::geometry;
using namespace media;
using namespace common;

/*
    �������� ���������� ��������� �����
*/

typedef ResourceLibrary<Mesh> MeshLibraryHolder;

struct MeshLibrary::Impl
{
  stl::string       name;   //��� ������
  MeshLibraryHolder meshes; //���������� �����
};

/*
    ������������ / ����������
*/

MeshLibrary::MeshLibrary ()
  : impl (new Impl)
  {}

MeshLibrary::MeshLibrary (const char* file_name)
  : impl (new Impl)
{
  Load (file_name);
  Rename (file_name);
}

MeshLibrary::MeshLibrary (const MeshLibrary& model)
  : impl (new Impl (*model.impl))
  {}

MeshLibrary::~MeshLibrary ()
{
}

/*
    ������������
*/

MeshLibrary& MeshLibrary::operator = (const MeshLibrary& model)
{
  MeshLibrary (model).Swap (*this);
  return *this;
}

/*
    ��� ����������
*/

const char* MeshLibrary::Name () const
{
  return impl->name.c_str ();
}

void MeshLibrary::Rename (const char* name)
{
  if (!name)
    RaiseNullArgument ("media::geometry::MeshLibrary::Rename", name);
    
  impl->name = name;
}

/*
    ���������� ����� / �������� �� �������
*/

size_t MeshLibrary::Size () const
{
  return impl->meshes.Size ();
}

bool MeshLibrary::IsEmpty () const
{
  return impl->meshes.IsEmpty ();
}

/*
    ��������� ���������
*/

MeshLibrary::Iterator MeshLibrary::CreateIterator ()
{
  return impl->meshes.CreateIterator ();
}

MeshLibrary::ConstIterator MeshLibrary::CreateIterator () const
{
  return impl->meshes.CreateIterator ();
}

/*
    �����
*/

Mesh* MeshLibrary::Find (const char* name)
{
  return impl->meshes.Find (name);
}

const Mesh* MeshLibrary::Find (const char* name) const
{
  return impl->meshes.Find (name);
}

/*
    ������������� �����
*/

void MeshLibrary::Attach (const char* name, Mesh& mesh, CloneMode mode)
{
  impl->meshes.Insert (name, mesh, mode);
}

void MeshLibrary::Detach (const char* name)
{
  impl->meshes.Remove (name);
}

void MeshLibrary::DetachAll ()
{
  impl->meshes.Clear ();
}

/*
    ������� ����������
*/

void MeshLibrary::Clear ()
{
  DetachAll ();
  
  impl->name.clear ();
}

/*
    �������� / ����������
*/

void MeshLibrary::Load (const char* file_name)
{
  if (!file_name)
    RaiseNullArgument ("media::MeshLibrary::Load", "file_name");

  try
  {
    MeshLibraryManagerSingleton::Instance ().Load (file_name, *this);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("media::MeshLibrary::Load");
    throw;
  }
}

void MeshLibrary::Save (const char* file_name)
{
  try
  {
    MeshLibraryManagerSingleton::Instance ().Save (file_name, *this);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("media::MeshLibrary::Save");
    throw;
  }
}

/*
    �����
*/

void MeshLibrary::Swap (MeshLibrary& model)
{
  stl::swap (model.impl, impl);
}

namespace media
{

namespace geometry
{

void swap (MeshLibrary& model1, MeshLibrary& model2)
{
  model1.Swap (model2);
}

}

}
