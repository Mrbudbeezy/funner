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
    throw xtl::make_null_argument_exception ("media::geometry::MeshLibrary::Rename", name);
    
  impl->name = name;
}

/*
    ���������� ����� / �������� �� �������
*/

uint32_t MeshLibrary::Size () const
{
  return (uint32_t)impl->meshes.Size ();
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
    ������������� ���� � ����������
*/

const char* MeshLibrary::ItemId (const ConstIterator& i) const
{
  return impl->meshes.ItemId (i);
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

void MeshLibrary::Attach (const char* name, Mesh& mesh)
{
  if (Size () == (uint32_t)-1)
    throw xtl::format_operation_exception ("media::geometry::MeshLibrary::Attach", "Mesh library max size exceeded");

  impl->meshes.Insert (name, mesh);
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
    throw xtl::make_null_argument_exception ("media::MeshLibrary::Load", "file_name");

  try
  {
    static ComponentLoader loader ("media.geometry.loaders.*");

    MeshLibraryManager::GetLoader (file_name, SerializerFindMode_ByName)(file_name, *this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::MeshLibrary::Load");
    throw;
  }
}

void MeshLibrary::Save (const char* file_name)
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("media::MeshLibrary::Save", "file_name");

  try
  {
    static ComponentLoader loader ("media.geometry.savers.*");
    
    MeshLibraryManager::GetSaver (file_name, SerializerFindMode_ByName)(file_name, *this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::MeshLibrary::Save");
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

/*
    ����������� �������������� �� ���������
*/

void MeshLibrary::RegisterDefaultSavers ()
{
  static ComponentLoader loader ("media.geometry.savers.*");
}

void MeshLibrary::RegisterDefaultLoaders ()
{
  static ComponentLoader loader ("media.geometry.loaders.*");
}
