#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)

#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   ����������� / ����������
*/

Texture1D::Texture1D (const ContextManager& manager, const TextureDesc& tex_desc, const TextureData* data, bool ignore_null_data)
  : Texture (manager, tex_desc, GL_TEXTURE_1D, get_mips_count (tex_desc.width))
{
  const char* METHOD_NAME = "render::low_level::opengl::Texture1D::Texture1D";

    //��������� �������� � �������� OpenGL

  Bind ();
  
    //�������������� ������� ��������

  if (is_compressed (tex_desc.format))
    throw xtl::format_not_supported_exception (METHOD_NAME, "1D texture can't be compressed");

  GLenum gl_internal_format = get_gl_internal_format (tex_desc.format),
         gl_format          = get_gl_format (tex_desc.format),
         gl_type            = get_gl_type (tex_desc.format);  
  
    //�������� ����������� �������� ��������

/*  glTexImage1D (GL_PROXY_TEXTURE_1D, 1, gl_internal_format, tex_desc.width, 0, gl_format, gl_type, 0);

  GLint proxy_width = 0;  

  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_1D, 1, GL_TEXTURE_WIDTH, &proxy_width);

  if (!proxy_width)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create 1D texture %u@%s. Reason: proxy texure fail", tex_desc.width, get_name (tex_desc.format));*/
    
  if (data || !ignore_null_data)
  {
      //�������� ��������        

    TextureDataSelector data_selector (tex_desc, data);

    for (size_t i=0; i<GetMipsCount (); i++)
    {
      size_t level_width = tex_desc.width >> i;
      
      TextureLevelData level_data;
      
      data_selector.GetLevelData (level_width, 1, 1, level_data);

      glTexImage1D (GL_TEXTURE_1D, i, gl_internal_format, level_width, 0, gl_format, gl_type, level_data.data);

      data_selector.Next ();
    }
  }

    //�������� ������

  CheckErrors (METHOD_NAME);
}

/*
    ��������� ������
*/

void Texture1D::SetUncompressedData (size_t layer, size_t mip_level, size_t x, size_t, size_t width, size_t, GLenum format, GLenum type, const void* buffer)
{
  glTexSubImage1D (GL_TEXTURE_1D, mip_level, x, width, format, type, buffer);
}

void Texture1D::SetCompressedData (size_t, size_t, size_t, size_t, size_t, size_t, GLenum, size_t, const void*)
{
  throw xtl::format_not_supported_exception ("render::low_level::opengl::Texture1D::SetCompressedData", "Compression for 1D textures not supported");
}

#endif
