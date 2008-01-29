#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   ����������� / ����������
*/

Texture::Texture  (const ContextManager& manager, const TextureDesc& tex_desc, GLenum in_target)
  : ContextObject (manager), target (in_target), desc (tex_desc), mips_count (0)
{
  MakeContextCurrent ();
  glGenTextures(1, &texture_id);
  CheckErrors ("render::low_level::opengl::Texture::Texture");
}

Texture::~Texture ()
{
  MakeContextCurrent ();
  glDeleteTextures (1, &texture_id);
  CheckErrors ("render::low_level::opengl::Texture::~Texture");
}

/*
   ��������� �����������
*/

void Texture::GetDesc (TextureDesc& target_desc)
{
  target_desc.dimension    = desc.dimension;
  target_desc.width        = desc.width;
  target_desc.height       = desc.height;
  target_desc.layers       = desc.layers;
  target_desc.format       = desc.format;
  target_desc.generate_mips_enable = desc.generate_mips_enable;
  target_desc.bind_flags   = BindFlag_Texture;
  target_desc.access_flags = AccessFlag_Read | AccessFlag_Write;

  if ((target_desc.usage_mode != UsageMode_Default) && (target_desc.usage_mode != UsageMode_Static) && 
      (target_desc.usage_mode != UsageMode_Dynamic) && (target_desc.usage_mode != UsageMode_Stream))
    target_desc.usage_mode = UsageMode_Default;
}

void Texture::GetDesc (BindableTextureDesc& desc)
{
  desc.id     = texture_id;
  desc.target = target;
}

/*
   ����� �������� � �������� OpenGL
*/

void Texture::Bind ()
{
  MakeContextCurrent ();
  glBindTexture(target, texture_id);
  CheckErrors ("render::low_level::opengl::Texture::Bind");
}

/*
   ������ � �������
*/

void Texture::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer)
{
  static Extension EXT_packed_depth_stencil     = "GL_EXT_packed_depth_stencil",
                   EXT_framebuffer_object       = "GL_EXT_framebuffer_object";
    
  bool has_ext_packed_depth_stencil = GetContextManager().IsSupported (EXT_packed_depth_stencil) && GetContextManager().IsSupported (EXT_framebuffer_object);  

  if (!buffer)
    common::RaiseNullArgument ("render::low_level::opengl::Texture::SetData", "buffer");

  if ((source_format == PixelFormat_D24S8)  && (!has_ext_packed_depth_stencil))
    common::RaiseNotSupported ("render::low_level::opengl::Texture::SetData", 
                               "Can't set depth-stencil data. Reason: GL_EXT_packed_depth_stencil extension not supported");

  if (is_depth_format (desc.format) && !is_depth_format (source_format))
    common::RaiseInvalidArgument ("render::low_level::opengl::Texture::SetData", "source_format");
  if (!is_depth_format (desc.format) && is_depth_format (source_format))
    common::RaiseInvalidArgument ("render::low_level::opengl::Texture::SetData", "source_format");
}

void Texture::GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer)
{
  if (!buffer)
    common::RaiseNullArgument ("render::low_level::opengl::Texture::GetData", "buffer");
  if ((target_format == PixelFormat_D24S8)  && (desc.format != PixelFormat_D24S8))
    common::RaiseInvalidArgument ("render::low_level::opengl::Texture::GetData", "target_format");
  if (is_depth_format (desc.format) && !is_depth_format (target_format))
    common::RaiseInvalidArgument ("render::low_level::opengl::Texture::GetData", "target_format");
  if (!is_depth_format (desc.format) && is_depth_format (target_format))
    common::RaiseInvalidArgument ("render::low_level::opengl::Texture::GetData", "target_format");
}

/*
   ��������� ���������� ��������
*/

namespace render
{

namespace low_level
{

namespace opengl
{
  
size_t texel_size (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_S8:    return 1;
    case PixelFormat_LA8:
    case PixelFormat_D16:   return 2;
    case PixelFormat_RGB8:  return 3;
    case PixelFormat_RGBA8:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8: return 4;  
    default: common::RaiseInvalidArgument ("render::low_level::opengl::Texture::texel_size", "format"); return 1;
  }
}

GLint gl_internal_format (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_L8:    return GL_LUMINANCE8;
    case PixelFormat_A8:    return GL_ALPHA8;
    case PixelFormat_LA8:   return GL_LUMINANCE8_ALPHA8;
    case PixelFormat_RGB8:  return GL_RGB8;
    case PixelFormat_RGBA8: return GL_RGBA8;
    case PixelFormat_DXT1:  return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
    case PixelFormat_DXT3:  return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    case PixelFormat_DXT5:  return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    case PixelFormat_D16:   return GL_DEPTH_COMPONENT16_ARB;
    case PixelFormat_D24X8: return GL_DEPTH_COMPONENT24_ARB;
    case PixelFormat_D24S8: return GL_DEPTH24_STENCIL8_EXT;  
    case PixelFormat_S8:    common::RaiseNotSupported ("render::low_level::opengl::Texture::gl_format", "Stencil textures not supported.");
    default: common::RaiseInvalidArgument ("render::low_level::opengl::Texture::gl_internal_format", "format"); return GL_ALPHA;
  }
}

GLenum gl_format (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_DXT1:  return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
    case PixelFormat_DXT3:  return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    case PixelFormat_DXT5:  return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    case PixelFormat_L8:    return GL_LUMINANCE;
    case PixelFormat_A8:    return GL_ALPHA;
    case PixelFormat_LA8:   return GL_LUMINANCE_ALPHA;
    case PixelFormat_RGB8:  return GL_RGB;
    case PixelFormat_RGBA8: return GL_RGBA;
    case PixelFormat_D16:   
    case PixelFormat_D24X8: return GL_DEPTH_COMPONENT;
    case PixelFormat_D24S8: return GL_DEPTH_STENCIL_EXT;  
    case PixelFormat_S8:    common::RaiseNotSupported ("render::low_level::opengl::Texture::gl_format", "Stencil textures not supported.");
    default: common::RaiseInvalidArgument ("render::low_level::opengl::Texture::gl_format", "format"); return GL_ALPHA;
  }
}

GLenum gl_type (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_L8:    
    case PixelFormat_A8:    
    case PixelFormat_S8:
    case PixelFormat_LA8:   
    case PixelFormat_RGB8:  
    case PixelFormat_RGBA8: 
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:  return GL_UNSIGNED_BYTE;
    case PixelFormat_D16:   return GL_UNSIGNED_SHORT;
    case PixelFormat_D24X8: return GL_UNSIGNED_INT;
    case PixelFormat_D24S8: return GL_UNSIGNED_INT_24_8_EXT;
    default: common::RaiseInvalidArgument ("render::low_level::opengl::Texture::gl_type", "format"); return GL_UNSIGNED_BYTE;
  }
}

bool is_compressed_format (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:  return true;
    case PixelFormat_L8:    
    case PixelFormat_A8:    
    case PixelFormat_S8:
    case PixelFormat_LA8:   
    case PixelFormat_RGB8:  
    case PixelFormat_RGBA8: 
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8:   
    default: return false;
  }
}

bool is_depth_format (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8: return true;   
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
    case PixelFormat_L8:    
    case PixelFormat_A8:    
    case PixelFormat_S8:
    case PixelFormat_LA8:   
    case PixelFormat_RGB8:  
    case PixelFormat_RGBA8: 
    default: return false;
  }
}

size_t compressed_quad_size (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_DXT1: return 8;
    case PixelFormat_DXT3: return 16;
    case PixelFormat_DXT5: return 16;
    default: common::RaiseInvalidArgument ("render::low_level::opengl::Texture::compressed_texel_size", "format"); return 1;
  }
}

}

}

}
