#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::debug;

/*
    �����������
*/

Renderer2d::Renderer2d ()
{
  log.Printf ("Create 2D renderer (id=%u)", Id ());
}

Renderer2d::~Renderer2d ()
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
    �������� ��������
*/

ITexture* Renderer2d::CreateTexture (const media::Image& image)
{
  return CreateTexture (image.Width (), image.Height (), image.Format ());
}

ITexture* Renderer2d::CreateTexture (size_t width, size_t height, media::PixelFormat pixel_format)
{
  return new Texture2d (width, height, pixel_format);
}

ISpriteList* Renderer2d::CreateSpriteList (size_t sprites_count)
{
  return new SpriteList (sprites_count);
}

/*
    ���������� ����� �� ���������
*/

void Renderer2d::AddFrame (renderer2d::Frame*)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::debug::Renderer2d::AddFrame");
}

/*
    ����� ���������
*/

void Renderer2d::DrawFrames ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::debug::Renderer2d::DrawFrames");
}
