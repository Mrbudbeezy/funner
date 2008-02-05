#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   ����������� / ����������
*/

Texture3D::Texture3D  (const ContextManager& manager, const TextureDesc& tex_desc)
  : Texture (manager, tex_desc, GL_TEXTURE_3D_EXT)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Texture3D::Texture3D";

    //����� �������� ��������� � ������� ��������

  MakeContextCurrent ();
  
  Bind ();
  
    //�������� ������������ ������� ����������� ��������
    
  switch (desc.format)
  {
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_LA8:
    case PixelFormat_RGB8:
    case PixelFormat_RGBA8:
      break;
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
      RaiseNotSupported (METHOD_NAME, "3D compressed textures not supported (desc.format=%s)", get_name (desc.format));
      return;
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8:
    case PixelFormat_S8:    
      RaiseNotSupported (METHOD_NAME, "3D depth-stencil textures not supported (desc.format=%s)", get_name (desc.format));
      return;
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.format", desc.format);
      return;
  }
  
    //�������������� ������� ��������
    
  GLenum gl_internal_format = opengl::gl_internal_format (desc.format),
         gl_format          = opengl::gl_format (desc.format),
         gl_type            = opengl::gl_type (desc.format);  

  if (glTexImage3D)
  {
    glTexImage3D (GL_PROXY_TEXTURE_3D_EXT, 1, gl_internal_format, tex_desc.width, tex_desc.height, tex_desc.layers, 0, gl_format, gl_type, 0);
  }
  else
  {
    glTexImage3DEXT (GL_PROXY_TEXTURE_3D_EXT, 1, gl_internal_format, tex_desc.width, tex_desc.height, tex_desc.layers, 0, gl_format, gl_type, 0);
  }

  GLint width = 0;

  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D_EXT, 1, GL_TEXTURE_WIDTH, &width);

  if (!width)
    RaiseNotSupported (METHOD_NAME, "Can't create 3d texture %ux%ux%u@%s (proxy texture fail)", 
                       tex_desc.width, tex_desc.height, tex_desc.layers, get_name (tex_desc.format));

    //�������� mip-�������

  size_t depth = desc.layers;  

  for (size_t mip_level=0; mip_level<mips_count; mip_level++)
  {
    MipLevelDesc level_desc;

    GetMipLevelDesc (mip_level, level_desc);        

    if (glTexImage3D)
    {
      glTexImage3D (GL_TEXTURE_3D_EXT, mip_level, gl_internal_format, level_desc.width, level_desc.height, depth, 0, gl_format, gl_type, 0);
    }
    else
    {
      glTexImage3DEXT (GL_TEXTURE_3D_EXT, mip_level, gl_internal_format, level_desc.width, level_desc.height, depth, 0, gl_format, gl_type, 0);
    }

    if (depth > 1) depth /= 2;
  }
  
    //��������� �������������� ��������� !!!!������� � Texture::Texture!!!!
    
  TextureExtensions ext (GetContextManager ());//?????

  if (desc.generate_mips_enable && ext.has_sgis_generate_mipmap)
    glTexParameteri (GL_TEXTURE_3D_EXT, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

    //�������� ������

  CheckErrors (METHOD_NAME);
}

/*
    ��������� ������
*/

void Texture3D::SetUncompressedData
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
  if (glTexSubImage3D) glTexSubImage3D    (GL_TEXTURE_3D, mip_level, x, y, layer, width, height, 1, format, type, buffer);
  else                 glTexSubImage3DEXT (GL_TEXTURE_3D_EXT, mip_level, x, y, layer, width, height, 1, format, type, buffer);
}

void Texture3D::SetCompressedData (size_t, size_t, size_t, size_t, size_t, size_t, GLenum, size_t, const void*)
{
  RaiseNotSupported ("render::low_level::opengl::Texture3D::SetCompressedData", "Compression for 3D textures not supported");
}
