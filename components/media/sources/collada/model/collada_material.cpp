#include "shared.h"

using namespace media::collada;
using namespace media;
using namespace common;

/*
    ���������� ����������
*/

struct Material::Impl: public xtl::reference_counter
{
  stl::string effect; //������, ��������� � ����������
  stl::string id;     //������������� ���������  
};

/*
    ������������ / ���������� / ������������
*/

Material::Material ()
  : impl (new Impl, false)
    {}
    
Material::Material (const Material& mtl, media::CloneMode mode)
  : impl (media::clone (mtl.impl, mode, "media::collada::Material::Material"))
  {}

Material::~Material ()
{
}

Material& Material::operator = (const Material& mtl)
{
  impl = mtl.impl;
  
  return *this;
}

/*
    ������������� ���������
*/

const char* Material::Id () const
{
  return impl->id.c_str ();
}

void Material::SetId (const char* id)
{
  if (!id)
    RaiseNullArgument ("media::collada::Material::SetId", "id");
    
  impl->id = id;
}

/*
    ������ ��������� � ����������
*/

const char* Material::Effect () const
{
  return impl->effect.c_str ();
}

void Material::SetEffect (const char* effect_id)
{
  if (!effect_id)
    RaiseNullArgument ("media::collada::Material::SetEffect", "effect_id");

  impl->effect = effect_id;
}
