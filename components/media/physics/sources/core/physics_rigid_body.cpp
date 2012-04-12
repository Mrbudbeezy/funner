#include "shared.h"

using namespace media::physics;

/*
   ���������� �������� ����
*/

struct RigidBody::Impl : public xtl::reference_counter
{
  stl::string              name;                       //��� ����
  stl::string              collision_group;            //������ ��������
  float                    mass;                       //�����
  math::vec3f              center_of_mass_position;    //��������� ������ ����
  math::quatf              center_of_mass_orientation; //���������� ������ ����
  math::vec3f              mass_space_inertia_tensor;  //������
  media::physics::Shape    shape;                      //�������������� ����
  media::physics::Material material;                   //��������
  size_t                   flags;                      //����� ���������

  Impl ()
    : mass (0.f)
    , flags (0)
    {}

  Impl (const Impl& source)
    : name                       (source.name)
    , collision_group            (source.collision_group)
    , mass                       (source.mass)
    , center_of_mass_position    (source.center_of_mass_position)
    , center_of_mass_orientation (source.center_of_mass_orientation)
    , mass_space_inertia_tensor  (source.mass_space_inertia_tensor)
    , shape                      (source.shape.Clone ())
    , material                   (source.material.Clone ())
    , flags                      (source.flags)
    {}
};

/*
   ������������ / ���������� / ������������
*/

RigidBody::RigidBody ()
  : impl (new Impl)
{
}

RigidBody::RigidBody (const RigidBody& source)
  : impl (source.impl)
{
  addref (impl);
}

RigidBody::RigidBody (Impl* in_impl)
  : impl (in_impl)
{
}

RigidBody::~RigidBody ()
{
  release (impl);
}

RigidBody& RigidBody::operator = (const RigidBody& source)
{
  RigidBody (source).Swap (*this);

  return *this;
}
    
/*
   �������� �����
*/

RigidBody RigidBody::Clone () const
{
  return RigidBody (new Impl (*impl));
}

/*
   �������������
*/

size_t RigidBody::Id () const
{
  return (size_t)impl;
}

/*
   ��� ����
*/

const char* RigidBody::Name () const
{
  return impl->name.c_str ();
}

void RigidBody::Rename (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::physics::RigidBody::Rename", "name");

  impl->name = name;
}
    
/*
   �����
*/

float RigidBody::Mass () const
{
  return impl->mass;
}

void RigidBody::SetMass (float mass)
{
  impl->mass = mass;
}

/*
   ���������� ��������
*/

const math::vec3f& RigidBody::MassSpaceInertiaTensor () const
{
  return impl->mass_space_inertia_tensor;
}

void RigidBody::SetMassSpaceInertiaTensor (const math::vec3f& tensor)
{
  impl->mass_space_inertia_tensor = tensor;
}

/*
   �������������� ����
*/

const media::physics::Shape& RigidBody::Shape () const
{
  return impl->shape;
}

void RigidBody::SetShape (const media::physics::Shape& shape)
{
  impl->shape = shape;
}

/*
   ��������
*/

const media::physics::Material& RigidBody::Material () const
{
  return impl->material;
}

void RigidBody::SetMaterial (const media::physics::Material& material)
{
  impl->material = material;
}

/*
   ����� ���������
*/

size_t RigidBody::Flags () const
{
  return impl->flags;
}

void RigidBody::SetFlags (size_t flags)
{
  impl->flags = flags;
}

/*
   ������ ��������
*/

const char* RigidBody::CollisionGroup () const
{
  return impl->collision_group.c_str ();
}

void RigidBody::SetCollisionGroup (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::physics::RigidBody::SetCollisionGroup", "name");

  impl->collision_group = name;
}

/*
   �����
*/

void RigidBody::Swap (RigidBody& source)
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

void swap (RigidBody& body1, RigidBody& body2)
{
  body1.Swap (body2);
}

}

}
