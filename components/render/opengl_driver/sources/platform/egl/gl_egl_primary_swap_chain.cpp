#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::egl;

namespace
{

/*
    ���������
*/

const size_t CONFIG_MAX_ATTRIBUTES = 128; //������������ ���������� ��������� � ������������

}

/*
    �������� ���������� ��������� ������� ������
*/

typedef xtl::com_ptr<Adapter> AdapterPtr;
typedef Output::Pointer       OutputPtr;

struct PrimarySwapChain::Impl
{
  AdapterPtr    adapter;     //�������, �������� ����������� ����������
  OutputPtr     output;      //������� ���������� ������
  Log           log;         //��������
  EGLDisplay    egl_display; //������� ���������� ������
  EGLConfig     egl_config;  //������� ������������
  EGLSurface    egl_surface; //������� ����������� ���������
  SwapChainDesc desc;        //���������� ������� ������
  PropertyList  properties;  //�������� ������� ������

///�����������
  Impl (Adapter* in_adapter, const SwapChainDesc& in_desc)
    : adapter (in_adapter)
    , output (adapter->GetOutput (in_desc.window_handle))
    , egl_display (output->GetEglDisplay ())
    , egl_config (0)
    , egl_surface (0)    
  {
    try
    {
      DisplayLock lock (output->GetDisplay ());
      
        //����� ������������

      EGLint config_attributes [CONFIG_MAX_ATTRIBUTES], *attr = config_attributes;
      
      *attr++ = EGL_BUFFER_SIZE;
      *attr++ = in_desc.frame_buffer.color_bits;
      *attr++ = EGL_ALPHA_SIZE;
      *attr++ = in_desc.frame_buffer.alpha_bits;      
      *attr++ = EGL_DEPTH_SIZE;
      *attr++ = in_desc.frame_buffer.depth_bits;
//      *attr++ = EGL_STENCIL_SIZE; //for tests only!!!!!!!
//      *attr++ = in_desc.frame_buffer.stencil_bits;
      *attr++ = EGL_SAMPLES;
      *attr++ = in_desc.samples_count;
      *attr++ = EGL_SURFACE_TYPE;
      *attr++ = EGL_WINDOW_BIT;
      *attr++ = EGL_RENDERABLE_TYPE;
      *attr++ = EGL_OPENGL_ES_BIT;
      
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
          
      EGLint format = 0;
          
      eglGetConfigAttrib (egl_display, egl_config, EGL_NATIVE_VISUAL_ID, &format);                
        
      log.Printf ("...choose configuration #%u (VisualId: %d, RGB/A: %u/%u, D/S: %u/%u, Samples: %u)",
        egl_config, format, in_desc.frame_buffer.color_bits, in_desc.frame_buffer.alpha_bits, in_desc.frame_buffer.depth_bits,
        in_desc.frame_buffer.stencil_bits, in_desc.samples_count);
        
      log.Printf ("...create window surface");              
        
#ifdef ANDROID

      egl_surface = eglCreateWindowSurfaceAndroid (egl_display, egl_config, output->GetWindowHandle (), format);  
      
#else

        //�������� ����������� ���������

      egl_surface = eglCreateWindowSurface (egl_display, egl_config, (NativeWindowType)output->GetWindowHandle (), 0);
      
#endif      
      
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
      desc.buffers_count             = 1;
      desc.fullscreen                = false;
      
        //��������� ������� ������� ������
        
      try
      {
        properties.AddProperty ("egl_vendor",      GetEglString (EGL_VENDOR));
        properties.AddProperty ("egl_version",     GetEglString (EGL_VERSION));
        properties.AddProperty ("egl_extensions",  GetEglString (EGL_EXTENSIONS));
        properties.AddProperty ("egl_client_apis", GetEglString (EGL_CLIENT_APIS));
      }
      catch (...)
      {
        //���������� ��� ������ ������� EGL �� �������� ���������� ��� ������
        //(����� ���� egl ��� bada)
      }
    }
    catch (...)
    {
      if (egl_surface)
        eglDestroySurface (egl_display, egl_surface);

      throw;
    }
  }
  
///����������
  ~Impl ()
  {
    try
    {    
      DisplayLock lock (output->GetDisplay ());
      
      eglDestroySurface (egl_display, egl_surface);
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
  return impl->output.get ();
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
  try
  {
    DisplayLock lock (impl->output->GetDisplay ());        
    
    if (!eglSwapBuffers (impl->egl_display, impl->egl_surface))
      raise_error ("::eglSwapBuffers");                  
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::egl::PrimarySwapChain::Present");
    throw;
  }
}

/*
    ������ ������� ���������� ������
*/

IPropertyList* PrimarySwapChain::GetProperties ()
{
  return &impl->properties;
}

/*
    ��������� EGL ���������� ������� ������
*/

EGLDisplay PrimarySwapChain::GetEglDisplay ()
{
  return impl->output->GetEglDisplay ();
}

EGLConfig PrimarySwapChain::GetEglConfig ()
{
  return impl->egl_config;
}

EGLSurface PrimarySwapChain::GetEglSurface ()
{
  return impl->egl_surface;
}

NativeDisplayType PrimarySwapChain::GetDisplay ()
{
  return impl->output->GetDisplay ();
}
