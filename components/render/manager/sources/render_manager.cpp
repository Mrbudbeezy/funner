#include "shared.h"

using namespace render;
using namespace render::low_level;

namespace
{

/*
    ���������
*/

const size_t WINDOW_ARRAY_RESERVE_SIZE = 8; //������������� ����� ����

}

/*
    �������� ���������� ��������� ����������
*/

typedef xtl::signal<void (RenderManager&, Window&)> WindowSignal;
typedef stl::vector<WindowImpl*>                    WindowArray;

struct RenderManagerImpl::Impl: public xtl::trackable
{
  RenderManagerImpl*  owner;                                         //��������
  SettingsPtr         settings;                                      //��������� ����������
  Log                 log;                                           //�������� ���������
  DeviceManagerPtr    device_manager;                                //�������� ���������� ���������
  CacheManagerPtr     cache_manager;                                 //�������� �����������
  PropertyCachePtr    property_cache;                                //��� ������� �������
  WindowSignal        window_signals [RenderManagerWindowEvent_Num]; //������� �������
  WindowArray         windows;                                       //����
  TextureManagerPtr   textures;                                      //��������
  PrimitiveManagerPtr primitives;                                    //���������
  ProgramManagerPtr   programs;                                      //���������
  MaterialManagerPtr  materials;                                     //���������
  EffectManagerPtr    effects;                                       //�������

///�����������
  Impl (RenderManagerImpl* in_owner)
    : owner (in_owner)
    , settings (new render::Settings, false)
    , cache_manager (new render::CacheManager, false)
  {
    windows.reserve (WINDOW_ARRAY_RESERVE_SIZE);
  }
  
///��������� ��������� ����������
  render::DeviceManager& DeviceManager ()
  {
    if (!device_manager)
      throw xtl::format_operation_exception ("render::RenderManagerImpl::Impl::DeviceManager", "Device not initialized. Create rendering window first");
      
    return *device_manager;
  }

///��������� ���� ������� �������
  render::PropertyCache& PropertyCache ()
  {
    if (!property_cache)
      property_cache = PropertyCachePtr (new render::PropertyCache (&DeviceManager ()), false);

    return *property_cache;
  }
  
///���������� �� �������� ����
  void OnWindowDestroy (WindowImpl* window)
  {
    for (WindowArray::iterator iter=windows.begin (), end=windows.end (); iter!=end; ++iter)
      if (*iter == window)
      {
        windows.erase (iter);
        
        render::Window wrapped_window = Wrappers::Wrap<render::Window> (window);
        
        WindowEventNotify (RenderManagerWindowEvent_OnRemoved, wrapped_window);
        
        return;
      }
  }
  
///������� ����������
  void WindowEventNotify (RenderManagerWindowEvent event, render::Window& window)
  {
    if (window_signals [event].empty ())
      return;
      
    try
    {
      RenderManager manager = Wrappers::Wrap<RenderManager> (owner);
      
      window_signals [event](manager, window);
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }
};

/*
    ����������� / ����������
*/

RenderManagerImpl::RenderManagerImpl ()
{
  try
  {
    impl = new Impl (this);
    
      //�������� �����������
    
    media::Image::RegisterDefaultLoaders ();
    media::CompressedImage::RegisterDefaultLoaders ();
    media::rfx::MaterialLibrary::RegisterDefaultLoaders ();
    media::geometry::MeshLibrary::RegisterDefaultLoaders ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderManagerImpl::RenderManagerImpl");
    throw;
  }
}

RenderManagerImpl::~RenderManagerImpl ()
{
}

/*
    ������ �������� ���������� ����������
*/

const char* RenderManagerImpl::Description ()
{
  return impl->DeviceManager ().Device ().GetName ();
}

/*
    ��������� ��������
*/

DeviceManager& RenderManagerImpl::DeviceManager ()
{
  return impl->DeviceManager ();
}

CacheManager& RenderManagerImpl::CacheManager ()
{
  return *impl->cache_manager;
}

TextureManager& RenderManagerImpl::TextureManager ()
{
  try
  {
    if (!impl->textures)
      impl->textures = TextureManagerPtr (new render::TextureManager (&impl->DeviceManager ()), false);    

    return *impl->textures;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderManagerImpl::TextureManager");
    throw;
  }
}

ProgramManager& RenderManagerImpl::ProgramManager ()
{
  try
  {
    if (!impl->programs)
      impl->programs = ProgramManagerPtr (new render::ProgramManager (), false);

    return *impl->programs;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderManagerImpl::ProgramManager");
    throw;
  }
}

MaterialManager& RenderManagerImpl::MaterialManager ()
{
  try
  {
    if (!impl->materials)
      impl->materials = MaterialManagerPtr (new render::MaterialManager (&impl->DeviceManager (), &TextureManager (), &ProgramManager ()), false);

    return *impl->materials;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderManagerImpl::MaterialManager");
    throw;
  }
}

PrimitiveManager& RenderManagerImpl::PrimitiveManager ()
{
  try
  {
    if (!impl->primitives)
      impl->primitives = PrimitiveManagerPtr (new render::PrimitiveManager (&impl->DeviceManager (), &MaterialManager ()), false);    

    return *impl->primitives;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderManagerImpl::PrimitiveManager");
    throw;
  }
}

EffectManager& RenderManagerImpl::EffectManager ()
{
  try
  {
    if (!impl->effects)
      impl->effects = EffectManagerPtr (new render::EffectManager (&impl->DeviceManager (), &TextureManager (), &ProgramManager ()), false);

    return *impl->effects;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderManagerImpl::EffectManager");
    throw;
  }
}

/*
    �������� ���� ����������
*/

WindowPtr RenderManagerImpl::CreateWindow (syslib::Window& sys_window, common::PropertyMap& properties)
{
  try
  {
    WindowPtr window (new WindowImpl (impl->device_manager, sys_window, properties, impl->settings, impl->cache_manager), false);
    
    window->connect_tracker (xtl::bind (&Impl::OnWindowDestroy, &*impl, &*window), *impl);

    impl->windows.push_back (&*window);
    
    if (!impl->device_manager)
      impl->device_manager = window->DeviceManager ();
      
    render::Window wrapped_window = Wrappers::Wrap<render::Window> (window);
    
    impl->WindowEventNotify (RenderManagerWindowEvent_OnAdded, wrapped_window);
    
    return window;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderManagerImpl::CreateWindow");
    throw;
  }
}

/*
    ������� ���� ����������
*/

size_t RenderManagerImpl::WindowsCount ()
{
  return impl->windows.size ();
}

WindowPtr RenderManagerImpl::Window (size_t index)
{
  if (index >= impl->windows.size ())    
    throw xtl::make_range_exception ("render::RenderManagerImpl::Window", "index", index, impl->windows.size ());
    
  return impl->windows [index];
}

/*
    �������� ����� ����������
*/

RenderTargetPtr RenderManagerImpl::CreateRenderBuffer (size_t width, size_t height, render::PixelFormat format)
{
  try
  {
    low_level::TextureDesc texture_desc;
    
    memset (&texture_desc, 0, sizeof texture_desc);
    
    texture_desc.dimension            = low_level::TextureDimension_2D;
    texture_desc.width                = width;
    texture_desc.height               = height;
    texture_desc.layers               = 1;
    texture_desc.generate_mips_enable = false;
    texture_desc.access_flags         = low_level::AccessFlag_ReadWrite;
    texture_desc.bind_flags           = low_level::BindFlag_RenderTarget;
    
    switch (format)
    {
      case PixelFormat_RGB8:
        texture_desc.format = low_level::PixelFormat_RGB8;
        break;
      case PixelFormat_RGBA8:
        texture_desc.format = low_level::PixelFormat_RGBA8;
        break;
      case PixelFormat_L8:
        texture_desc.format = low_level::PixelFormat_L8;
        break;
      case PixelFormat_A8:
        texture_desc.format = low_level::PixelFormat_A8;
        break;
      case PixelFormat_LA8:
        texture_desc.format = low_level::PixelFormat_LA8;
        break;
      default:
        throw xtl::make_argument_exception ("", "format", format);
    }
    
    LowLevelTexturePtr texture (impl->DeviceManager ().Device ().CreateTexture (texture_desc), false);
    
    return RenderTargetPtr (new RenderTargetImpl (&impl->DeviceManager (), &*texture), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderManagerImpl::CreateRenderBuffer");
    throw;
  }
}

RenderTargetPtr RenderManagerImpl::CreateDepthStencilBuffer (size_t width, size_t height)
{
  try
  {
    low_level::TextureDesc texture_desc;
    
    memset (&texture_desc, 0, sizeof texture_desc);
    
    texture_desc.dimension            = low_level::TextureDimension_2D;
    texture_desc.width                = width;
    texture_desc.height               = height;
    texture_desc.layers               = 1;
    texture_desc.format               = low_level::PixelFormat_D24S8;
    texture_desc.generate_mips_enable = false;
    texture_desc.access_flags         = low_level::AccessFlag_ReadWrite;
    texture_desc.bind_flags           = low_level::BindFlag_DepthStencil;
    
    LowLevelTexturePtr texture (impl->DeviceManager ().Device ().CreateTexture (texture_desc), false);
 
    return RenderTargetPtr (new RenderTargetImpl (&impl->DeviceManager (), &*texture), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderManagerImpl::CreateDepthStencilBuffer");
    throw;
  }
}

/*
    �������� ��������
*/

FramePtr RenderManagerImpl::CreateFrame ()
{
  try
  {
    return FramePtr (new FrameImpl (&impl->DeviceManager (), &EffectManager (), &impl->PropertyCache ()), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderManagerImpl::CreateFrame");
    throw;
  }
}

EntityPtr RenderManagerImpl::CreateEntity ()
{
  try
  {
    return EntityPtr (new EntityImpl (&impl->DeviceManager (), &TextureManager (), &PrimitiveManager ()), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderManagerImpl::CreateEntity");
    throw;
  }
}

/*
    �������� ��������
*/

void RenderManagerImpl::LoadResource (const char* resource_name)
{
  try
  {
    if (!resource_name)
      throw xtl::make_null_argument_exception ("", "resource_name");
    
    if (render::TextureManager::IsTextureResource (resource_name))
    {
      TextureManager ().LoadTexture (resource_name);
      return;
    }
    
    if (render::PrimitiveManager::IsMeshLibraryResource (resource_name))
    {
      PrimitiveManager ().LoadMeshLibrary (resource_name);
      return;
    }
    
    if (render::MaterialManager::IsMaterialLibraryResource (resource_name))
    {
      MaterialManager ().LoadMaterialLibrary (resource_name);
      return;
    }    
    
    if (render::EffectManager::IsEffectLibraryResource (resource_name))
    {
      EffectManager ().LoadEffectLibrary (resource_name);
      return;
    }
      
    throw xtl::format_operation_exception ("", "Type of resource '%s' is unknown", resource_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderManagerImpl::LoadResource(const char*)");
    throw;
  }
}

void RenderManagerImpl::UnloadResource (const char* resource_name)
{
  try
  {
    if (!resource_name)
      return;
    
    if (render::TextureManager::IsTextureResource (resource_name))
    {
      TextureManager ().UnloadTexture (resource_name);
      return;
    }
    
    if (render::PrimitiveManager::IsMeshLibraryResource (resource_name))
    {
      PrimitiveManager ().UnloadMeshLibrary (resource_name);
      return;
    }
    
    if (render::MaterialManager::IsMaterialLibraryResource (resource_name))
    {
      MaterialManager ().UnloadMaterialLibrary (resource_name);
      return;
    }
    
    if (render::EffectManager::IsEffectLibraryResource (resource_name))
    {
      EffectManager ().UnloadEffectLibrary (resource_name);
      return;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderManagerImpl::UnloadResource(const char*)");
    throw;
  } 
}

void RenderManagerImpl::LoadResource (const media::rfx::MaterialLibrary& library)
{
  try
  {
    MaterialManager ().LoadMaterialLibrary (library);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderManagerImpl::LoadResource(const media::rfx::MaterialLibrary&)");
    throw;
  }
}

void RenderManagerImpl::UnloadResource (const media::rfx::MaterialLibrary& library)
{
  try
  {
    MaterialManager ().UnloadMaterialLibrary (library);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderManagerImpl::UnloadResource(const media::rfx::MaterialLibrary&)");
    throw;
  }
}

void RenderManagerImpl::LoadResource (const media::geometry::MeshLibrary& library)
{
  try
  {
    PrimitiveManager ().LoadMeshLibrary (library);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderManagerImpl::LoadResource(const media::geometry::MeshLibrary&)");
    throw;
  }
}

void RenderManagerImpl::UnloadResource (const media::geometry::MeshLibrary& library)
{
  try
  {
    PrimitiveManager ().UnloadMeshLibrary (library);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderManagerImpl::UnloadResource(const media::geometry::MeshLibrary&)");
    throw;
  }
}

/*
    ����������� �� �������
*/

xtl::connection RenderManagerImpl::RegisterWindowEventHandler (RenderManagerWindowEvent event, const WindowEventHandler& handler) const
{
  switch (event)
  {
    case RenderManagerWindowEvent_OnAdded:
    case RenderManagerWindowEvent_OnRemoved:
      return impl->window_signals [event].connect (handler);
    default:
      throw xtl::make_argument_exception ("render::RenderManagerImpl::RegisterWindowEventHandler", "event", event);
  }
}

/*
    ���������� ������������
*/

void RenderManagerImpl::UpdateCache ()
{
  throw xtl::make_not_implemented_exception ("render::RenderManagerImpl::UpdateCache");
}

void RenderManagerImpl::ResetCache ()
{
  throw xtl::make_not_implemented_exception ("render::RenderManagerImpl::ResetCache");
}

/*
    ��������� ��������� ����������
*/

render::Settings& RenderManagerImpl::Settings ()
{
  return *impl->settings;
}
