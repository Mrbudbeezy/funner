#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   ����������� / ����������
*/

TextureCubemap::TextureCubemap  (const ContextManager& manager, const TextureDesc& tex_desc, const TextureData* data)
  : Texture (manager, tex_desc, GL_TEXTURE_CUBE_MAP, get_mips_count (tex_desc.width, tex_desc.height))
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureCubemap::TextureCubemap";

    //��������� �������� � �������� OpenGL

  Bind ();
  
    //�������� ������������ ����������� ��������

  if (is_depth_stencil (tex_desc.format))
    throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create depth cubemap texture. Reason: depth texture may be only 1D or 2D");

  if (tex_desc.layers != 6)
    throw xtl::make_argument_exception (METHOD_NAME, "desc.layers", (size_t)tex_desc.layers, "Cubemap texture must has desc.layers = 6");

  if (tex_desc.width != tex_desc.height)
    throw xtl::format_exception<xtl::bad_argument> (METHOD_NAME, "Cubemap texture sizes must be equal (desc.width=%u, desc.height=%u)", tex_desc.width, tex_desc.height);

     //�������������� ������� ��������

  GLenum gl_uncompressed_format = get_uncompressed_gl_format (tex_desc.format),
         gl_uncompressed_type   = get_uncompressed_gl_type (tex_desc.format),
         gl_internal_format;
         
  bool is_dxt = false;
         
  switch (tex_desc.format)
  {
    case PixelFormat_RGB_PVRTC2:
    case PixelFormat_RGB_PVRTC4:
    case PixelFormat_RGBA_PVRTC2:
    case PixelFormat_RGBA_PVRTC4:
      if (!GetCaps ().has_img_texture_compression_pvrtc)
        throw xtl::format_not_supported_exception (METHOD_NAME, "PVRTC texture compression not supported");
      break;
    case PixelFormat_ATC_RGB_AMD:
    case PixelFormat_ATC_RGBA_EXPLICIT_ALPHA_AMD:
    case PixelFormat_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
      if (!GetCaps ().has_amd_compressed_atc_texture)
        throw xtl::format_not_supported_exception (METHOD_NAME, "AMD ATC texture compression not supported");
      break;
    default:
      break;
  }

  switch (tex_desc.format)
  {
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
      gl_internal_format = GetCaps ().has_ext_texture_compression_s3tc ? get_gl_internal_format (tex_desc.format) : get_uncompressed_gl_internal_format (tex_desc.format);
      is_dxt             = true;
      break;
    default:
      gl_internal_format = get_gl_internal_format (tex_desc.format);    
      break;    
  }

#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)

    //�������� ����������� �������� ��������

  glTexImage2D (GL_PROXY_TEXTURE_CUBE_MAP, 1, gl_internal_format, tex_desc.width, tex_desc.height, 0, gl_uncompressed_format, gl_uncompressed_type, 0);
  
  GLint proxy_width = 0;

  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_CUBE_MAP, 1, GL_TEXTURE_WIDTH, &proxy_width);

//  if (!proxy_width)
//  {
//    throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create cubemap texture %ux%u@%s. Reason: proxy texure fail",
//                       tex_desc.width, tex_desc.height, get_name (tex_desc.format));
//  }
    
#endif

    //��������� ������������ ������ � ������    

  glPixelStorei (GL_UNPACK_ALIGNMENT,   1);    //������������ ������ �����

#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)

  glPixelStorei (GL_UNPACK_SKIP_ROWS,   0);    //���������� ������������ �����
  glPixelStorei (GL_UNPACK_SKIP_PIXELS, 0);    //���������� ������������ ��������

#endif

    //�������� mip-�������
    
  TextureDataSelector data_selector (tex_desc, data);
  bool                is_compressed_data = is_compressed (tex_desc.format);

  PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D_fn = GetCaps ().glCompressedTexImage2D_fn;
  
  xtl::uninitialized_storage<char> unpacked_buffer;      
  
  if (is_compressed_data && data && !GetCaps ().has_ext_texture_compression_s3tc && is_dxt)
  {
    unpacked_buffer.resize (get_uncompressed_image_size (tex_desc.width, tex_desc.height, tex_desc.format));  
  }

  unsigned int mips_count = GetMipsCount ();

  for (unsigned int i=0; i<mips_count; i++)
  {
    MipLevelDesc level_desc;

    GetMipLevelDesc (i, level_desc);

    for (unsigned int j=0; j<6; j++)
    {
      TextureLevelData level_data;

      if (data_selector.GetLevelData (level_desc.width, level_desc.height, 1, level_data) && glCompressedTexImage2D_fn && is_compressed_data)
      {
        glCompressedTexImage2D_fn (GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, i, gl_internal_format, level_desc.width, level_desc.height, 0, level_data.size, level_data.data);        
      }
      else
      {
#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)

        glPixelStorei (GL_UNPACK_ROW_LENGTH, level_desc.width); //����� ������ � �������� (��� �������� mip-������)
        
#endif

        if (is_compressed_data && level_data.data && is_dxt)
        {
          unpack_dxt (tex_desc.format, level_desc.width, level_desc.height, level_data.data, unpacked_buffer.data ());

          glTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, i, gl_internal_format, level_desc.width, level_desc.height, 0, gl_uncompressed_format, gl_uncompressed_type, unpacked_buffer.data ());
        }
        else
        {
          glTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, i, gl_internal_format, level_desc.width, level_desc.height, 0, gl_uncompressed_format, gl_uncompressed_type, level_data.data);
        }      
      }            

      data_selector.Next ();
    }
  }

   //��������� ��������� ����������� ������� �������� �������� (������� � ���������� ������� �������)

#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)

  glGetTexLevelParameteriv (GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_TEXTURE_INTERNAL_FORMAT, (GLint*)&gl_internal_format);
      
#endif

  try
  {
    SetFormat (get_pixel_format (gl_internal_format));
  }
  catch (xtl::exception& e)
  {
    e.touch (METHOD_NAME);

    throw;
  }

    //�������� ������

  CheckErrors (METHOD_NAME);
}

/*
    ��������� ����������� ���� ��������
*/

void TextureCubemap::GetLayerDesc (unsigned int layer, LayerDesc& desc)
{
  if (layer > 6)
    throw xtl::make_range_exception ("render::low_level::opengl::TextureCubemap::GetLayerDesc", "layer", layer, 6);

  desc.target    = GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer;
  desc.new_index = 0;
}

/*
    ��������� ������
*/

void TextureCubemap::SetUncompressedData
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
  glTexSubImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer, mip_level, x, y, width, height, format, type, buffer);  
}

void TextureCubemap::SetCompressedData
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
  GetCaps ().glCompressedTexSubImage2D_fn (GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer, mip_level, x, y, width, height, format, buffer_size, buffer);
}
