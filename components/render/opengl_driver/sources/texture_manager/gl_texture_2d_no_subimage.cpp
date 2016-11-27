#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)

#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   Конструктор / деструктор
*/

Texture2DNoSubimage::Texture2DNoSubimage  (const ContextManager& manager, const TextureDesc& tex_desc, const TextureData* data)
  : Texture2D (manager, tex_desc, data, true)
{
  gl_internal_format = GetCaps ().has_ext_texture_compression_s3tc ? get_gl_internal_format (GetFormat ()) : get_uncompressed_gl_internal_format (GetFormat ());
}

/*
    Установка данных
*/

void Texture2DNoSubimage::SetUncompressedData
 (unsigned int layer,
  unsigned int mip_level,
  unsigned int x,
  unsigned int y,
  unsigned int width,
  unsigned int height,
  GLenum       format,
  GLenum       type,
  const void*  buffer)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Texture2DNoSubimage::SetUncompressedData";
  
  MipLevelDesc level_desc;

  GetMipLevelDesc (mip_level, level_desc);  

  if (x || y || width != level_desc.width || height != level_desc.height)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Could not copy sub-image (x=%u, y=%u, width=%u, height=%u) "
      "in texture2D (mip_level=%u, width=%u, height=%u). This feature unsupported in texture emulation mode", x, y, width, height,
      mip_level, level_desc.width, level_desc.height);

  glTexImage2D (GL_TEXTURE_2D, mip_level, gl_internal_format, width, height, 0, format, type, buffer);
}

void Texture2DNoSubimage::SetCompressedData
 (unsigned int layer,
  unsigned int mip_level,
  unsigned int x,
  unsigned int y,
  unsigned int width,
  unsigned int height,
  GLenum       format,
  unsigned int buffer_size,
  const void*  buffer)
{
  throw xtl::format_not_supported_exception ("render::low_level::opengl::Texture2DNoSubimage::SetCompressedData",
    "Compression not supported in texture emulation mode");
}

#endif
