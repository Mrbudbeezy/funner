#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::egl;

namespace
{

/*
    ���������
*/

const size_t OUTPUT_MAX_NAME_SIZE  = 128; //������������ ������ ����� ���������� ������
const size_t CONFIG_MAX_ATTRIBUTES = 128; //������������ ���������� ��������� � ������������

}

/*
    �������� ���������� ��������� ������� ������
*/

typedef xtl::com_ptr<Adapter> AdapterPtr;

struct PrimarySwapChain::Impl
{
  AdapterPtr        adapter;                     //�������, �������� ����������� ����������
  Log               log;                         //��������
  NativeWindowType  native_window;               //����
  NativeDisplayType native_display;              //������� ���������-��������� ���������� ������
  EGLDisplay        egl_display;                 //������� ���������� ������
  EGLConfig         egl_config;                  //������� ������������
  EGLSurface        egl_surface;                 //������� ����������� ���������
  char              name [OUTPUT_MAX_NAME_SIZE]; //��� ������� ������
  SwapChainDesc     desc;                        //���������� ������� ������
  PropertyList      properties;                  //�������� ������� ������

///�����������
  Impl (Adapter* in_adapter, const SwapChainDesc& in_desc)
    : adapter (in_adapter)
    , native_window (0)
    , native_display (0)
    , egl_display (0)
    , egl_config (0)
    , egl_surface (0)    
  {
    *name = 0;    
    
    bool egl_initialized = false;

    try
    {
        //���������-��������� �������������
      
      PlatformInitialize (in_desc);
      
        //�������� �������
        
      log.Printf ("...create display");
        
      egl_display = eglGetDisplay (native_display);
      
      if (!egl_display)
        log.Printf ("...using default display");
        
        //������������� EGL
        
      log.Printf ("...initialize EGL");            
      
      EGLint major_version = 0, minor_version = 0;
      
      if (!eglInitialize (egl_display, &major_version, &minor_version))
        raise_error ("::eglInitialize");
        
      egl_initialized = true;

      log.Printf ("...EGL intialized successfull (version %d.%d)", major_version, minor_version);      
      
        //����� ������������

      EGLint config_attributes [CONFIG_MAX_ATTRIBUTES], *attr = config_attributes;
      
      *attr++ = EGL_BUFFER_SIZE;
      *attr++ = in_desc.frame_buffer.color_bits;
      *attr++ = EGL_ALPHA_SIZE;
      *attr++ = in_desc.frame_buffer.alpha_bits;      
      *attr++ = EGL_DEPTH_SIZE;
      *attr++ = in_desc.frame_buffer.depth_bits;
      *attr++ = EGL_STENCIL_SIZE;
      *attr++ = in_desc.frame_buffer.stencil_bits;
      *attr++ = EGL_SAMPLES;
      *attr++ = in_desc.samples_count;
      
      if (in_desc.buffers_count > 1 || !in_desc.buffers_count)
        throw xtl::format_not_supported_exception ("", "desc.buffers_count=%u not supported", in_desc.buffers_count);

      switch (in_desc.swap_method)
      {
        case SwapMethod_Discard:
        case SwapMethod_Flip:
        case SwapMethod_Copy:
          break;
        default:
          throw xtl::make_argument_exception ("", "desc.swap_method", desc.swap_method);
      }            
      
      *attr++ = EGL_SURFACE_TYPE;
      *attr++ = EGL_WINDOW_BIT;
      *attr++ = EGL_NONE;      

      EGLint configs_count = 0;

      if (!eglChooseConfig (egl_display, config_attributes, &egl_config, 1, &configs_count))
        raise_error ("::eglChooseConfig");

      if (!configs_count)
        throw xtl::format_operation_exception ("", "Bad EGL configuration (RGB/A: %u/%u, D/S: %u/%u, Samples: %u)",
          in_desc.frame_buffer.color_bits, in_desc.frame_buffer.alpha_bits, in_desc.frame_buffer.depth_bits,
          in_desc.frame_buffer.stencil_bits, in_desc.samples_count);
        
      log.Printf ("...choose configuration #%u (RGB/A: %u/%u, D/S: %u/%u, Samples: %u)",
        egl_config, in_desc.frame_buffer.color_bits, in_desc.frame_buffer.alpha_bits, in_desc.frame_buffer.depth_bits,
        in_desc.frame_buffer.stencil_bits, in_desc.samples_count);
        
      log.Printf ("...create window surface");
        
        //�������� ����������� ���������

      egl_surface = eglCreateWindowSurface (egl_display, egl_config, native_window, 0);
      
      if (!egl_surface)
        raise_error ("::eglCreateWindowSurface");

        //���������� ����������� ����������        

      desc = in_desc;

      desc.frame_buffer.width        = GetSurfaceAttribute (EGL_WIDTH);
      desc.frame_buffer.height       = GetSurfaceAttribute (EGL_HEIGHT);
      desc.frame_buffer.color_bits   = GetConfigAttribute (EGL_BUFFER_SIZE);
      desc.frame_buffer.alpha_bits   = GetConfigAttribute (EGL_ALPHA_SIZE);
      desc.frame_buffer.depth_bits   = GetConfigAttribute (EGL_DEPTH_SIZE);
      desc.frame_buffer.stencil_bits = GetConfigAttribute (EGL_STENCIL_SIZE);
      desc.samples_count             = GetConfigAttribute (EGL_SAMPLES);
      desc.fullscreen                = false;
      desc.vsync                     = false;
      
        //��������� ������� ������� ������
        
      properties.AddProperty ("egl_vendor",      GetEglString (EGL_VENDOR));
      properties.AddProperty ("egl_version",     GetEglString (EGL_VERSION));
      properties.AddProperty ("egl_extensions",  GetEglString (EGL_EXTENSIONS));
      properties.AddProperty ("egl_client_apis", GetEglString (EGL_CLIENT_APIS));
    }
    catch (...)
    {
      if (egl_surface)
        eglDestroySurface (egl_display, egl_surface);

      if (egl_initialized)
        eglTerminate (egl_display);

      PlatformDone ();
      
      throw;
    }
  }
  
///����������
  ~Impl ()
  {
    try
    {
      eglDestroySurface (egl_display, egl_surface);
      eglTerminate (egl_display);

      PlatformDone ();
    }
    catch (...)
    {
    }
  }
  
///��������� ��������
  EGLint GetConfigAttribute (EGLint attribute)
  {
    EGLint value = 0;
    
    if (!eglGetConfigAttrib (egl_display, egl_config, attribute, &value))
      raise_error ("::eglGetConfigAttrib");
      
    return value;
  }
  
///��������� �������� �������� ����������� ���������
  EGLint GetSurfaceAttribute (EGLint attribute)
  {
    EGLint value = 0;
    
    if (!eglQuerySurface (egl_display, egl_surface, attribute, &value))
      raise_error ("::eglQuerySurface");
      
    return value;
  }
  
///��������� ���������� �������� EGL
  const char* GetEglString (EGLint name)
  {
    const char* value = eglQueryString (egl_display, name);
    
    if (!value)
      raise_error ("::eglQueryString");
      
    return value;
  }

#ifdef _WIN32

///���������-��������� �������������
  void PlatformInitialize (const SwapChainDesc& in_desc)
  {
    try
    {
      native_window = (HWND)in_desc.window_handle;
      
      if (!native_window)
        throw xtl::make_null_argument_exception ("", "desc.window_handle");

      log.Printf ("...get device context");

      native_display = ::GetDC (native_window);      

      if (!native_display)
        throw xtl::format_operation_exception ("::GetDC", "Operation failed"); //������� ����� raise_error!!!

      log.Printf ("...get window name");

      if (!GetWindowTextA (native_window, name, sizeof (name)))
        throw xtl::format_operation_exception ("::GetWindowTextA", "Operation failed"); //������� ����� raise_error!!!
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("render::low_level::opengl::egl::Adapter::Impl::PlatformInitializeWin32");
      throw;
    }
  }
  
///���������-��������� ������������ ��������
  void PlatformDone ()
  {    
    if (native_display && native_window)
    {
      log.Printf ("...release device context");
      
      ::ReleaseDC (native_window, native_display);
    }
  }

#else
  #error Unknown platform!
#endif
};

/*
    ����������� / ����������
*/

PrimarySwapChain::PrimarySwapChain (Adapter* adapter, const SwapChainDesc& desc)
{
  try
  {
    if (!adapter)
      throw xtl::make_null_argument_exception ("", "adapter");
    
    impl = new Impl (adapter, desc);
        
    adapter->RegisterOutput (this);
    
    impl->log.Printf ("...swap chain successfully created");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::egl::PrimarySwapChain::PrimarySwapChain");
    throw;
  }
}

PrimarySwapChain::~PrimarySwapChain ()
{
  impl->adapter->UnregisterOutput (this);
}

/*
    ��������� �����
*/

const char* PrimarySwapChain::GetName ()
{
  return impl->name;
}

/*
    ��������� ������ �����-�������
*/

size_t PrimarySwapChain::GetModesCount ()
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::egl::PrimarySwapChain::GetModesCount");
}

void PrimarySwapChain::GetModeDesc (size_t mode_index, OutputModeDesc& mode_desc)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::egl::PrimarySwapChain::GetModeDesc");
}

/*
    ��������� �������� �����-������
*/

void PrimarySwapChain::SetCurrentMode (const OutputModeDesc&)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::egl::PrimarySwapChain::SetCurrentMode");
}

void PrimarySwapChain::GetCurrentMode (OutputModeDesc&)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::egl::PrimarySwapChain::GetCurrentMode");
}

/*
    ���������� �����-����������
*/

void PrimarySwapChain::SetGammaRamp (const Color3f table [256])
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::egl::PrimarySwapChain::SetGammaRamp");
}

void PrimarySwapChain::GetGammaRamp (Color3f table [256])
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::egl::PrimarySwapChain::GetGammaRamp");
}

/*
    ��������� ��������
*/

IAdapter* PrimarySwapChain::GetAdapter ()
{
  return impl->adapter.get ();
}

/*
    ��������� �����������
*/

void PrimarySwapChain::GetDesc (SwapChainDesc& out_desc)
{
  out_desc = impl->desc;
}

/*
    ��������� ���������� ������ � ������������ �������� ������� ����������
*/

IOutput* PrimarySwapChain::GetContainingOutput ()
{
  return this;
}

/*
    ��������� / ������ ��������� full-screen mode
*/

void PrimarySwapChain::SetFullscreenState (bool state)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::egl::PrimarySwapChain::SetFullscreenState");
}

bool PrimarySwapChain::GetFullscreenState ()
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::egl::PrimarySwapChain::GetFullscreenState");
}

/*
    ����� �������� ������� ������ � ��������� ������
*/

void PrimarySwapChain::Present ()
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::egl::PrimarySwapChain::Present");
}

/*
    ������ ������� ���������� ������
*/

IPropertyList* PrimarySwapChain::GetProperties ()
{
  return &impl->properties;
}
