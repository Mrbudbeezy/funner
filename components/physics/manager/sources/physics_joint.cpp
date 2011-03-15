#include "shared.h"

using namespace physics;

/*
   �������� ����� �������� ���������� � ����
*/

JointBind::JointBind (const RigidBody& in_body, const math::vec3f& in_anchor, const math::vec3f& in_axis)
  : body (in_body), anchor (in_anchor), axis (in_axis)
  {}

JointBind::JointBind (const RigidBody& in_body, const math::vec3f& in_axis)
  : body (in_body), axis (in_axis)
  {}

/*
   ���������� ���
*/

/*
   ����������� / ���������� / �����������
*/

JointInternal::JointInternal (JointImpl* impl)
  : Joint (impl)
  {}

Joint::Joint (JointImpl* source_impl)
  : impl (source_impl)
  {}

Joint::Joint (const Joint& source)
  : impl (source.impl)
{
  addref (impl);
}

Joint::~Joint ()
{
  release (impl);
}

Joint& Joint::operator = (const Joint& source)
{
  Joint (source).Swap (*this);

  return *this;
}

/*
   ��������� ����������� ���
*/

size_t Joint::BodiesCount () const
{
  return impl->BodiesCount ();
}

const RigidBody& Joint::Body (size_t index) const
{
  return const_cast<Joint&> (*this).Body (index);
}

RigidBody& Joint::Body (size_t index)
{
  return impl->Body (index);
}

/*
   �����
*/

void Joint::Swap (Joint& joint)
{
  stl::swap (impl, joint.impl);
}

namespace physics
{

/*
   �����
*/

void swap (Joint& joint1, Joint& joint2)
{
  joint1.Swap (joint2);
}

}
