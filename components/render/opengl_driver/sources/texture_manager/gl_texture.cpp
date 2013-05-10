#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    ���������
*/

const size_t DXT_EDGE_SIZE  = 4;                             //������ ������� ����� DXT-������
const size_t DXT_BLOCK_SIZE = DXT_EDGE_SIZE * DXT_EDGE_SIZE; //������ ����� DXT-������

/*
   ����������� / ����������
*/

Texture::Texture
 (const ContextManager& in_context_manager,
  const TextureDesc&    in_desc,
  GLenum                in_target,
  size_t                in_mips_count)
    : BindableTexture (in_context_manager),
      desc (in_desc),
      target (in_target),
      texture_id (0),      
      mips_count (in_mips_count),
      binded_sampler_hash (0)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Texture::Texture";   

    //�������� ������������ �������

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
        //�������� ����������� ������ ��������� � mip-mapping
        
      if (desc.generate_mips_enable)
        throw xtl::format_operation_exception (METHOD_NAME, "Auto-generate mipmaps incompatible with compressed textures (desc.format=%s)",
          get_name (desc.format));

      break;
    case PixelFormat_RGB_PVRTC2:
    case PixelFormat_RGB_PVRTC4:
    case PixelFormat_RGBA_PVRTC2:
    case PixelFormat_RGBA_PVRTC4:
      if (!GetCaps ().has_img_texture_compression_pvrtc)
        throw xtl::format_not_supported_exception (METHOD_NAME, "PVRTC texture compression doesn't supported");
        
      if (desc.generate_mips_enable)
        throw xtl::format_operation_exception (METHOD_NAME, "Auto-generate mipmaps incompatible with compressed textures (desc.format=%s)",
          get_name (desc.format));

      break;
    case PixelFormat_ATC_RGB_AMD:
    case PixelFormat_ATC_RGBA_EXPLICIT_ALPHA_AMD:
    case PixelFormat_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
      if (!GetCaps ().has_amd_compressed_atc_texture)
        throw xtl::format_not_supported_exception (METHOD_NAME, "AMD ATC texture compression doesn't supported");

      if (desc.generate_mips_enable)
        throw xtl::format_operation_exception (METHOD_NAME, "Auto-generate mipmaps incompatible with compressed textures (desc.format=%s)",
          get_name (desc.format));

      break;
    case PixelFormat_D24S8:
      if (!GetCaps ().has_ext_packed_depth_stencil)
        throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create depth-stencil texture (GL_EXT_packed_depth_stencil not supported)");

    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D32:
      if (!GetCaps ().has_arb_depth_texture)
        throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create depth texture (GL_ARB_depth_texture and GL_VERSION_1_4 not supported)");

      break;
    case PixelFormat_S8:
      throw xtl::format_not_supported_exception (METHOD_NAME, "Stencil textures not supported");
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.format", desc.format);
  }
  
    //�������� ������������ ������ �������
    
  switch (desc.access_flags)
  {
    case 0:
    case AccessFlag_Read:
    case AccessFlag_Write:
    case AccessFlag_Read | AccessFlag_Write:
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.access_flags", desc.access_flags);
  }

    //��������� ������ OpenGL-�������������� ��������

  MakeContextCurrent ();
  glGenTextures      (1, &texture_id);
  
    //��������� �������������� ���������

  if (mips_count > 1)
  {
    Bind ();

#ifndef OPENGL_ES_SUPPORT
    glTexParameteri (target, GL_TEXTURE_MAX_LEVEL, mips_count - 1);
#endif

    if (desc.generate_mips_enable && GetCaps ().has_sgis_generate_mipmap)
      glTexParameteri (target, GL_GENERATE_MIPMAP, GL_TRUE);
  }

    //�������� ������

  CheckErrors (METHOD_NAME);
}

Texture::~Texture ()
{
  try
  {
    if (TryMakeContextCurrent ())
    {
      glDeleteTextures   (1, &texture_id);
      CheckErrors        ("");
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Texture::~Texture");

    LogPrintf ("%s\n", exception.what ());
  }
  catch (std::exception& exception)
  {
    LogPrintf ("%s\n", exception.what ());
  }
  catch (...)
  {
    //��������� ��� ����������
  }
}

/*
   ��������� ������������
*/

void Texture::GetDesc (TextureDesc& out_desc)
{
  out_desc = desc;
}

void Texture::GetDesc (BindableTextureDesc& out_desc)
{
  out_desc.id     = texture_id;
  out_desc.target = target;
}

void Texture::GetDesc (RenderTargetTextureDesc& out_desc)
{
  static_cast<TextureDesc&> (out_desc) = desc;

  out_desc.id     = texture_id;
  out_desc.target = target;
}

/*
    ��������� ������� ��������
*/

void Texture::SetFormat (PixelFormat format)
{
  desc.format = format;
}

/*
    ��������� / ��������� ���� ����������� �������������� ��������
*/

void Texture::SetSamplerHash (size_t hash)
{
  binded_sampler_hash = hash;
}

size_t Texture::GetSamplerHash ()
{
  return binded_sampler_hash;
}

/*
   ����� �������� � �������� OpenGL
*/

void Texture::Bind ()
{
    //��������� ���-����������

  const size_t current_active_slot = GetContextCacheValue (CacheEntry_ActiveTextureSlot),
               current_texture_id  = GetContextCacheValue (CacheEntry_TextureId0 + current_active_slot);

    //�������� ������������� �������� ��������

  if (current_texture_id == GetId ())
    return;

    //��������� �������� ���������

  MakeContextCurrent ();

    //������� ��������

  glBindTexture (target, texture_id);

    //�������� ������

  CheckErrors ("render::low_level::opengl::Texture::Bind");

    //���������� ���-����������

  SetContextCacheValue (CacheEntry_TextureId0 + current_active_slot, GetId ());
  
    //���������� � ������������� ���������� ������
    
  StageRebindNotify (Stage_TextureManager);
}

/*
    ��������� ����������� mip-������ ��������
*/

void Texture::GetMipLevelDesc (size_t mip_level, MipLevelDesc& out_desc)
{
  size_t level_width  = desc.width >> mip_level,
         level_height = desc.height >> mip_level;
  
  if (!level_width)  level_width  = 1;
  if (!level_height) level_height = 1;

  out_desc.width  = level_width;
  out_desc.height = level_height;
}

/*
    ��������� ����������� ���� ��������
*/

void Texture::GetLayerDesc (size_t layer, LayerDesc& desc)
{
  desc.target    = target;
  desc.new_index = layer;
}

/*
    ��������� mip-�������
*/

namespace
{

size_t get_next_mip_size (size_t size)
{
  switch (size)
  {
    case 0:
    case 1:   return size;
    default:  return size / 2;
  }
}

}

void Texture::BuildMipmaps
 (size_t       x,
  size_t       y,
  size_t       z,
  size_t       width,
  size_t       unclamped_width,
  size_t       height,
  PixelFormat  format,
  const void*  data)
{
  GLenum gl_format = get_gl_format (format),
         gl_type   = get_gl_type (format);

  const size_t buffer_size = get_image_size (get_next_mip_size (width), get_next_mip_size (height), format);

  xtl::uninitialized_storage<char> dst_buffer (buffer_size),
                                   src_buffer (buffer_size / 2); //�� 4 ������ �� �����! (���������� ��� ������������� ������� � ������ ������� 1)

  const char* src = reinterpret_cast<const char*> (data);
  char*       dst = dst_buffer.data ();
    
  for (size_t mip_level=1; width > 1 || height > 1; mip_level++)
  {
    scale_image_2x_down (format, width, height, src, dst);

    width            = get_next_mip_size (width);
    height           = get_next_mip_size (height);
    unclamped_width  = get_next_mip_size (unclamped_width);
    x               /= 2;
    y               /= 2;    
    
#ifndef OPENGL_ES_SUPPORT

    if (target != GL_TEXTURE_CUBE_MAP)
      z /= 2;

    glPixelStorei (GL_UNPACK_ROW_LENGTH, unclamped_width); //����� ������ � �������� (��� ������� mip-������)

#endif

    SetUncompressedData (z, mip_level, x, y, width, height, gl_format, gl_type, dst);

    dst_buffer.swap (src_buffer);

    dst = dst_buffer.data ();
    src = src_buffer.data ();
  }
}

/*
    ��������� ������
*/

void Texture::SetUncompressedData (size_t, size_t, size_t, size_t, size_t, size_t, GLenum, GLenum, const void*)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::Texture::SetUncompressedData");
}

void Texture::SetCompressedData (size_t, size_t, size_t, size_t, size_t, size_t, GLenum, size_t, const void*)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::Texture::SetCompressedData");
}

/*
   ������ � �������
*/

void Texture::SetData
 (size_t          layer,
  size_t          mip_level,
  size_t          x,
  size_t          y,
  size_t          width,
  size_t          height,
  PixelFormat     source_format,
  const void*     buffer, 
  IDeviceContext* context)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Texture::SetData";

  if (context)
    throw xtl::format_operation_exception (METHOD_NAME, "Context must be null");
  
    //�������� ����������� ������
    
  if (!(desc.access_flags & AccessFlag_Write))
    throw xtl::format_operation_exception (METHOD_NAME, "Can't set texture data (no AccessFlag_Write in desc.access_flags)");
  
    //�������� ������������ ������� ���� � mip-������

  if (layer >= desc.layers)
    throw xtl::make_range_exception (METHOD_NAME, "layer", layer, desc.layers);

  if (mip_level >= mips_count)
    throw xtl::make_range_exception (METHOD_NAME, "mip_level", mip_level, mips_count);

    //���������� ������������ ��������

  size_t unclamped_width  = width,
         unclamped_height = height;    

    //��������� ����������� mip-������ � ���������� � ���� ��������

  MipLevelDesc level_desc;
  LayerDesc    layer_desc;

  GetMipLevelDesc (mip_level, level_desc);
  GetLayerDesc    (layer, layer_desc);

    //���������

  if (x >= level_desc.width || y >= level_desc.height)
    return;

  if (x + width > level_desc.width)
    width = level_desc.width - x;

  if (y + height > level_desc.height)
    height = level_desc.height - y;    

  if (!width || !height)
    return;

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");    
    
    //��������� �������� � �������� OpenGL

  Bind ();  
  
#ifndef OPENGL_ES_SUPPORT

    //��������� ���������� ������������ ������ � ������

  glPixelStorei (GL_UNPACK_ROW_LENGTH,  unclamped_width); //����� ������ � �������� (��� �������� mip-������)
  glPixelStorei (GL_UNPACK_ALIGNMENT,   1);               //������������ ������ �����
  glPixelStorei (GL_UNPACK_SKIP_ROWS,   0);               //���������� ������������ �����
  glPixelStorei (GL_UNPACK_SKIP_PIXELS, 0);               //���������� ������������ ��������
  
#else

  if (unclamped_width != width)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Texture image clipping doesn't supported");

#endif

    //�������� ������������� ��������

  bool is_depth_stencil_source_format = is_depth_stencil (source_format),
       is_depth_stencil_tex_format    = is_depth_stencil (desc.format),
       is_compatible                  = (is_depth_stencil_tex_format  && is_depth_stencil_source_format) ||
                                        (!is_depth_stencil_tex_format && !is_depth_stencil_source_format);

  if (!is_compatible)
  {
    throw xtl::format_not_supported_exception (METHOD_NAME, "Texture format %s incompatible with source_format %s", get_name (desc.format),
      get_name (source_format));
  }                                    
                                
  if (source_format == PixelFormat_D24S8 && !GetCaps ().has_ext_packed_depth_stencil)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported format %s (GL_EXT_packed_depth_stencil not supported)", get_name (source_format));

    //�������������� ������� ��������
  
  GLenum gl_format = get_gl_format (source_format),
         gl_type   = get_gl_type (source_format);

  if (is_compressed (source_format))
  {
      //�������� ������������ ��������
      
    switch (source_format)
    {
      case PixelFormat_DXT1:
      case PixelFormat_DXT3:
      case PixelFormat_DXT5:
      {
        if ((x % DXT_EDGE_SIZE) || (y % DXT_EDGE_SIZE))
          throw xtl::format_not_supported_exception (METHOD_NAME, "Offset (%u, %u) in compressed texture must be a multiple of 4", x, y);

        if (width % DXT_EDGE_SIZE == 3 || height % DXT_EDGE_SIZE == 3)
          throw xtl::format_not_supported_exception (METHOD_NAME, "Wrong block size (%u, %u) of compressed texture", width, height);

          //����������� ������� ������

        if (GetCaps ().has_ext_texture_compression_s3tc && source_format == desc.format)
        {
          size_t buffer_size = get_image_size (width, height, source_format);

          SetCompressedData (layer, mip_level, x, y, width, height, gl_format, buffer_size, buffer);
        }
        else
        {
          xtl::uninitialized_storage<char> unpacked_buffer (get_uncompressed_image_size (unclamped_width, unclamped_height, source_format));

          unpack_dxt (source_format, unclamped_width, unclamped_height, buffer, unpacked_buffer.data ());

          gl_format = get_uncompressed_gl_format (source_format);
          gl_type   = get_uncompressed_gl_type (source_format);

          SetUncompressedData (layer, mip_level, x, y, width, height, gl_format, gl_type, unpacked_buffer.data ());
        }
        
        break;
      }
      case PixelFormat_RGB_PVRTC2:
      case PixelFormat_RGB_PVRTC4:
      case PixelFormat_RGBA_PVRTC2:
      case PixelFormat_RGBA_PVRTC4:
      case PixelFormat_ATC_RGB_AMD:
      case PixelFormat_ATC_RGBA_EXPLICIT_ALPHA_AMD:
      case PixelFormat_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
      {
        size_t buffer_size = get_image_size (width, height, source_format);

        SetCompressedData (layer, mip_level, x, y, width, height, gl_format, buffer_size, buffer);

        break;
      }
      default:
        throw xtl::format_not_supported_exception (METHOD_NAME, "Set data to with compression format %s not supported", get_name (source_format));
    }
  }
  else
  {
      //����������� ��������� ������

      //�������������� ������ ������ �������

    xtl::uninitialized_storage<size_t> tmp_buffer;

    switch (source_format)
    {
#ifndef OPENGL_ES_SUPPORT
      case PixelFormat_D16:
      {
        tmp_buffer.resize (unclamped_width * unclamped_height);

        size_t*               dst_pixel = tmp_buffer.data ();
        const unsigned short* src_pixel = static_cast<const unsigned short*> (buffer);

        for (size_t count=tmp_buffer.size (); count--; src_pixel++, dst_pixel++)
          *dst_pixel = size_t (*src_pixel) << 16;

        buffer  = tmp_buffer.data ();
        gl_type = GL_UNSIGNED_INT;
        
        break;        
      }        
#else
      case PixelFormat_D16: //need test!
        break;
#endif
      case PixelFormat_D24X8:
      {
        tmp_buffer.resize (unclamped_width * unclamped_height);        

        size_t*       dst_pixel = tmp_buffer.data ();
        const size_t* src_pixel = static_cast<const size_t*> (buffer);

        for (size_t count=tmp_buffer.size (); count--; src_pixel++, dst_pixel++)
          *dst_pixel = *src_pixel << 8;

        buffer = tmp_buffer.data ();

        break;
      }
      default:
        break;
    }      

    SetUncompressedData (layer, mip_level, x, y, width, height, gl_format, gl_type, buffer);
    
    if (desc.generate_mips_enable && !mip_level && !GetCaps ().has_sgis_generate_mipmap)
    {
      BuildMipmaps (x, y, layer, width, unclamped_width, height, source_format, buffer);
    }
  }     

    //�������� ������

  CheckErrors (METHOD_NAME);
}


#ifndef OPENGL_ES_SUPPORT

void Texture::GetData
 (size_t          layer,
  size_t          mip_level,
  size_t          x,
  size_t          y,
  size_t          width,
  size_t          height,
  PixelFormat     target_format,
  void*           buffer,
  IDeviceContext* context)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Texture::GetData";

  if (context)
    throw xtl::format_operation_exception (METHOD_NAME, "Context must be null");

    //�������� ����������� ������

  if (!(desc.access_flags & AccessFlag_Read))
    throw xtl::format_operation_exception (METHOD_NAME, "Can't get texture data (no AccessFlag_Read in desc.access_flags)");

    //�������� ������������ ������� ���� � mip-������

  if (layer >= desc.layers)
    throw xtl::make_range_exception (METHOD_NAME, "layer", layer, desc.layers);

  if (mip_level >= mips_count)
    throw xtl::make_range_exception (METHOD_NAME, "mip_level", mip_level, mips_count);
    
    //��������� ����������� mip-������ � ���������� � ���� ��������

  MipLevelDesc level_desc;
  LayerDesc    layer_desc;

  GetMipLevelDesc (mip_level, level_desc);
  GetLayerDesc    (layer, layer_desc);  

    //���������
    
  size_t unclamped_width  = width,
         unclamped_height = height;

  if (x >= level_desc.width || y >= level_desc.height)
    return;    
    
  if (x + width > level_desc.width)
    width = level_desc.width - x;

  if (y + height > level_desc.height)
    height = level_desc.height - y;    

  if (!width || !height)
    return;    

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");    

    //�������� ������������� ��������

  bool is_depth_stencil_target_format = is_depth_stencil (target_format),
       is_depth_stencil_tex_format    = is_depth_stencil (desc.format),
       is_compatible                  = (is_depth_stencil_tex_format  && is_depth_stencil_target_format) ||
                                        (!is_depth_stencil_tex_format && !is_depth_stencil_target_format);

  if (target_format == PixelFormat_D24S8 && desc.format != PixelFormat_D24S8)
    is_compatible = false;

  if (!is_compatible)
  {
    throw xtl::format_not_supported_exception (METHOD_NAME, "Texture format %s incompatible with target_format %s", get_name (desc.format),
      get_name (target_format));
  }
  
    //��������� �������� � �������� OpenGL

  Bind ();
  
  const ContextCaps& caps = GetCaps ();  
  
    //��������� ���������� ������������ ������ � ������

  glPixelStorei (GL_PACK_ROW_LENGTH,  0); //������ ������ � ��������
  glPixelStorei (GL_PACK_ALIGNMENT,   1); //������������ ������ �����
  glPixelStorei (GL_PACK_SKIP_ROWS,   0); //���������� ������������ �����
  glPixelStorei (GL_PACK_SKIP_PIXELS, 0); //���������� ������������ ��������

  if (caps.has_ext_texture3d)
  {
    glPixelStorei (GL_PACK_SKIP_IMAGES_EXT,  0); //���������� ������������ �����
    glPixelStorei (GL_PACK_IMAGE_HEIGHT_EXT, 0); //������ ���� � ��������
  }

    //�������� �������� ������ ����������� ������� ������

  bool is_full_image = width == level_desc.width && height == level_desc.height && desc.layers == 1 && !x && !y;

  if (is_compressed (target_format))
  {    
      //�������� ����������� �����������
      
    if (target_format != desc.format)
      throw xtl::format_not_supported_exception (METHOD_NAME, "Can't get compressed texture data, target format %s mismatch with texture format %s", 
      get_name (target_format), get_name (desc.format));            

    if (is_full_image)
    {      
      caps.glGetCompressedTexImage_fn (layer_desc.target, mip_level, buffer);
    }
    else
    {            
        //�������� ������������
        
      switch (target_format)
      {
        case PixelFormat_RGB_PVRTC2:
        case PixelFormat_RGB_PVRTC4:
        case PixelFormat_RGBA_PVRTC2:
        case PixelFormat_RGBA_PVRTC4:
        case PixelFormat_ATC_RGB_AMD:
        case PixelFormat_ATC_RGBA_EXPLICIT_ALPHA_AMD:
        case PixelFormat_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
          throw xtl::format_not_supported_exception (METHOD_NAME, "Get texture sub-image data not supported for format=%s", get_name (target_format));
        case PixelFormat_DXT1:
        case PixelFormat_DXT3:
        case PixelFormat_DXT5:
        {
          if (!caps.has_ext_texture_compression_s3tc)
            throw xtl::format_not_supported_exception (METHOD_NAME, "Texture packing not supported (GL_EXT_texture_compression_s3tc not supported)");              

          if (x % DXT_EDGE_SIZE || y % DXT_EDGE_SIZE)
            throw xtl::format_not_supported_exception (METHOD_NAME, "Offset (%u, %u) in compressed texture must be a multiple of 4", x, y);
            
          if (width % DXT_EDGE_SIZE == 3 || height % DXT_EDGE_SIZE == 3)
            throw xtl::format_not_supported_exception (METHOD_NAME, "Wrong block size (%u, %u) of compressed texture", width, height);

            //������� � ����������� ����� � DXT

          x                /= DXT_EDGE_SIZE;
          y                /= DXT_EDGE_SIZE;
          width             = stl::max (width / DXT_EDGE_SIZE, DXT_EDGE_SIZE);
          height            = stl::max (height / DXT_EDGE_SIZE, DXT_EDGE_SIZE);
          unclamped_width   = stl::max (unclamped_width / DXT_EDGE_SIZE, DXT_EDGE_SIZE);
          unclamped_height  = stl::max (unclamped_height / DXT_EDGE_SIZE, DXT_EDGE_SIZE);

            //����������� ������� ������ �� ��������� �����

          size_t layer_size = get_image_size (level_desc.width, level_desc.height, target_format),
                 quad_size  = get_image_size (DXT_EDGE_SIZE, DXT_EDGE_SIZE, target_format);

          xtl::uninitialized_storage<char> temp_buffer (layer_size * desc.layers);

          caps.glGetCompressedTexImage_fn (layer_desc.target, mip_level, temp_buffer.data ());

            //����������� ����� ������ � ���������������� �����

          size_t src_line_size    = stl::max (level_desc.width / DXT_EDGE_SIZE, DXT_EDGE_SIZE) * quad_size,
                 src_start_offset = layer_desc.new_index * layer_size + y * src_line_size + x * quad_size,
                 block_size       = width * quad_size,
                 dst_line_size    = unclamped_width * quad_size;

          const char* src = temp_buffer.data () + src_start_offset;
          char*       dst = reinterpret_cast<char*> (buffer);

          for (size_t i=0; i<height; i++, src += src_line_size, dst += dst_line_size)
            memcpy (dst, src, block_size);
            
          break;
        }
        default:
          throw xtl::format_not_supported_exception (METHOD_NAME, "Get data with compression format %s not supported", get_name (target_format));
      }
    }
  }
  else
  {
    if (is_compressed (desc.format))
      throw xtl::format_not_supported_exception (METHOD_NAME, "Can't get uncompressed data (format %s) from compressed texture (format %s)", get_name (target_format), get_name (desc.format));

      //����������� ��������� ������
      
    GLenum gl_format = get_gl_format (target_format),
           gl_type   = get_gl_type (target_format);

    if (is_full_image)
    {
      glGetTexImage (layer_desc.target, mip_level, gl_format, gl_type, buffer);

      switch (target_format)
      {
        case PixelFormat_D24X8:
        {
          size_t* pixel = static_cast<size_t*> (buffer);

          for (size_t count=width*height; count--; pixel++)
            *pixel >>= 8;

          break;
        }
        default:
          break;
      }
    }
    else
    {
        //����������� ������� ������ �������� �� ��������� �����

      size_t texel_size = get_texel_size (target_format);

      xtl::uninitialized_storage<char> temp_buffer (get_image_size (level_desc.width, level_desc.height, desc.layers, target_format));

      glGetTexImage (layer_desc.target, mip_level, gl_format, gl_type, temp_buffer.data ());

        //����������� ����� ������ � ��������������� �����

      size_t src_line_size    = level_desc.width * texel_size,
             src_layer_size   = src_line_size * level_desc.height,
             src_start_offset = layer_desc.new_index * src_layer_size + y * src_line_size + x * texel_size,
             block_size       = width * texel_size,
             dst_line_size    = unclamped_width * texel_size;

      switch (target_format)
      {
        default:
        {
          const char* src = temp_buffer.data () + src_start_offset;
          char*       dst = reinterpret_cast<char*> (buffer);

          for (size_t i=0; i<height; i++, src += src_line_size, dst += block_size)
            memcpy (dst, src, block_size);

          break;          
        }
        case PixelFormat_D24X8:
        {
            //�������������� ������ ������ �������

          const size_t* src = reinterpret_cast<const size_t*> (temp_buffer.data () + src_start_offset);
          size_t*       dst = reinterpret_cast<size_t*> (buffer);

          block_size    /= sizeof (size_t);
          src_line_size /= sizeof (size_t);
          dst_line_size /= sizeof (size_t);
          
          size_t src_skip_size = src_line_size - block_size,
                 dst_skip_size = dst_line_size - block_size;

          for (size_t i=0; i<height; i++, src += src_skip_size, dst += dst_skip_size)
          {
            for (size_t j=0; j<block_size; j++, src++, dst++)
              *dst = *src >> 8;
          }

          break;
        }
      }
    }
  }

    //�������� ������

  CheckErrors (METHOD_NAME);
}

#else

void Texture::GetData
 (size_t      layer,
  size_t      mip_level,
  size_t      x,
  size_t      y,
  size_t      width,
  size_t      height,
  PixelFormat target_format,
  void*       buffer)
{
  throw xtl::format_not_supported_exception ("render::low_level::opengl::Texture::GetData", "Get texture image data not supported");
}

#endif
