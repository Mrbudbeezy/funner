#include "shared.h"

using namespace render::mid_level;
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
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::EntityImpl");
}

EntityImpl::~EntityImpl ()
{
}

/*
    �������� ����������
*/

void EntityImpl::SetProperties (const common::PropertyMap& properties)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::SetProperties");
}

const common::PropertyMap& EntityImpl::Properties ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::Properties");
}

/*
    ������� ��������������
*/

void EntityImpl::SetTransformation (const math::mat4f&)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::SetTransformation");
}

const math::vec4f& EntityImpl::Transformation ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::Transformation");
}

/*
    ������ � ������� (��� ���������)
      �������������� ���������� �� ������� EntityImpl::Transformation � ������ ���� ��� �� ��������    
*/

void EntityImpl::SetJointsCount (size_t count)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::SetJointsCount");
}

size_t EntityImpl::JointsCount ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::JointsCount");
}

void EntityImpl::SetJointTransformation (size_t joint_index, const math::mat4f&)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::SetJointTransformation");
}

const math::mat4f& EntityImpl::JointTransformation (size_t joint_index)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::JointTransformation");
}

/*
    ������ � �������� �����������
*/

size_t EntityImpl::LodsCount ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::LodsCount");
}

/*
    ������ � ����������
*/

const PrimitivePtr& EntityImpl::Primitive (size_t level_of_detail)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::Primitive");
}

void EntityImpl::ResetPrimitive (size_t level_of_detail)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::ResetPrimitive");
}

void EntityImpl::SetPrimitive (const PrimitivePtr&, size_t level_of_detail)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::SetPrimitive");
}

bool EntityImpl::HasPrimitive (size_t level_of_detail)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::HasPrimitive");
}

void EntityImpl::ResetAllPrimitives ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::ResetAllPrimitives");
}
