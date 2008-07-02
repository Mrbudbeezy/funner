#include "shared.h"

using namespace media::rfx;

/*
    �������� ���������� SpriteMaterial
*/

struct SpriteMaterial::Impl
{
  stl::string     image;        //������� �����������
  SpriteBlendMode blend_mode;   //����� ����������
  bool            tiling_state; //������� �� �������
  size_t          tile_width;   //������ �����
  size_t          tile_height;  //������ �����
  
  Impl () : blend_mode (SpriteBlendMode_None), tiling_state (false), tile_width (0), tile_height (0) {}
};

/*
    ������������ / ����������
*/

SpriteMaterial::SpriteMaterial ()
  : impl (new Impl)
{
}

SpriteMaterial::SpriteMaterial (const SpriteMaterial& mtl)
  : impl (new Impl (*mtl.impl))
{
}

SpriteMaterial::~SpriteMaterial ()
{
}

/*
    �������� ���������
*/

SpriteMaterial::Pointer SpriteMaterial::Create ()
{
  return Pointer (new SpriteMaterial, false);
}

/*
    ��� �������� �����������
*/

void SpriteMaterial::SetImage (const char* image_name)
{
  if (!image_name)
    throw xtl::make_null_argument_exception ("media::rfx::SpriteMaterial::SetImage", "image_name");
    
  impl->image = image_name;
}

const char* SpriteMaterial::Image () const
{
  return impl->image.c_str ();
}

/*
    ����� ���������� ������
*/

void SpriteMaterial::SetBlendMode (SpriteBlendMode mode)
{
  switch (mode)
  {
    case SpriteBlendMode_None:
    case SpriteBlendMode_Translucent:
    case SpriteBlendMode_Mask:
    case SpriteBlendMode_Additive:
      break;
    default:
      throw xtl::make_argument_exception ("media::rfx::SetBlendMode", "mode", mode);
  }
  
  impl->blend_mode = mode;
}

SpriteBlendMode SpriteMaterial::BlendMode () const
{
  return impl->blend_mode;
}

/*
    ��������� / ���������� ��������
*/

void SpriteMaterial::SetTiling (bool state)
{
  impl->tiling_state = state;
}

bool SpriteMaterial::IsTiled () const
{
  return impl->tiling_state;
}

/*
    ��������� ��������
*/

void SpriteMaterial::SetTileSize (size_t width, size_t height)
{
  impl->tile_width  = width;
  impl->tile_height = height;
}

size_t SpriteMaterial::TileWidth () const
{
  return impl->tile_width;
}

size_t SpriteMaterial::TileHeight  () const
{
  return impl->tile_height;
}

/*
    ������������ ���������������
*/

void SpriteMaterial::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Material::AcceptCore (visitor);
}

Material* SpriteMaterial::CloneCore () const
{
  return new SpriteMaterial (*this);
}

namespace media
{

namespace rfx
{

/*
    ��������� ���������� �����
*/

const char* get_name (SpriteBlendMode mode)
{
  switch (mode)
  {
    case SpriteBlendMode_None:        return "none";
    case SpriteBlendMode_Translucent: return "translucent";
    case SpriteBlendMode_Mask:        return "mask";
    case SpriteBlendMode_Additive:    return "additive";
    default:
      throw xtl::make_argument_exception ("media::rfx::get_name(SpriteBlendMode)", "mode", mode);
  }
}

}

}
