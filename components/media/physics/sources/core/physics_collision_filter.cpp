#include "shared.h"

using namespace media::physics;

/*
   ���������� ���������
*/

struct CollisionFilter::Impl : public xtl::reference_counter
{
  stl::string group1_wildcard; //����� ���� ������ ������
  stl::string group2_wildcard; //����� ���� ������ ������
  bool        collides;        //������������ �� ������

  Impl ()
    : collides (false)
    {}
};

/*
   ������������ / ���������� / ������������
*/

CollisionFilter::CollisionFilter ()
  : impl (new Impl)
{
}

CollisionFilter::CollisionFilter (const CollisionFilter& source)
  : impl (source.impl)
{
  addref (impl);
}

CollisionFilter::CollisionFilter (Impl* in_impl)
  : impl (in_impl)
{
}

CollisionFilter::~CollisionFilter ()
{
  release (impl);
}

CollisionFilter& CollisionFilter::operator = (const CollisionFilter& source)
{
  CollisionFilter (source).Swap (*this);

  return *this;
}
    
/*
   �������� �����
*/

CollisionFilter CollisionFilter::Clone () const
{
  return CollisionFilter (new Impl (*impl));
}

/*
   �������������
*/

size_t CollisionFilter::Id () const
{
  return (size_t)impl;
}

/*
   ����� ���� �����
*/

const char* CollisionFilter::Group1Wildcard () const
{
  return impl->group1_wildcard.c_str ();
}

const char* CollisionFilter::Group2Wildcard () const
{
  return impl->group2_wildcard.c_str ();
}

void CollisionFilter::SetGroup1Wildcard (const char* wildcard)
{
  if (!wildcard)
    throw xtl::make_null_argument_exception ("media::physics::CollisionFilter::SetGroup1Wildcard", "wildcard");

  impl->group1_wildcard = wildcard;
}

void CollisionFilter::SetGroup2Wildcard (const char* wildcard)
{
  if (!wildcard)
    throw xtl::make_null_argument_exception ("media::physics::CollisionFilter::SetGroup2Wildcard", "wildcard");

  impl->group2_wildcard = wildcard;
}

/*
   ��������� ����������, ������������ �� ���� �����
*/

bool CollisionFilter::IsCollides () const
{
  return impl->collides;
}

void CollisionFilter::SetCollides (bool collides)
{
  impl->collides = collides;
}

/*
   �����
*/

void CollisionFilter::Swap (CollisionFilter& source)
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

void swap (CollisionFilter& filter1, CollisionFilter& filter2)
{
  filter1.Swap (filter2);
}

}

}
