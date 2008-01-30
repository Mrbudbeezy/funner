#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   ����������� / ����������
*/

Texture2D::Texture2D  (const ContextManager& manager, const TextureDesc& tex_desc)
  : Texture (manager, tex_desc, GL_TEXTURE_2D)
{
  TextureExtensions ext (GetContextManager ());

  Bind ();

  size_t width = tex_desc.width; size_t height = tex_desc.height;
  
  for (size_t i = 0; i < mips_count; i++)
  {
    GLenum gl_internal_format, gl_format, gl_type;     
    
    if (ext.has_ext_texture_compression_s3tc)
    {
      gl_internal_format = opengl::gl_internal_format (tex_desc.format);        
      gl_format          = opengl::unpack_format      (tex_desc.format);
      gl_type            = opengl::unpack_type        (tex_desc.format);      
    }
    else
    {
      gl_internal_format = opengl::unpack_internal_format (tex_desc.format);        
      gl_format          = opengl::unpack_format          (tex_desc.format);
      gl_type            = opengl::unpack_type            (tex_desc.format);
    }

    glTexImage2D (GL_TEXTURE_2D, i, gl_internal_format, width, height, 0, gl_format, gl_type, NULL);

    if (width > 1)  width  /= 2; //get_next_mip_size!!!
    if (height > 1) height /= 2;
  }

  if (tex_desc.generate_mips_enable)
    if (ext.has_sgis_generate_mipmap)
      glTexParameteri (GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, true);

  CheckErrors ("render::low_level::opengl::Texture2D::Texture2D");
}

/*
   ������ � �������
*/

namespace
{

void SetTexData2D (size_t mip_level, size_t x, size_t y, size_t, size_t width, size_t height, GLenum format, GLenum type, const void* data)
{
  glTexSubImage2D (GL_TEXTURE_2D, mip_level, x, y, width, height, format, type, data);
}

}

void Texture2D::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer)
{
  Texture::SetData (layer, mip_level, x, y, width, height, source_format, buffer);
  
  if (mip_level > mips_count)
    RaiseOutOfRange ("render::low_level::opengl::Texture2D::SetData", "mip_level", mip_level, (size_t)0, mips_count);
  if (((x + width) > (desc.width >> mip_level)) && ((x + width) != 1))
    RaiseOutOfRange ("render::low_level::opengl::Texture2D::SetData", "x + width", x + width, (size_t)0, desc.width >> mip_level);
  if (((y + height) > (desc.height >> mip_level)) && ((y + height) != 1))
    RaiseOutOfRange ("render::low_level::opengl::Texture2D::SetData", "y + height", y + height, (size_t)0, desc.height >> mip_level);
  if (!width || !height)
    return;
  if (is_compressed_format (desc.format))
  {
    if (desc.generate_mips_enable)
      RaiseInvalidOperation ("render::low_level::opengl::Texture2D::SetData", "Generate mipmaps not compatible with compressed textures.");
    if (x & 3)
      RaiseInvalidArgument ("render::low_level::opengl::Texture2D::SetData", "x", x, "x must be a multiple of 4.");
    if (y & 3)
      RaiseInvalidArgument ("render::low_level::opengl::Texture2D::SetData", "y", y, "y must be a multiple of 4.");
    if (width & 3)
      RaiseInvalidArgument ("render::low_level::opengl::Texture2D::SetData", "width", width, "width must be a multiple of 4.");
    if (height & 3)
      RaiseInvalidArgument ("render::low_level::opengl::Texture2D::SetData", "height", height, "height must be a multiple of 4.");
  }
  if (is_compressed_format (source_format))
    if (source_format != desc.format)
      RaiseInvalidArgument ("render::low_level::opengl::Texture2D::SetData", "source_format");

  TextureExtensions ext (GetContextManager ());

  MakeContextCurrent ();
  Bind ();

  if (is_compressed_format (source_format))
  {
    if (ext.has_ext_texture_compression_s3tc)
    {
      printf ("in %04x format=%04x mip_level=%u\n", glGetError (), gl_format (source_format), mip_level);
      glCompressedTexSubImage2D (GL_TEXTURE_2D, mip_level, x, y, width, height, gl_format (source_format),
                                 ((width * height) / 16) * compressed_quad_size (source_format), buffer);
      printf ("out %04x\n", glGetError ());
    }
    else
    {
      xtl::uninitialized_storage <char> unpacked_buffer (width * height * unpack_texel_size (source_format));

      unpack_dxt (source_format, width, height, buffer, unpacked_buffer.data ());
      glTexSubImage2D (GL_TEXTURE_2D, mip_level, x, y, width, height, unpack_format (source_format), unpack_type (source_format), unpacked_buffer.data ());
    }
  }
  else
  {
    if (mip_level && ext.has_sgis_generate_mipmap) // �������� ��� ������������� ��� ��������� mip_level != 0!!!
      glTexParameteri (GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, false); 

    glTexSubImage2D (GL_TEXTURE_2D, mip_level, x, y, width, height, gl_format (source_format), gl_type (source_format), buffer);
    
    if (mip_level && ext.has_sgis_generate_mipmap)
      glTexParameteri (GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, true);

    if (desc.generate_mips_enable && !mip_level && !ext.has_sgis_generate_mipmap)
    {
      generate_mips (x, y, 0, width, height, source_format, buffer, &SetTexData2D);
    }
  }

  CheckErrors ("render::low_level::opengl::Texture2D::SetData");
}
                                