#include "shared.h"

using namespace render;
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
  throw xtl::make_not_implemented_exception ("render::MaterialImpl::MaterialImpl");
}

MaterialImpl::~MaterialImpl ()
{
}

/*
    ������������� ���������
*/

const char* MaterialImpl::Id ()
{
  throw xtl::make_not_implemented_exception ("render::MaterialImpl::Id");
}

void MaterialImpl::SetId (const char* id)
{
  throw xtl::make_not_implemented_exception ("render::MaterialImpl::SetId");
}

/*
    ���������� ���������
*/

void MaterialImpl::Update (const media::rfx::Material& material)
{
  throw xtl::make_not_implemented_exception ("render::MaterialImpl::Update");
}
