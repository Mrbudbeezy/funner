#include <media/collada/material.h>

using namespace media::collada;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

/*
    ���������� ����������
*/

struct Material::Impl
{
  media::collada::Effect& effect;  //������, ��������� � ����������
  
  Impl (Entity& entity, media::collada::Effect& in_effect) : effect (in_effect)
  {
    if (effect.Owner () != entity.Owner ())
      raise_incompatible ("media::collada::Material::Material", effect, entity);
  }
};

/*
    ����������� / ����������
*/

Material::Material (media::collada::Effect& effect, ModelImpl* owner, const char* id)
  : Entity (owner, id),
    impl (new Impl (*this, effect))
    {}

Material::~Material ()
{
  delete impl;
}

/*
    ������ ��������� � ����������
*/

const media::collada::Effect& Material::Effect () const
{
  return impl->effect;
}

media::collada::Effect& Material::Effect ()
{
  return impl->effect;
}
