#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::debug;
using namespace render::mid_level::debug::renderer2d;
using namespace media;

/*
    ����������� / ����������
*/

Texture::Texture (size_t width, size_t height, media::PixelFormat in_format)
  : RenderTarget (width, height, RenderTargetType_Color), format (in_format)
{
  switch (format)
  {
    case PixelFormat_RGB8:
    case PixelFormat_RGB16:
    case PixelFormat_BGR8:
    case PixelFormat_RGBA8:
    case PixelFormat_RGBA16:
    case PixelFormat_BGRA8:
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_LA8:
      break;
    default:
      throw xtl::make_argument_exception ("render::mid_level::debug::renderer2d::Texture::Texture", "format", format);
  }
  
  log.Printf ("Create texture %ux%u format=%d (id=%u)", width, height, format, Id ());
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
    ����������� ������ �������� � ��������
*/

void Texture::CaptureImage (media::Image& image)
{
  media::Image (GetWidth (), GetHeight (), 1, GetFormat ()).Swap (image);

  log.Printf ("Capture image from texture (id=%u)", Id ());  
}