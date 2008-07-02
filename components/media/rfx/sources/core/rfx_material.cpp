#include "shared.h"

using namespace media::rfx;
using namespace media;
using namespace common;

/*
    �������� ���������� Material
*/

struct Material::Impl: public xtl::reference_counter
{
  stl::string name;       //��� ���������
  size_t      name_hash;  //��� �����
  int         sort_group; //������ ����������

  Impl () : name_hash (strhash ("")), sort_group (0) {}
};

/*
    ������������ / ����������
*/

Material::Material ()
  : impl (new Impl)
  {}

Material::Material (const Material& material)
  : impl (new Impl (*material.impl))
  {}

Material::~Material ()
{
}

/*
    �����������
*/

Material::Pointer Material::Clone () const
{
  return Pointer (CloneCore (), false);
}

Material* Material::CloneCore () const
{
  return new Material (*this);
}

/*
    ��� ���������
*/

const char* Material::Name () const
{
  return impl->name.c_str ();
}

size_t Material::NameHash () const
{
  return impl->name_hash;
}

void Material::Rename (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::rfx::Material::Rename", "name");
        
  impl->name      = name;
  impl->name_hash = strhash (name);
}

/*
    ������������ ���������������
*/

void Material::Accept (Visitor& visitor)
{
  AcceptCore (visitor);
}

void Material::AcceptCore (Visitor& visitor)
{
  visitor (*this);
}

/*
    ��������� ������ ����������
*/

void Material::SetSortGroup (int group)
{
  impl->sort_group = group;
}

int Material::SortGroup () const
{
  return impl->sort_group;
}

/*
    ������� ������
*/

void Material::AddRef () const
{
  addref (*impl);
}

void Material::Release () const
{
  release (impl.get ());
}
