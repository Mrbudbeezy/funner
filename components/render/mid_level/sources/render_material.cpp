#include "shared.h"

using namespace render::mid_level;
using namespace render::low_level;

/*
    �������� ���������� ���������
*/

struct MaterialImpl::Impl
{
};

/*
    ����������� / ����������
*/

MaterialImpl::MaterialImpl ()
  : impl (new Impl)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::MaterialImpl::MaterialImpl");
}

MaterialImpl::~MaterialImpl ()
{
}

/*
    ������������� ���������
*/

const char* MaterialImpl::Id ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::MaterialImpl::Id");
}

void MaterialImpl::SetId (const char* id)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::MaterialImpl::SetId");
}

/*
    ���������� ���������
*/

void MaterialImpl::Update (const media::rfx::Material& material)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::MaterialImpl::Update");
}
