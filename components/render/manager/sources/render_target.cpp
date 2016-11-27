#include "shared.h"

using namespace render::manager;
using namespace render::low_level;

/*
    Описание реализации цели рендеринга
*/

struct RenderTargetImpl::Impl
{
  DeviceManagerPtr    device_manager;  //менеджер устройства отрисовки
  LowLevelTexturePtr  texture;         //целевая текстура
  LowLevelViewPtr     view;            //отображение для рендеринга в текстуру
  unsigned int        width;           //ширина цели рендеринга
  unsigned int        height;          //высота цели рендеринга
  math::vec2ui        viewport_offset; //смещение облсти вывода
  
  Impl (const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)
    , width (0)
    , height (0)
  {
  }
};

/*
    Конструктор / деструктор
*/

RenderTargetImpl::RenderTargetImpl (const DeviceManagerPtr& device_manager, render::low_level::ITexture* texture, unsigned int layer, unsigned int mip_level)
{
  try
  {
    if (!device_manager)
      throw xtl::make_null_argument_exception ("", "device_manager");
      
    if (!texture)
      throw xtl::make_null_argument_exception ("", "texture");
    
    impl = new Impl (device_manager);
    
    SetTarget (texture, layer, mip_level);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::RenderTargetImpl::RenderTargetImpl");
    throw;
  }
}

RenderTargetImpl::~RenderTargetImpl ()
{
}

/*
    Получение отображения (в случае отсутствия - исключение)
*/

render::low_level::IView& RenderTargetImpl::View ()
{
  if (!impl->view)
    throw xtl::format_operation_exception ("render::manager::RenderTargetImpl::View", "No view binded");
    
  return *impl->view;
}

/*
    Обновление текстуры
*/

void RenderTargetImpl::SetTarget (render::low_level::ITexture* texture, unsigned int layer, unsigned int mip_level)
{
  try
  {
    if (!texture)
    {
      impl->texture = 0;
      impl->view    = 0;
      impl->width   = 0;
      impl->height  = 0;

      return;
    }
    
    low_level::ViewDesc view_desc;
    
    memset (&view_desc, 0, sizeof view_desc);
    
    view_desc.layer     = layer;
    view_desc.mip_level = mip_level;
    
    LowLevelViewPtr view (impl->device_manager->Device ().CreateView (texture, view_desc), false);
    
    low_level::TextureDesc texture_desc;
    
    texture->GetDesc (texture_desc);
    
    impl->width   = texture_desc.width;
    impl->height  = texture_desc.height;
    impl->view    = view;
    impl->texture = texture;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::RenderTargetImpl::SetTarget");
    throw;
  }
}

/*
    Размеры цели рендеринга
*/

void RenderTargetImpl::Resize (unsigned int width, unsigned int height)
{
  impl->width  = width;
  impl->height = height;
}

unsigned int RenderTargetImpl::Width ()
{
  return impl->width;
}

unsigned int RenderTargetImpl::Height ()
{
  return impl->height;
}

/*
    Смещение области вывода от начала координат цели рендеринга
*/

void RenderTargetImpl::SetViewportOffset (const math::vec2ui& offset)
{
  impl->viewport_offset = offset;
}

const math::vec2ui& RenderTargetImpl::ViewportOffset ()
{
  return impl->viewport_offset;
}

/*
    Захват изображения
*/

void RenderTargetImpl::Capture (media::Image& image)
{
  try
  {
    if (!impl->texture)
      throw xtl::format_operation_exception ("", "No texture binded");
    
    render::low_level::TextureDesc texture_desc;

    impl->texture->GetDesc (texture_desc);    
    
    media::PixelFormat image_format;

    switch (texture_desc.format)
    {
      case render::low_level::PixelFormat_RGB8:
        image_format = media::PixelFormat_RGB8;
        break;
      case render::low_level::PixelFormat_RGBA8:
        image_format = media::PixelFormat_RGBA8;
        break;
      default:
        throw xtl::format_not_supported_exception ("", "Texture uses incompatible format %s", render::low_level::get_name (texture_desc.format));
    }    

    media::Image result_image (texture_desc.width, texture_desc.height, 1, image_format);

    impl->texture->GetData (0, 0, 0, 0, texture_desc.width, texture_desc.height, texture_desc.format, result_image.Bitmap ());    
    
    result_image.Swap (image);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::RenderTargetImpl::Capture");
    throw;
  }
}
