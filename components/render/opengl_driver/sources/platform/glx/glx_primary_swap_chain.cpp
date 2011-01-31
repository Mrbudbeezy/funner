#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

/*
    �������� ���������� ��������� ������� ������
*/

typedef xtl::com_ptr<Adapter> AdapterPtr;
typedef Output::Pointer       OutputPtr;

struct PrimarySwapChain::Impl
{
  Log                 log;                     //��������
  AdapterPtr          adapter;                 //�������, �������� ����������� ����������
  AdapterLibraryPtr   library;                 //���������� ��������
  GlxExtensionEntries glx_extension_entries;   //������� WGL-����������
  int                 pixel_format_index;      //������ ������� �������� ���������� ������
  SwapChainDesc       desc;                    //���������� ������� ������
  Display*            display;                 //���������� � ��������
  Window              window;                  //����
  GLXFBConfig         glx_fb_config;           //������������ ������ �����
  PropertyList        properties;              //�������� ������� ������

///�����������
  Impl (const SwapChainDesc& in_desc, const PixelFormatDesc& pixel_format)
    : adapter (pixel_format.adapter)
    , library (&adapter->GetLibrary ())
    , pixel_format_index (pixel_format.pixel_format_index)
    , display ((Display*)syslib::x11::DisplayManager::DisplayHandle ())
    , window  ((Window)in_desc.window_handle)
  {
    try
    {
        //������������� ������� ����������

      if (pixel_format.glx_extension_entries)
      {
        glx_extension_entries = *pixel_format.glx_extension_entries;
      }
      else
      {
        memset (&glx_extension_entries, 0, sizeof glx_extension_entries);
      }

        //��������� ��������� FullScreen

      if (in_desc.fullscreen)
      {
        log.Printf ("...set fullscreen mode");
        
        SetFullscreenState (true);
      }
      
        //������������� ������������ ������ �����
        
      glx_fb_config = pixel_format.config;
            
        //������������� ����������� ������� ������
        
      desc.frame_buffer.width        = XWidthOfScreen (get_screen (window));
      desc.frame_buffer.height       = XHeightOfScreen (get_screen (window));
      desc.frame_buffer.color_bits   = pixel_format.color_bits;
      desc.frame_buffer.alpha_bits   = pixel_format.alpha_bits;
      desc.frame_buffer.depth_bits   = pixel_format.depth_bits;
      desc.frame_buffer.stencil_bits = pixel_format.stencil_bits;
      desc.samples_count             = pixel_format.samples_count;
      desc.buffers_count             = pixel_format.buffers_count;
      desc.swap_method               = pixel_format.swap_method;
      desc.fullscreen                = in_desc.fullscreen;
      desc.vsync                     = in_desc.vsync;
      desc.window_handle             = in_desc.window_handle;
    }
    catch (...)
    {
      throw;
    }
  }
  
///����������
  ~Impl ()
  {
    log.Printf ("...release resources");
  }
  
///��������� ���������� ������ � ������������ �������� ������� ����������
  IOutput* GetContainingOutput ()
  {
    return adapter->GetOutput (window).get ();
  }
  
  void SetFullscreenState (bool state)
  {
//    throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::PrimarySwapChain::impl::SetFullscreenState");

    if (GetFullscreenState () == state)
      return;
      
    Output* output = adapter->GetOutput (window).get ();
    
    if (!output)
      return;      
      
    if (state)
    {
      DisplayLock lock (display);
      
      int event_base;
      int error_base;
      
      // ������ ���������� XF86VidMode
      
      if (XF86VidModeQueryExtension (display, &event_base, &error_base) == 0)
        throw xtl::format_operation_exception ("render::low_level::opengl::glx::PrimarySwapChain::impl::SetFullscreenState",
          "XF86VidModeQueryExtension missing");
          
      // ����������� ������ ���� �������
      
      int best_mode     = 0;
      int mode_num      = 0;
      int screen_number = get_screen_number (window);
      
      XF86VidModeModeInfo **modes;
      
      XF86VidModeGetAllModeLines (display, screen_number, &mode_num, &modes);
      
      // ���� ����� � ������ �����������
      
      for (int i=0; i<mode_num; i++)
      {                                                                    
        if ((modes [i]->hdisplay == desc.frame_buffer.width) && (modes [i]->vdisplay == desc.frame_buffer.height))
           best_mode = i;
      } 

      // ������������� � ����� fullscreen      

      XF86VidModeSwitchToMode (display, screen_number, modes [best_mode]);

      XF86VidModeSetViewPort (display, screen_number, 0, 0);            

      XFree (modes);
      
      // �������� ������� ������� ������
      
      OutputModeDesc mode_desc;
      
//      output->GetCurrentMode (mode_desc);

      mode_desc.width  = modes [best_mode]->hdisplay;
      mode_desc.height = modes [best_mode]->vdisplay;

//      output->SetCurrentMode (mode_desc);
    }
    else
    {
      /// TODO: ������� � ����� ����
    }
  }
  
  bool GetFullscreenState ()
  {
//    throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::PrimarySwapChain::impl::GetFullscreenState");

    Output* output = adapter->GetOutput (window).get ();

    if (!output)
      return false;

    OutputModeDesc mode_desc;
    
    output->GetCurrentMode (mode_desc);
    
    unsigned int screen_width  = WidthOfScreen  (get_screen (window));
    unsigned int screen_height = HeightOfScreen (get_screen (window));
    
    return mode_desc.width == screen_width && mode_desc.height == screen_height;
  }
  
  void Present ()
  {  
    try
    {
      library->SwapBuffers (display, window);
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("render::low_level::opengl::glx::PrimarySwapChain::impl::Present");
      throw;
    }
  }
};

/*
    ����������� / ����������
*/

PrimarySwapChain::PrimarySwapChain (const SwapChainDesc& sc_desc, const PixelFormatDesc& pf_desc)
{
  try
  {
    impl = new Impl (sc_desc, pf_desc);

    impl->log.Printf ("...primary swap chain (id=%u) successfully created", GetId ());
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

Adapter* PrimarySwapChain::GetAdapterImpl ()
{
  return &*impl->adapter;
}

/*
    ���������� ����������� ��� �������� ���������
*/

Display* PrimarySwapChain::GetDisplay ()
{
  return impl->display;
}

/*
    ���� ���������
*/

Window PrimarySwapChain::GetWindow ()
{
  return impl->window;
}

/*
    ��������� ������� GLX-����������
*/

const GlxExtensionEntries& PrimarySwapChain::GetGlxExtensionEntries ()
{
  return impl->glx_extension_entries;
}

/*
    ������������ ������ �����
*/

GLXFBConfig PrimarySwapChain::GetFBConfig ()
{
  return impl->glx_fb_config;
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
  return impl->GetContainingOutput ();
}

/*
    ��������� / ������ ��������� full-screen mode
*/

void PrimarySwapChain::SetFullscreenState (bool state)
{
  impl->SetFullscreenState (state);
}

bool PrimarySwapChain::GetFullscreenState ()
{
  return impl->GetFullscreenState ();
}

/*
    ����� �������� ������� ������ � ��������� ������
*/

void PrimarySwapChain::Present ()
{
  impl->Present ();
}

/*
    ������ ������� ���������� ������
*/

IPropertyList* PrimarySwapChain::GetProperties ()
{
  return &impl->properties;
}
