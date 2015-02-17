#include "shared.h"

using namespace media;

/*
   ���������� ���������������� ������
*/

namespace media
{

struct RasterizedFont::Impl : public xtl::reference_counter
{
  xtl::com_ptr<IFontRasterizer> rasterizer;

  Impl (IFontRasterizer* in_rasterizer)
    : rasterizer (in_rasterizer)
    {}
};

}

/*
   �����������/����������
*/

RasterizedFont::RasterizedFont (IFontRasterizer* rasterizer)
  : impl (new Impl (rasterizer))
{
}

RasterizedFont::RasterizedFont (const RasterizedFont& source)
  : impl (source.impl)
{
  addref (impl);
}

RasterizedFont::~RasterizedFont ()
{
  release (impl);
}

/*
   ������������
*/

RasterizedFont& RasterizedFont::operator = (const RasterizedFont& source)
{
  RasterizedFont (source).Swap (*this);

  return *this;
}

/*
   ��������������� �����
*/

const RasterizedGlyphInfo* RasterizedFont::RasterizedGlyphs () const
{
  return impl->rasterizer->RasterizedGlyphs ();
}

/*
   ���������� ��������
*/

unsigned int RasterizedFont::ImagesCount () const
{
  return impl->rasterizer->ImagesCount ();
}

/*
   ���������� ��������
*/

void RasterizedFont::BuildImage (unsigned int image_index, media::Image& out_image) const
{
  if (image_index >= ImagesCount ())
    throw xtl::make_range_exception ("media::RasterizedFont::BuildImage", "image_index", (size_t)image_index, (size_t)0, (size_t)ImagesCount ());

  impl->rasterizer->BuildImage (image_index, out_image);
}

/*
   �����
*/

void RasterizedFont::Swap (RasterizedFont& source)
{
  stl::swap (impl, source.impl);
}

namespace media
{

/*
   �����
*/

void swap (RasterizedFont& font1, RasterizedFont& font2)
{
  font1.Swap (font2);
}

}
