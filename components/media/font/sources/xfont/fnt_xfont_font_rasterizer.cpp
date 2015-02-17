#include "shared.h"

using namespace media;
using namespace media::xfont;

namespace
{

static const char* LOG_NAME = "media.xfont.XFontFontRasterizer";

//��������� ��������� ������ ������� ������
size_t get_next_higher_power_of_two (size_t k)
{
  if (!k)
    return 1;

  if (!(k & (k-1)))
    return k;

  k--;

  for (size_t i=1; i < sizeof (size_t) * 8; i *= 2)
          k |= k >> i;

  return k + 1;
}

//�������� ������� �� ������� ������
bool is_power_of_two (size_t size)
{
  return ((size - 1) & size) == 0;
}

}

namespace media
{

namespace xfont
{

/*
   ������������ ������
*/

struct XFontFontRasterizer::Impl
{
  RasterizedFontCreationParams params;      //��������� ������������
  RasterizedFontParamsPtr      font_params; //������ � ������

  ///�����������
  Impl (const RasterizedFontCreationParams& in_params, const RasterizedFontParamsPtr& in_font_params)
    : font_params (in_font_params)
  {
    memcpy (&params, &in_params, sizeof (params));
  }
};

}

}

/*
   ����������� / ����������
*/

XFontFontRasterizer::XFontFontRasterizer (const RasterizedFontCreationParams& params, const RasterizedFontParamsPtr& font_params)
  : impl (new Impl (params, font_params))
  {}

XFontFontRasterizer::~XFontFontRasterizer ()
{
  delete impl;
}

/*
   ��������� ���������� � �������������� ��������
*/

const RasterizedGlyphInfo* XFontFontRasterizer::RasterizedGlyphs ()
{
  return impl->font_params->rasterized_glyphs.data ();
}

/*
   ���������� ��������
*/

unsigned int XFontFontRasterizer::ImagesCount ()
{
  return impl->font_params->images_count;
}

void XFontFontRasterizer::BuildImage (unsigned int image_index, media::Image& out_image)
{
  try
  {
    out_image.Load (common::format (impl->font_params->image_name_format.c_str (), image_index).c_str ());

    unsigned int image_width  = out_image.Width (),
                 image_height = out_image.Height ();

    if (impl->params.pot)
    {
      image_width  = (unsigned int)get_next_higher_power_of_two (image_width);
      image_height = (unsigned int)get_next_higher_power_of_two (image_height);
    }

    unsigned int max_side = stl::max (image_width, image_height);

    if (max_side > impl->params.max_image_size)
      throw xtl::format_operation_exception ("", "Can't build image, size %lu is larger than max size %lu", max_side, impl->params.max_image_size);

    if (image_width != out_image.Width () || image_height != out_image.Height ())
    {
      common::Log (LOG_NAME).Printf ("Rasterizing params requires image enlarging to pot");

      Image pot_image (image_width, image_height, 1, out_image.Format ());

      pot_image.PutImage (0, 0, 0, out_image.Width (), out_image.Height (), 1, out_image.Format (), out_image.Bitmap ());

      out_image.Swap (pot_image);
    }

    media::PixelFormat original_format = out_image.Format ();

    if (original_format != impl->params.image_format && impl->params.image_format != media::PixelFormat_Default)
    {
      common::Log (LOG_NAME).Printf ("Requested pixel format '%s' requires image convertation from '%s'", get_format_name (impl->params.image_format), get_format_name (out_image.Format ()));
      out_image.Convert (impl->params.image_format);

      if (original_format == PixelFormat_L8 && impl->params.image_format == PixelFormat_RGBA8) //special case which requires additional processing
      {
        unsigned char* current_pixel = (unsigned char*)out_image.Bitmap ();

        for (unsigned int i = 0, pixels_count = image_width * image_height; i < pixels_count; current_pixel += 4, i++)
        {
          if (*current_pixel)
          {
            current_pixel [3] = *current_pixel;
            current_pixel [0] = 255;
            current_pixel [1] = 255;
            current_pixel [2] = 255;
          }
          else
            current_pixel [3] = 0;
        }
      }
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::xfont::XFontFontRasterizer::BuildImage");
    throw;
  }
}
