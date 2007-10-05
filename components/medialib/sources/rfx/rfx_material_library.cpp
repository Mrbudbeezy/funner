#include "shared.h"

using namespace media::rfx;
using namespace media;
using namespace stl;
using namespace common;

/*
    �������� ���������� ���������� ����������
*/

typedef ResourceLibrary<Material> MaterialLibraryHolder;

struct MaterialLibrary::Impl
{
  string                name;      //��� ������
  MaterialLibraryHolder materials; //���������� ����������
};

/*
    ������������ / ����������
*/

MaterialLibrary::MaterialLibrary ()
  : impl (new Impl)
  {}

MaterialLibrary::MaterialLibrary (const char* file_name)
  : impl (new Impl)
{
  Load (file_name);
  Rename (file_name);
}

MaterialLibrary::MaterialLibrary (const MaterialLibrary& library)
  : impl (new Impl (*library.impl))
  {}

MaterialLibrary::~MaterialLibrary ()
{
}

/*
    ������������
*/

MaterialLibrary& MaterialLibrary::operator = (const MaterialLibrary& library)
{
  MaterialLibrary (library).Swap (*this);
  return *this;
}

/*
    ��� ������
*/

const char* MaterialLibrary::Name () const
{
  return impl->name.c_str ();
}

void MaterialLibrary::Rename (const char* name)
{
  if (!name)
    RaiseNullArgument ("media::rfx::MaterialLibrary::Rename", name);
    
  impl->name = name;
}

/*
    ���������� ����� / �������� �� �������
*/

size_t MaterialLibrary::Size () const
{
  return impl->materials.Size ();
}

bool MaterialLibrary::IsEmpty () const
{
  return impl->materials.IsEmpty ();
}

/*
    ��������� ���������
*/

MaterialLibrary::Iterator MaterialLibrary::CreateIterator ()
{
  return impl->materials.CreateIterator ();
}

MaterialLibrary::ConstIterator MaterialLibrary::CreateIterator () const
{
  return impl->materials.CreateIterator ();
}

/*
    �����
*/

Material* MaterialLibrary::Find (const char* name)
{
  return impl->materials.Find (name);
}

const Material* MaterialLibrary::Find (const char* name) const
{
  return impl->materials.Find (name);
}

/*
    ������������� �����
*/

void MaterialLibrary::Attach (const char* name, Material& material, CloneMode mode)
{
  impl->materials.Insert (name, material, mode);
}

void MaterialLibrary::Detach (const char* name)
{
  impl->materials.Remove (name);
}

void MaterialLibrary::DetachAll ()
{
  impl->materials.Clear ();
}

/*
    �������
*/

void MaterialLibrary::Clear ()
{
  DetachAll ();
  
  impl->name.clear ();
}

/*
    �������� / ����������
*/

void MaterialLibrary::Load (const char* file_name)
{
  if (!file_name)
    RaiseNullArgument ("media::MaterialLibrary::Load", "file_name");
    
  try
  {
    MaterialLibraryManagerSingleton::Instance ().Load (file_name, *this);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("media::MaterialLibrary::Load");
    throw;
  }
}

void MaterialLibrary::Save (const char* file_name)
{
  if (!file_name)
    RaiseNullArgument ("media::MaterialLibrary::Save", "file_name");
    
  try
  {
    MaterialLibraryManagerSingleton::Instance ().Save (file_name, *this);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("media::MaterialLibrary::Save");
    throw;
  }
}

/*
    �����
*/

void MaterialLibrary::Swap (MaterialLibrary& library)
{
  swap (library.impl, impl);
}

namespace media
{

namespace rfx
{

void swap (MaterialLibrary& material1, MaterialLibrary& material2)
{
  material1.Swap (material2);
}

}

}
