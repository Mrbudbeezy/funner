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
  Log           log;         //��������
  Display*      display;     //���������� � ��������
  SwapChainDesc desc;        //���������� ������� ������
  PropertyList  properties;  //�������� ������� ������

///�����������
  Impl (Adapter* in_adapter, const SwapChainDesc& in_desc)
    : adapter (in_adapter)
    , display (DisplayManager::DisplayHandle ())
  {
    try
    {
        //�������� ����������� ������ GLX
        
      int error_base, event_base;        
        
      if (!glXQueryExtension (display, &error_base, &event_base ) )
      {
          fprintf(stderr, "glxsimple: %s\n", "X server has no OpenGL GLX extension");
          exit(1);
      }
        
      
        //����� ������������

      glxint config_attributes [CONFIG_MAX_ATTRIBUTES], *attr = config_attributes;
      
      *attr++ = glx_BUFFER_SIZE;
      *attr++ = in_desc.frame_buffer.color_bits;
      *attr++ = glx_ALPHA_SIZE;
      *attr++ = in_desc.frame_buffer.alpha_bits;      
      *attr++ = glx_DEPTH_SIZE;
      *attr++ = in_desc.frame_buffer.depth_bits;
//      *attr++ = glx_STENCIL_SIZE; //for tests only!!!!!!!
//      *attr++ = in_desc.frame_buffer.stencil_bits;
      *attr++ = glx_SAMPLES;
      *attr++ = in_desc.samples_count;
      
      switch (in_desc.swap_method)
      {
        case SwapMethod_Discard:
        case SwapMethod_Flip:
        case SwapMethod_Copy:
          break;
        default:
          throw xtl::make_argument_exception ("", "desc.swap_method", desc.swap_method);
      }            
      
      *attr++ = glx_SURFACE_TYPE;
      *attr++ = glx_WINDOW_BIT;
      *attr++ = glx_NONE;      

      glxint configs_count = 0;

      if (!glxChooseConfig (glx_display, config_attributes, &glx_config, 1, &configs_count))
        raise_error ("::glxChooseConfig");

      if (!configs_count)
        throw xtl::format_operation_exception ("", "Bad glx configuration (RGB/A: %u/%u, D/S: %u/%u, Samples: %u)",
          in_desc.frame_buffer.color_bits, in_desc.frame_buffer.alpha_bits, in_desc.frame_buffer.depth_bits,
          in_desc.frame_buffer.stencil_bits, in_desc.samples_count);
        
      log.Printf ("...choose configuration #%u (RGB/A: %u/%u, D/S: %u/%u, Samples: %u)",
        glx_config, in_desc.frame_buffer.color_bits, in_desc.frame_buffer.alpha_bits, in_desc.frame_buffer.depth_bits,
        in_desc.frame_buffer.stencil_bits, in_desc.samples_count);
        
      log.Printf ("...create window surface");
        
        //�������� ����������� ���������

      glx_surface = glxCreateWindowSurface (glx_display, glx_config, (NativeWindowType)output->GetWindowHandle (), 0);
      
      if (!glx_surface)
        raise_error ("::glxCreateWindowSurface");

        //���������� ����������� ����������        

      desc = in_desc;

      desc.frame_buffer.width        = GetSurfaceAttribute (glx_WIDTH);
      desc.frame_buffer.height       = GetSurfaceAttribute (glx_HEIGHT);
      desc.frame_buffer.color_bits   = GetConfigAttribute (glx_BUFFER_SIZE);
      desc.frame_buffer.alpha_bits   = GetConfigAttribute (glx_ALPHA_SIZE);
      desc.frame_buffer.depth_bits   = GetConfigAttribute (glx_DEPTH_SIZE);
      desc.frame_buffer.stencil_bits = GetConfigAttribute (glx_STENCIL_SIZE);
      desc.samples_count             = GetConfigAttribute (glx_SAMPLES);
      desc.buffers_count             = 1;
      desc.fullscreen                = false;
      
        //��������� ������� ������� ������
        
      try
      {
        properties.AddProperty ("glx_vendor",      GetglxString (glx_VENDOR));
        properties.AddProperty ("glx_version",     GetglxString (glx_VERSION));
        properties.AddProperty ("glx_extensions",  GetglxString (glx_EXTENSIONS));
        properties.AddProperty ("glx_client_apis", GetglxString (glx_CLIENT_APIS));
      }
      catch (...)
      {
        //���������� ��� ������ ������� glx �� �������� ���������� ��� ������
        //(����� ���� glx ��� bada)
      }
    }
    catch (...)
    {
      if (glx_surface)
        glxDestroySurface (glx_display, glx_surface);

      throw;
    }
  }
  
///����������
  ~Impl ()
  {
    glxDestroySurface (glx_display, glx_surface);
  }
  
///��������� ��������
  glxint GetConfigAttribute (glxint attribute)
  {
    glxint value = 0;
    
    if (!glxGetConfigAttrib (glx_display, glx_config, attribute, &value))
      raise_error ("::glxGetConfigAttrib");
      
    return value;
  }
  
///��������� �������� �������� ����������� ���������
  glxint GetSurfaceAttribute (glxint attribute)
  {
    glxint value = 0;
    
    if (!glxQuerySurface (glx_display, glx_surface, attribute, &value))
      raise_error ("::glxQuerySurface");
      
    return value;
  }
  
///��������� ���������� �������� glx
  const char* GetglxString (glxint name)
  {
    const char* value = glxQueryString (glx_display, name);
    
    if (!value)
      raise_error ("::glxQueryString");
      
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
    exception.touch ("render::low_level::opengl::glx::PrimarySwapChain::PrimarySwapChain");
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
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::PrimarySwapChain::SetFullscreenState");
}

bool PrimarySwapChain::GetFullscreenState ()
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::PrimarySwapChain::GetFullscreenState");
}

/*
    ����� �������� ������� ������ � ��������� ������
*/

void PrimarySwapChain::Present ()
{
  try
  {
    if (!glxSwapBuffers (impl->glx_display, impl->glx_surface))
      raise_error ("::glxSwapBuffers");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::PrimarySwapChain::Present");
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
    ��������� glx ���������� ������� ������
*/

glxDisplay PrimarySwapChain::GetglxDisplay ()
{
  return impl->output->GetglxDisplay ();
}

glxConfig PrimarySwapChain::GetglxConfig ()
{
  return impl->glx_config;
}

glxSurface PrimarySwapChain::GetglxSurface ()
{
  return impl->glx_surface;
}
