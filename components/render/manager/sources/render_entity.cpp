#include "shared.h"

using namespace render;
using namespace render::low_level;

/*
    �������� ���������� ������� ����������
*/

struct EntityImpl::Impl
{
};

/*
    ����������� / ����������
*/

EntityImpl::EntityImpl ()
  : impl (new Impl)
{
  throw xtl::make_not_implemented_exception ("render::EntityImpl::EntityImpl");
}

EntityImpl::~EntityImpl ()
{
}

/*
    �������� ����������
*/

void EntityImpl::SetProperties (const common::PropertyMap& properties)
{
  throw xtl::make_not_implemented_exception ("render::EntityImpl::SetProperties");
}

const common::PropertyMap& EntityImpl::Properties ()
{
  throw xtl::make_not_implemented_exception ("render::EntityImpl::Properties");
}

/*
    ������� ��������������
*/

void EntityImpl::SetTransformation (const math::mat4f&)
{
  throw xtl::make_not_implemented_exception ("render::EntityImpl::SetTransformation");
}

const math::vec4f& EntityImpl::Transformation ()
{
  throw xtl::make_not_implemented_exception ("render::EntityImpl::Transformation");
}

/*
    ������ � ������� (��� ���������)
      �������������� ���������� �� ������� EntityImpl::Transformation � ������ ���� ��� �� ��������    
*/

void EntityImpl::SetJointsCount (size_t count)
{
  throw xtl::make_not_implemented_exception ("render::EntityImpl::SetJointsCount");
}

size_t EntityImpl::JointsCount ()
{
  throw xtl::make_not_implemented_exception ("render::EntityImpl::JointsCount");
}

void EntityImpl::SetJointTransformation (size_t joint_index, const math::mat4f&)
{
  throw xtl::make_not_implemented_exception ("render::EntityImpl::SetJointTransformation");
}

const math::mat4f& EntityImpl::JointTransformation (size_t joint_index)
{
  throw xtl::make_not_implemented_exception ("render::EntityImpl::JointTransformation");
}

/*
    ������ � �������� �����������
*/

size_t EntityImpl::LodsCount ()
{
  throw xtl::make_not_implemented_exception ("render::EntityImpl::LodsCount");
}

/*
    ������ � ����������
*/

const PrimitivePtr& EntityImpl::Primitive (size_t level_of_detail)
{
  throw xtl::make_not_implemented_exception ("render::EntityImpl::Primitive");
}

void EntityImpl::ResetPrimitive (size_t level_of_detail)
{
  throw xtl::make_not_implemented_exception ("render::EntityImpl::ResetPrimitive");
}

void EntityImpl::SetPrimitive (const PrimitivePtr&, size_t level_of_detail)
{
  throw xtl::make_not_implemented_exception ("render::EntityImpl::SetPrimitive");
}

bool EntityImpl::HasPrimitive (size_t level_of_detail)
{
  throw xtl::make_not_implemented_exception ("render::EntityImpl::HasPrimitive");
}

void EntityImpl::ResetAllPrimitives ()
{
  throw xtl::make_not_implemented_exception ("render::EntityImpl::ResetAllPrimitives");
}
