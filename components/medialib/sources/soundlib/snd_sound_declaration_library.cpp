#include "shared.h"

using namespace stl;
using namespace common;
using namespace media;

/*
    �������� ���������� SoundDeclarationLibrary
*/

typedef ResourceLibrary<SoundDeclaration> SoundDeclarationLibraryHolder;

struct SoundDeclarationLibrary::Impl
{
  string                        name;         //��� ����������
  SoundDeclarationLibraryHolder declarations; //���������� ������
};

/*
    ������������ / ����������
*/

SoundDeclarationLibrary::SoundDeclarationLibrary ()
  : impl (new Impl)
  {}

SoundDeclarationLibrary::SoundDeclarationLibrary (const char* file_name)
  : impl (new Impl)
{
  Load (file_name);
  Rename (file_name);
}

SoundDeclarationLibrary::SoundDeclarationLibrary (const SoundDeclarationLibrary& library)
  : impl (new Impl (*library.impl))
  {}

SoundDeclarationLibrary::~SoundDeclarationLibrary ()
{
}

/*
    ������������
*/

SoundDeclarationLibrary& SoundDeclarationLibrary::operator = (const SoundDeclarationLibrary& library)
{
  SoundDeclarationLibrary (library).Swap (*this);
  return *this;
}

/*
    ��� ������
*/

const char* SoundDeclarationLibrary::Name () const
{
  return impl->name.c_str ();
}

void SoundDeclarationLibrary::Rename (const char* name)
{
  if (!name)
    RaiseNullArgument ("media::SoundDeclarationLibrary::Rename", name);
    
  impl->name = name;
}

/*
    ���������� ������ / �������� �� �������
*/

size_t SoundDeclarationLibrary::Size () const
{
  return impl->declarations.Size ();
}

bool SoundDeclarationLibrary::IsEmpty () const
{
  return impl->declarations.IsEmpty ();
}

/*
    ��������� ���������
*/

SoundDeclarationLibrary::Iterator SoundDeclarationLibrary::CreateIterator ()
{
  return impl->declarations.CreateIterator ();
}

SoundDeclarationLibrary::ConstIterator SoundDeclarationLibrary::CreateIterator () const
{
  return impl->declarations.CreateConstIterator ();
}

/*
    ��� ���������� �� ���������
*/

const char* SoundDeclarationLibrary::ItemId (const Iterator& i)
{
  return impl->declarations.ItemId (i);
}

const char* SoundDeclarationLibrary::ItemId (const ConstIterator& i) const
{
  return impl->declarations.ItemId (i);
}

/*
    �����
*/

SoundDeclaration* SoundDeclarationLibrary::Find (const char* name)
{
  return impl->declarations.Find (name);
}

const SoundDeclaration* SoundDeclarationLibrary::Find (const char* name) const
{
  return impl->declarations.Find (name);
}

/*
    ������������� ������
*/

void SoundDeclarationLibrary::Attach (const char* name, SoundDeclaration& decl, CloneMode mode)
{
  impl->declarations.Insert (name, decl, mode);
}

void SoundDeclarationLibrary::Detach (const char* name)
{
  impl->declarations.Remove (name);
}

void SoundDeclarationLibrary::DetachAll ()
{
  impl->declarations.Clear ();
}

/*
    �������� / ����������
*/

void SoundDeclarationLibrary::Load (const char* file_name)
{
  if (!file_name)
    RaiseNullArgument ("media::SoundDeclarationLibrary::Load", "file_name");
    
  try
  {
    SoundDeclarationLibraryManagerSingleton::Instance ().Load (file_name, *this);    
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("media::SoundDeclarationLibrary::Load");
    throw;
  }
}

void SoundDeclarationLibrary::Save (const char* file_name)
{
  if (!file_name)
    RaiseNullArgument ("media::SoundDeclarationLibrary::Save", "file_name");
    
  try
  {
    SoundDeclarationLibraryManagerSingleton::Instance ().Save (file_name, *this);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("media::SoundDeclarationLibrary::Save");
    throw;
  }
}

/*
    �����
*/

void SoundDeclarationLibrary::Swap (SoundDeclarationLibrary& library)
{
  swap (library.impl, impl);
}

namespace media
{

void swap (SoundDeclarationLibrary& library1, SoundDeclarationLibrary& library2)
{
  library1.Swap (library2);
}

}
