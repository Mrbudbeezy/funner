#include "shared.h"

using namespace media::physics;

namespace
{

const float       DEFAULT_ANGULAR_DAMPING      = 0.f;
const math::vec3f DEFAULT_ANISOTROPIC_FRICTION (1.f);
const float       DEFAULT_FRICTION             = 0.5f;
const float       DEFAULT_LINEAR_DAMPING       = 0.f;
const float       DEFAULT_RESTITUTION          = 0.f;

}

/*
   ���������� ���������
*/

struct Material::Impl : public xtl::reference_counter
{
  stl::string name;                  //��� ���������
  math::vec3f anisotropic_friction;  //������ �� ������ ����
  float       linear_damping;        //�������� �����������
  float       angular_damping;       //������� �����������
  float       friction;              //������
  float       restitution;           //���������

  Impl ()
    : anisotropic_friction (DEFAULT_ANISOTROPIC_FRICTION)
    , linear_damping       (DEFAULT_LINEAR_DAMPING)
    , angular_damping      (DEFAULT_ANGULAR_DAMPING)
    , friction             (DEFAULT_FRICTION)
    , restitution          (DEFAULT_RESTITUTION)
    {}
};

/*
   ������������ / ���������� / ������������
*/

Material::Material ()
  : impl (new Impl)
{
}

Material::Material (const Material& source)
  : impl (source.impl)
{
  addref (impl);
}

Material::Material (Impl* in_impl)
  : impl (in_impl)
{
}

Material::~Material ()
{
  release (impl);
}

Material& Material::operator = (const Material& source)
{
  Material (source).Swap (*this);

  return *this;
}
    
/*
   �������� �����
*/

Material Material::Clone () const
{
  return Material (new Impl (*impl));
}

/*
   �������������
*/

size_t Material::Id () const
{
  return (size_t)impl;
}

/*
   ��� ���������
*/

const char* Material::Name () const
{
  return impl->name.c_str ();
}

void Material::Rename (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::physics::Material::Rename", "name");

  impl->name = name;
}
    
/*
   ���������� ��������/������� �������������
*/

float Material::LinearDamping () const
{
  return impl->linear_damping;
}

float Material::AngularDamping () const
{
  return impl->angular_damping;
}

void Material::SetLinearDamping (float value)
{
  impl->linear_damping = value;
}

void Material::SetAngularDamping (float value)
{
  impl->angular_damping = value;
}

/*
   ���������� �������
*/

float Material::Friction () const
{
  return impl->friction;
}

const math::vec3f& Material::AnisotropicFriction () const
{
  return impl->anisotropic_friction;
}

void Material::SetFriction (float value)
{
  impl->friction = value;
}

void Material::SetAnisotropicFriction (const math::vec3f& value)
{
  impl->anisotropic_friction = value;
}

/*
   ���������� ����������
*/

float Material::Restitution () const
{
  return impl->restitution;
}

void Material::SetRestitution (float value)
{
  impl->restitution = value;
}

/*
   �����
*/

void Material::Swap (Material& source)
{
  stl::swap (impl, source.impl);
}

namespace media
{

namespace physics
{

/*
   �����
*/

void swap (Material& material1, Material& material2)
{
  material1.Swap (material2);
}

}

}
