#include "shared.h"

using namespace render;
using namespace render::low_level;

namespace
{

/*
    ���� ����������
*/

struct RenderTargetDesc
{
  size_t          layer;         //����� ����
  size_t          mip_level;     //����� ���-������
  RenderTargetPtr render_target; //���� ���������� 
  
  RenderTargetDesc (size_t in_layer, size_t in_mip_level, const RenderTargetPtr& in_render_target)
    : layer (in_layer)
    , mip_level (in_mip_level)
    , render_target (in_render_target)
  {
  }
};

typedef stl::vector<RenderTargetDesc> RenderTargetArray;

}

/*
    �������� ���������� ��������
*/

struct TextureImpl::Impl
{
  DeviceManagerPtr               device_manager; //�������� ����������
  LowLevelTexturePtr             texture;        //��������
  TextureDimension               dimension;      //����������� ��������
  render::PixelFormat            format;         //������ ��������
  render::low_level::PixelFormat target_format;  //������� ������ ��������
  size_t                         width;          //������ ��������
  size_t                         height;         //������ ��������
  size_t                         depth;          //������� �������� ���� ���������� ����
  RenderTargetArray              render_targets; //���� ����������

///�����������
  Impl (const DeviceManagerPtr& in_device_manager, TextureDimension in_dimension, PixelFormat in_format, render::low_level::PixelFormat in_target_format)
    : device_manager (in_device_manager)
    , dimension (in_dimension)
    , format (in_format)
    , target_format (in_target_format)
    , width (0)
    , height (0)
    , depth (0)
  {
  }
  
///��������� ���� ����������
  RenderTargetPtr GetRenderTarget (size_t layer, size_t mip_level)
  {
      //����� ���� ���������� ����� ������ ��� ���������
    
    for (RenderTargetArray::iterator iter=render_targets.begin (), end=render_targets.end (); iter!=end; ++iter)
      if (iter->layer == layer && iter->mip_level == mip_level)
        return iter->render_target;
        
     //�������� ����� ���� ����������
     
   RenderTargetPtr render_target (new RenderTargetImpl (device_manager, &*texture, layer, mip_level), false);
   
   render_targets.push_back (RenderTargetDesc (layer, mip_level, render_target));
   
   return render_target;
  }
};

/*
    ����������� / ���������� / ������������
*/

TextureImpl::TextureImpl 
 (const DeviceManagerPtr&  device_manager,
  render::TextureDimension dimension,
  size_t                   width,
  size_t                   height,
  size_t                   depth,
  render::PixelFormat      format,
  bool                     generate_mips_enable)
{
  try
  {
      //�������������� ����������
      
    if (!device_manager)
      throw xtl::make_null_argument_exception ("", "device_manager");
    
    low_level::PixelFormat      target_format;
    low_level::TextureDimension target_dimension;
    
    switch (format)
    {
      case PixelFormat_RGB8:
        target_format = low_level::PixelFormat_RGB8;
        break;
      case PixelFormat_RGBA8:
        target_format = low_level::PixelFormat_RGBA8;
        break;
      case PixelFormat_L8:
        target_format = low_level::PixelFormat_L8;
        break;
      case PixelFormat_A8:
        target_format = low_level::PixelFormat_A8;
        break;
      case PixelFormat_LA8:
        target_format = low_level::PixelFormat_LA8;
        break;
      default:
        throw xtl::make_argument_exception ("", "format", format);
    }
    
    switch (dimension)
    {
      case TextureDimension_2D:
        target_dimension = low_level::TextureDimension_2D;
        break;
      case TextureDimension_3D:
        target_dimension = low_level::TextureDimension_3D;
        break;
      case TextureDimension_Cubemap:
        target_dimension = low_level::TextureDimension_Cubemap;
        break;
      default:
        throw xtl::make_argument_exception ("", "dimension", dimension);
    }

      //�������� ��������
      
    low_level::TextureDesc desc;
    
    memset (&desc, 0, sizeof desc);
    
    desc.dimension            = target_dimension;
    desc.width                = width;
    desc.height               = height;
    desc.layers               = depth;
    desc.format               = target_format;
    desc.generate_mips_enable = generate_mips_enable;
    desc.access_flags         = low_level::AccessFlag_ReadWrite;
    desc.bind_flags           = BindFlag_Texture | BindFlag_RenderTarget;
    desc.usage_mode           = UsageMode_Static;

    impl = new Impl (device_manager, dimension, format, target_format);

    impl->width   = width;
    impl->height  = height;
    impl->depth   = depth;
    impl->format  = format;
    impl->texture = LowLevelTexturePtr (device_manager->Device ().CreateTexture (desc), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::TextureImpl::TextureImpl");
    throw;
  }
}

TextureImpl::~TextureImpl ()
{
}

/*
    �������������� ������ ��������
*/

LowLevelTexturePtr TextureImpl::DeviceTexture ()
{
  return impl->texture;
}

/*
    �����������
*/

render::TextureDimension TextureImpl::Dimension ()
{
  return impl->dimension;
}

/*
    ������ � �������
*/

render::PixelFormat TextureImpl::Format ()
{
  return impl->format;
}

size_t TextureImpl::Width ()
{
  return impl->width;
}

size_t TextureImpl::Height ()
{
  return impl->height;
}

size_t TextureImpl::Depth ()
{
  return impl->depth;
}

/*
    ��������� ���� ����������
*/

RenderTargetPtr TextureImpl::RenderTarget (size_t layer, size_t mip_level)
{
  try
  {
    return impl->GetRenderTarget (layer, mip_level);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::TextureImpl::RenderTarget");
    throw;
  }
}

/*
    ���������� ������
*/

void TextureImpl::Update (const media::Image& image)
{
  try
  {
      //�������� ����������� ����������
      
    if (image.Width () != impl->width)
      throw xtl::format_operation_exception ("", "Image width %u mismatch texture width %u", image.Width (), impl->width);
      
    if (image.Height () != impl->height)
      throw xtl::format_operation_exception ("", "Image height %u mismatch texture height %u", image.Height (), impl->height);
      
    if (image.Depth () != impl->depth)
      throw xtl::format_operation_exception ("", "Image depth %u mismatch texture depth %u", image.Depth (), impl->depth);
      
    low_level::PixelFormat source_format;
    
    switch (image.Format ())
    {
      case media::PixelFormat_RGB8:
        source_format = low_level::PixelFormat_RGB8;
        break;
      case media::PixelFormat_RGBA8:
        source_format = low_level::PixelFormat_RGBA8;
        break;     
      case media::PixelFormat_L8:
        source_format = low_level::PixelFormat_L8;
        break;
      case media::PixelFormat_A8:
        source_format = low_level::PixelFormat_A8;
        break;
      case media::PixelFormat_LA8:
        source_format = low_level::PixelFormat_LA8;
        break;
      case media::PixelFormat_RGB16:
      case media::PixelFormat_BGR8:
      case media::PixelFormat_RGBA16:
      case media::PixelFormat_BGRA8:
      {
        media::PixelFormat convertion_format;
        
        switch (image.Format ())
        {
          case media::PixelFormat_RGB16:
            convertion_format = media::PixelFormat_RGB8;
            break;
          case media::PixelFormat_BGR8:
            convertion_format = media::PixelFormat_RGB8;
            break;
          case media::PixelFormat_RGBA16:
            convertion_format = media::PixelFormat_RGBA8;
            break;
          case media::PixelFormat_BGRA8:
            convertion_format = media::PixelFormat_RGBA8;
            break;
          default:
            throw xtl::format_not_supported_exception ("", "Unsupported image format '%s'", media::get_format_name (image.Format ()));
        }

        media::Image converted_image (image, convertion_format);

        Update (converted_image);

        return;
      }
      default:
        throw xtl::format_not_supported_exception ("", "Unsupported image format '%s'", media::get_format_name (image.Format ()));
    }
    
      //���������� ������

    for (size_t i=0; i<impl->depth; i++)
      impl->texture->SetData (i, 0, 0, 0, impl->width, impl->height, source_format, image.Bitmap (i));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::TextureImpl::Update");
    throw;
  }
}

/*
    ������ ������
*/

namespace
{

//��������� ������� �������� ��� �����������
media::PixelFormat get_format (render::PixelFormat format)
{
  switch (format)
  {
    case render::PixelFormat_RGB8:  return media::PixelFormat_RGB8;
    case render::PixelFormat_RGBA8: return media::PixelFormat_RGBA8;
    case render::PixelFormat_L8:    return media::PixelFormat_L8;
    case render::PixelFormat_A8:    return media::PixelFormat_A8;
    case render::PixelFormat_LA8:   return media::PixelFormat_LA8;
    default:
      throw xtl::format_not_supported_exception ("render::get_format(render::PixelFormat)", "Format '%s' not supported", get_name (format));
  }
}

}

void TextureImpl::Capture (size_t layer, size_t mip_level, media::Image& image)
{
  try
  {
    media::PixelFormat image_format = get_format (impl->format);

    media::Image result_image (impl->width, impl->height, 1, image_format);

    impl->texture->GetData (0, 0, 0, 0, impl->width, impl->height, impl->target_format, result_image.Bitmap ());

    result_image.Swap (image);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::TextureImpl::Capture(size_t,size_t,media::Image&)");
    throw;
  }
}

void TextureImpl::Capture (size_t mip_level, media::Image& image)
{
  try
  {
    media::PixelFormat image_format = get_format (impl->format);
    
    size_t width = impl->width, height = impl->height, depth = impl->depth;
    
    for (size_t i=0; i<mip_level; i++)
    {
      width  = stl::max (width / 2, 1u);
      height = stl::max (height / 2, 1u);
      depth  = impl->dimension == TextureDimension_Cubemap ? depth : stl::max (depth / 2, 1u);
    }

    media::Image result_image (width, height, depth, image_format);

    for (size_t i=0; i<depth; i++)
      impl->texture->GetData (i, mip_level, 0, 0, width, height, impl->target_format, result_image.Bitmap (i));

    result_image.Swap (image);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::TextureImpl::Capture(size_t,media::Image&)");
    throw;
  }
}
