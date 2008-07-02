#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::debug;
using namespace render::mid_level::debug::renderer2d;

/*
    �����������
*/

Renderer::Renderer () 
{
  log.Printf ("Create 2D renderer (id=%u)", Id ());
}

Renderer::~Renderer ()
{
  try
  {
    log.Printf ("Destroy 2D renderer (id=%u)", Id ());
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

/*
    ��������
*/

const char* Renderer::GetDescription ()
{
  return "render::mid_level::debug::renderer2d::Renderer";
}

/*
    �������� ��������
*/

ITexture* Renderer::CreateTexture (const media::Image& image)
{
  return CreateTexture (image.Width (), image.Height (), image.Format ());
}

ITexture* Renderer::CreateTexture (size_t width, size_t height, media::PixelFormat pixel_format)
{
  return new Texture (width, height, pixel_format);
}

IPrimitive* Renderer::CreatePrimitive ()
{
  return new Primitive;
}

render::mid_level::renderer2d::IFrame* Renderer::CreateFrame ()
{
  return new Frame;
}
