#include "shared.h"

using namespace render::mid_level;
using namespace render::low_level;

/*
    �������� ���������� ���� ����������
*/

struct RenderTargetImpl::Impl
{
};

/*
    ����������� / ����������
*/

RenderTargetImpl::RenderTargetImpl ()
  : impl (new Impl)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderTargetImpl::RenderTargetImpl");
}

RenderTargetImpl::~RenderTargetImpl ()
{
}

/*
    ������� ���� ����������
*/

size_t RenderTargetImpl::Width ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderTargetImpl::Width");
}

size_t RenderTargetImpl::Height ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderTargetImpl::Height");
}

/*
    ������ �����������
*/

void RenderTargetImpl::Capture (media::Image& image)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderTargetImpl::Capture");
}
