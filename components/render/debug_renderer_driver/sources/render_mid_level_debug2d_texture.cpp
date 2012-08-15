#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::debug;
using namespace render::mid_level::debug::renderer2d;

/*
    ����������� / ����������
*/

Texture::Texture (size_t width, size_t height, PixelFormat in_format)
  : RenderTarget (width, height, RenderTargetType_Color)
  , is_compressed (false)
  , format (in_format)
  , min_filter ((render::mid_level::renderer2d::TexMinFilter)0)
{
  switch (format)
  {
    case PixelFormat_RGB8:
    case PixelFormat_RGBA8:
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_LA8:
      break;
    default:
      throw xtl::make_argument_exception ("render::mid_level::debug::renderer2d::Texture::Texture", "format", format);
  }
  
  log.Printf ("Create texture %ux%u format=%s (id=%u)", width, height, get_name (format), Id ());
}

Texture::Texture (size_t width, size_t height, const char* in_format)
  : RenderTarget (width, height, RenderTargetType_Color)
  , is_compressed (true)
  , format (PixelFormat_RGBA8)
  , min_filter ((render::mid_level::renderer2d::TexMinFilter)0)
{
  if (!in_format)
    throw xtl::make_null_argument_exception ("render::mid_level::debug::renderer2d::Texture::Texture", "format");

  log.Printf ("Create texture %ux%u format=%s (id=%u)", width, height, in_format, Id ());
}

Texture::~Texture ()
{
  try
  {
    log.Printf ("Destroy texture2d (id=%u)", Id ());
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

/*
   ����������
*/

void Texture::SetMinFilter (render::mid_level::renderer2d::TexMinFilter filter)
{
  min_filter = filter;

  log.Printf ("Set min filter %d for texture %u\n", filter, Id ());
}

render::mid_level::renderer2d::TexMinFilter Texture::GetMinFilter ()
{
  return min_filter;
}

/*
    ����������� ������ �������� � ��������
*/

void Texture::CaptureImage (media::Image& image)
{
  if (is_compressed)
    throw xtl::format_operation_exception ("render::mid_level::debug::renderer2d::Texture::CaptureImage", "Can't capture compressed image (not supported)");

  media::Image (GetWidth (), GetHeight (), 1, get_image_format (format)).Swap (image);

  log.Printf ("Capture image from texture (id=%u)", Id ());  
}

/*
    ���������� ��������
*/

void Texture::Update (media::Image& image)
{
  if (image.Width () != GetWidth () || image.Height () != GetHeight () || format != get_format (image.Format ()))
    throw xtl::format_operation_exception ("render::mid_level::debug::renderer2d::Texture::Update", "Incompatible image");

  log.Printf ("Update texture (id=%u)", Id ());
}
