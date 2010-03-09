#include "shared.h"

using namespace render::mid_level;
using namespace render::low_level;

/*
    �������� ���������� ���������� ��������
*/

struct ResourceLibraryImpl::Impl
{
};

/*
    ����������� / ����������
*/

ResourceLibraryImpl::ResourceLibraryImpl ()
  : impl (new Impl)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::ResourceLibraryImpl::ResourceLibraryImpl");
}

ResourceLibraryImpl::~ResourceLibraryImpl ()
{
}

/*
    ������������� ����������
*/

void ResourceLibraryImpl::Attach (const ResourceLibraryPtr& library)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::ResourceLibraryImpl::Attach");
}

void ResourceLibraryImpl::Detach (const ResourceLibraryPtr& library)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::ResourceLibraryImpl::Detach");
}

void ResourceLibraryImpl::DetachAllLibraries ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::ResourceLibraryImpl::DetachAllLibraries");
}
