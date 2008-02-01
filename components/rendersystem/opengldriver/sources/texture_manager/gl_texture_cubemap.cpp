#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   ����������� / ����������
*/

TextureCubemap::TextureCubemap  (const ContextManager& manager, const TextureDesc& tex_desc)
  : Texture (manager, tex_desc, GL_TEXTURE_CUBE_MAP_ARB)
{
  TextureExtensions ext (GetContextManager ());

  static const char* METHOD_NAME = "render::low_level::opengl::TextureCubemap::TextureCubemap";
  int width = 0;

  if (is_depth_format (tex_desc.format))
    RaiseNotSupported (METHOD_NAME, "Can't create depth cubemap texture. Reason: depth texture may be only 1d or 2d");

  if (tex_desc.layers != 6)
    RaiseOutOfRange (METHOD_NAME, "tex_desc.layers", (int)tex_desc.layers, 6, 6);
  
  if (tex_desc.width != tex_desc.height)
    Raise <Exception> (METHOD_NAME, "Cubemap texture width and height must be equal.");
  
  if ((tex_desc.width & 3) && is_compressed_format (tex_desc.format))
    RaiseInvalidArgument (METHOD_NAME, "tex_desc.width", tex_desc.width,
                          "Texture width for compressed image must be a multiple 4");
  
  if ((tex_desc.height & 3) && is_compressed_format (tex_desc.format))
    RaiseInvalidArgument (METHOD_NAME, "tex_desc.height", tex_desc.height,
                          "Texture height for compressed image must be a multiple 4");
  
    //MakeContextCurrent???

  Bind ();

    //�������� mip-�������

  for (size_t i=0; i<mips_count; i++)
  {        
    MipLevelDesc level_desc;

    GetMipLevelDesc (i, level_desc);

    for (size_t j=0; j<6; j++)
    {      
      if (is_compressed_format (tex_desc.format) && ext.has_ext_texture_compression_s3tc)
      {
        size_t tex_size = level_desc.width * level_desc.height / 16 * compressed_quad_size (tex_desc.format);

        if (ext.has_arb_texture_compression) glCompressedTexImage2DARB (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + j, i, gl_internal_format (tex_desc.format),
                                                                        level_desc.width, level_desc.height, 0, tex_size, 0);
        else                                 glCompressedTexImage2D    (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + j, i, gl_internal_format (tex_desc.format),
                                                                        level_desc.width, level_desc.height, 0, tex_size, 0);
      }
      else
      {
        glTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + j, i, unpack_internal_format (tex_desc.format), level_desc.width,
                      level_desc.height, 0, unpack_format (tex_desc.format), unpack_type (tex_desc.format), 0);
      }
    }
  }

  if (tex_desc.generate_mips_enable && ext.has_sgis_generate_mipmap)
      glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_GENERATE_MIPMAP_SGIS, true);

  CheckErrors (METHOD_NAME);
}

/*
    ��������� ����������� ���� ��������
*/

void TextureCubemap::GetLayerDesc (size_t layer, LayerDesc& desc)
{
  if (layer > 6)
    RaiseOutOfRange ("render::low_level::opengl::TextureCubemap::GetLayerDesc", "layer", layer, 6);

  desc.target    = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + layer;
  desc.new_index = 0;
}

/*
    ��������� ������
*/

void TextureCubemap::SetUncompressedData
 (size_t      layer,
  size_t      mip_level,
  size_t      x,
  size_t      y,
  size_t      width,
  size_t      height,
  GLenum      format,
  GLenum      type,
  const void* buffer)
{
  glTexSubImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + layer, mip_level, x, y, width, height, format, type, buffer);  
}

void TextureCubemap::SetCompressedData
 (size_t      layer,
  size_t      mip_level,
  size_t      x,
  size_t      y,
  size_t      width,
  size_t      height,
  GLenum      format,
  size_t      buffer_size,
  const void* buffer)
{
  GLenum layer_target = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + layer;

  if (glCompressedTexSubImage2D)
  {
    glCompressedTexSubImage2D (layer_target, mip_level, x, y, width, height, format, buffer_size, buffer);
  }
  else
  {
    glCompressedTexSubImage2DARB (layer_target, mip_level, x, y, width, height, format, buffer_size, buffer);
  }
}
