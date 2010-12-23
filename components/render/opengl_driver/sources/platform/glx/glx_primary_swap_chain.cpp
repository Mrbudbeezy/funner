#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

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
    , display ((Display*)syslib::x11::DisplayManager::DisplayHandle ())
  {
    try
    {
        //�������� ����������� ������ GLX
/*        
      int error_base, event_base;        
        
      if (!glXQueryExtension (display, &error_base, &event_base ) )
      {
          fprintf(stderr, "glxsimple: %s\n", "X server has no OpenGL GLX extension");
          exit(1);
      }
        
      
        //����� ������������

      GLint config_attributes [CONFIG_MAX_ATTRIBUTES], *attr = config_attributes;
      
      *attr++ = GL_BUFFER_SIZE;
      *attr++ = in_desc.frame_buffer.color_bits;
      *attr++ = GLX_ALPHA_SIZE;
      *attr++ = in_desc.frame_buffer.alpha_bits;      
      *attr++ = GLX_DEPTH_SIZE;
      *attr++ = in_desc.frame_buffer.depth_bits;
//      *attr++ = GL_STENCIL_SIZE; //for tests only!!!!!!!
//      *attr++ = in_desc.frame_buffer.stencil_bits;
      *attr++ = GL_SAMPLES;
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
      
      *attr++ = GLX_SURFACE_TYPE;
      *attr++ = GLX_WINDOW_BIT;
      *attr++ = GL_NONE;

      GLint configs_count = 0;

      if (!glxChooseFBConfig (display, config_attributes, &glx_config, 1, &configs_count))
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

      desc.frame_buffer.width        = GetSurfaceAttribute (GL_WIDTH);
      desc.frame_buffer.height       = GetSurfaceAttribute (GL_HEIGHT);
      desc.frame_buffer.color_bits   = GetConfigAttribute (GL_BUFFER_SIZE);
      desc.frame_buffer.alpha_bits   = GetConfigAttribute (GL_ALPHA_SIZE);
      desc.frame_buffer.depth_bits   = GetConfigAttribute (GL_DEPTH_SIZE);
      desc.frame_buffer.stencil_bits = GetConfigAttribute (GL_STENCIL_SIZE);
      desc.samples_count             = GetConfigAttribute (GL_SAMPLES);
      desc.buffers_count             = 1;
      desc.fullscreen                = false;
      
        //��������� ������� ������� ������
        
      properties.AddProperty ("GL_vendor",      GetGLString (GL_VENDOR));
      properties.AddProperty ("GL_version",     GetGLString (GL_VERSION));
      properties.AddProperty ("GL_extensions",  GetGLString (GL_EXTENSIONS));
      properties.AddProperty ("GL_client_apis", GetGLString (GL_CLIENT_APIS));*/
    }
    catch (...)
    {
/*      if (glx_surface)
        glxDestroySurface (glx_display, glx_surface);*/

      throw;
    }
  }
  
///����������
  ~Impl ()
  {
//    glxDestroySurface (glx_display, glx_surface);
  }
  
///��������� ��������
  GLint GetConfigAttribute (GLint attribute)
  {
    GLint value = 0;
    
//    if (!glxGetConfigAttrib (glx_display, glx_config, attribute, &value))
 //     raise_error ("::glxGetConfigAttrib");
      
    return value;
  }
  
///��������� �������� �������� ����������� ���������
  GLint GetSurfaceAttribute (GLint attribute)
  {
    GLint value = 0;
    
//    if (!glxQuerySurface (glx_display, glx_surface, attribute, &value))
//      raise_error ("::glxQuerySurface");
      
    return value;
  }
  
///��������� ���������� �������� glx
  const char* GetGLString (GLint name)
  {
/*    const char* value = glxQueryString (glx_display, name);
    
    if (!value)
      raise_error ("::glxQueryString");
      
    return value;*/
    return "";
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
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::PrimarySwapChain::GetContainingOutput");
//  return impl->output.get ();
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
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::PrimarySwapChain::Present");
/*  try
  {
    if (!glxSwapBuffers (impl->glx_display, impl->glx_surface))
      raise_error ("::glxSwapBuffers");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::PrimarySwapChain::Present");
    throw;
  }*/
}

/*
    ������ ������� ���������� ������
*/

IPropertyList* PrimarySwapChain::GetProperties ()
{
  return &impl->properties;
}
