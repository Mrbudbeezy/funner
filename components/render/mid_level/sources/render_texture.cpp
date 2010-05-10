#include "shared.h"

using namespace render::mid_level;
using namespace render::low_level;

/*
    �������� ���������� ��������
*/

struct TextureImpl::Impl
{
};

/*
    ����������� / ���������� / ������������
*/

TextureImpl::TextureImpl 
 (render::mid_level::TextureDimension dimension,
  size_t                              width,
  size_t                              height,
  size_t                              depth,
  render::mid_level::PixelFormat      format)
  : impl (new Impl)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::TextureImpl::TextureImpl");
}

TextureImpl::~TextureImpl ()
{
}

/*
    ������������� �������� � ��������� ����������
*/

const char* TextureImpl::Id ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::TextureImpl::Id");
}

void TextureImpl::SetId (const char* name)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::TextureImpl::SetId");
}

/*
    �����������
*/

render::mid_level::TextureDimension TextureImpl::Dimension ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::TextureImpl::Dimension");
}

/*
    ������ � �������
*/

render::mid_level::PixelFormat TextureImpl::Format ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::TextureImpl::Format");
}

size_t TextureImpl::Width ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::TextureImpl::Width");
}

size_t TextureImpl::Height ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::TextureImpl::Height");
}

size_t TextureImpl::Depth ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::TextureImpl::Depth");
}

/*
    ��������� ���� ����������
*/

RenderTargetPtr TextureImpl::RenderTarget (size_t layer, size_t mip_level)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::TextureImpl::RenderTarget");
}

/*
    ���������� ������
*/

void TextureImpl::Update (const media::Image& image)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::TextureImpl::Update");
}
