#include <media/collada/material.h>

using namespace medialib::collada;

/*
    ���������� ����������
*/

struct Material::Impl
{
  MaterialLibrary&           library; //����������
  medialib::collada::Effect& effect;  //������, ��������� � ����������
  
  Impl (medialib::collada::Effect& in_effect, MaterialLibrary& in_library) : library (in_library), effect (in_effect) {}
};

/*
    ����������� / ����������
*/

Material::Material (medialib::collada::Effect& effect, MaterialLibrary& library, const char* id)
  : Entity (library.Owner (), id),
    impl (new Impl (effect, library))
    {}

Material::~Material ()
{
  delete impl;
}

/*
    ����������
*/

MaterialLibrary& Material::Library () const
{
  return impl->library;
}

/*
    ������ ��������� � ����������
*/

const medialib::collada::Effect& Material::Effect () const
{
  return impl->effect;
}

medialib::collada::Effect& Material::Effect ()
{
  return impl->effect;
}
