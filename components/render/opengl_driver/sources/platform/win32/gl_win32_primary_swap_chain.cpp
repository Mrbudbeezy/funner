#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl::windows;

namespace
{

//�������� ������������ �������� �����������
HWND check_output_window (const SwapChainDesc& desc)
{
  HWND window = (HWND)desc.window_handle;
  
  if (!window)
    throw xtl::make_null_argument_exception ("render::low_level::opengl::windows::PrimarySwapChain::PrimarySwapChain", "swap_chain_desc.window_handle");
    
  return window;
}

}

/*
    �������� ���������� PrimarySwapChain
*/

typedef xtl::com_ptr<Adapter> AdapterPtr;
typedef xtl::signal<void ()>  ChangeDisplayModeSignal;

struct PrimarySwapChain::Impl: private IWindowListener
{
  Log                     log;                     //��������
  AdapterPtr              adapter;                 //�������
  WglExtensionEntries     wgl_extension_entries;   //������� WGL-����������
  HWND                    output_window;           //���� ������
  HDC                     output_context;          //�������� ������
  int                     pixel_format_index;      //������ ������� �������� ���������� ������
  DummyWindow             listener_window;         //�������� ����
  SwapChainDesc           desc;                    //���������� ������� ������
  ChangeDisplayModeSignal cdm_signal;              //������, ����������� �� ��������� �����-������
  PropertyList            properties;              //�������� ������� ������
  bool                    need_twice_swap_buffers; //����� �� ������ ���������� ������ (��. ������� PrimarySwapChain::Present)

///�����������
  Impl (const SwapChainDesc& in_desc, const PixelFormatDesc& pixel_format)
    : adapter (pixel_format.adapter),
      output_window (check_output_window (in_desc)),
      output_context (0),
      pixel_format_index (0),
      listener_window (output_window, this),
      need_twice_swap_buffers (false)
  {
      //��������� ���������
      
    log.Printf ("...get device context");

    output_context = ::GetDC (output_window);

    if (!output_context)
      raise_error ("GetDC");

      //������������� ������� ����������

    if (pixel_format.wgl_extension_entries)
    {
      wgl_extension_entries = *pixel_format.wgl_extension_entries;
    }
    else
    {
      memset (&wgl_extension_entries, 0, sizeof wgl_extension_entries);
    }

      //��������� ��������� FullScreen

    if (in_desc.fullscreen)
    {
      log.Printf ("...set fullscreen mode");
      
      SetFullscreenState (true, in_desc.frame_buffer.color_bits);
    }

      //��������� ������� ������� ������

    stl::string extensions_string = get_wgl_extensions_string (wgl_extension_entries, output_context).c_str ();

    IPropertyList* adapter_properties = adapter->GetProperties ();        

    if (adapter_properties)
    {
      for (size_t i=0, count=adapter_properties->GetSize (); i<count; i++)
        if (!xtl::xstrcmp (adapter_properties->GetKey (i), "bugs"))
        {
          const char* bugs = adapter_properties->GetValue (i);

          if (*bugs)
          {
            extensions_string += ' ';
            extensions_string += bugs;
            
              //����������� ������������� ������� ��������� ������ ��� ������ �����������

            need_twice_swap_buffers = strstr (bugs, "GLBUG_swap_buffers_twice_call") != 0;
          }

          break;
        }
    }        

    properties.AddProperty ("gl_extensions", extensions_string.c_str ());

      //��������� �������� ������� �������� ��� ��������� ���������      
      
    log.Printf ("...set pixel format");    

    pixel_format_index = pixel_format.pixel_format_index;

    adapter->GetLibrary ().SetPixelFormat (output_context, pixel_format_index);

      //������������� ����������� ������� ������

    desc.frame_buffer.width        = GetDeviceCaps (output_context, HORZRES);
    desc.frame_buffer.height       = GetDeviceCaps (output_context, VERTRES);
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
  
///����������
  ~Impl ()
  {
    log.Printf ("...release device context");

    adapter->GetLibrary ().ReleasePixelFormat (output_context);
    ::ReleaseDC (output_window, output_context);

    log.Printf ("...release resources");
  }  
  
///��������� ��������� �������������� ������
  void SetFullscreenState (bool state, size_t color_bits)
  {
    if (GetFullscreenState () == state)
      return;

    Output* output = adapter->FindContainingOutput (output_window);    
    
    if (!output)
      return;      

    if (state)
    {
      if (!SetWindowPos (output_window, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE))
        raise_error ("SetWindowPos");

      OutputModeDesc mode_desc;

      RECT window_rect;

      if (!GetWindowRect (output_window, &window_rect))
        raise_error ("GetWindowRect");

      mode_desc.width        = window_rect.right - window_rect.left;
      mode_desc.height       = window_rect.bottom - window_rect.top;
      mode_desc.color_bits   = color_bits;
      mode_desc.refresh_rate = 0;

      output->SetCurrentMode (mode_desc);
    }
    else
    {
      output->RestoreDefaultMode ();
    }
  }

///��������� ��������� �������������� ������
  bool GetFullscreenState ()
  {
    Output* output = adapter->FindContainingOutput (output_window);

    if (!output)
      return false;

    OutputModeDesc mode_desc;
    
    output->GetCurrentMode (mode_desc);

    RECT window_rect;

    if (!GetWindowRect (output_window, &window_rect))
      raise_error ("GetWindowRect");

    return window_rect.left == 0 && window_rect.top == 0 && window_rect.right - window_rect.left == mode_desc.width &&
           window_rect.bottom - window_rect.top == mode_desc.height && desc.frame_buffer.color_bits >= mode_desc.color_bits;
  }  
  
///����� �������� ������� ������ � ��������� ������
  void Present ()
  {
    IAdapterLibrary& library = adapter->GetLibrary ();
    
    if (need_twice_swap_buffers)
    {
        //� ��������� ����������� OpenGL (��������, MSOGL) ����� ������ ����������� ����� ����� �������� ������

      library.SwapBuffers (output_context);

      need_twice_swap_buffers = false;
    }

    library.SwapBuffers (output_context);    
  }
  
///��������� ������� �������� ����
  void OnDestroy ()
  {
      //���� ������� �������� ������ �������� ���������� ������ ���� - ���������� ������� ��������

    if (adapter->GetLibrary ().GetCurrentDC () == output_context)
      adapter->GetLibrary ().MakeCurrent (0, 0);
  }

///��������� ������� ��������� �����-������
  void OnDisplayModeChange ()
  {
    try
    {
        //���������� �� ��������� �����-������

      cdm_signal ();
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

PrimarySwapChain::PrimarySwapChain (const SwapChainDesc& desc, const PixelFormatDesc& pixel_format)
{
  Log log;

  log.Printf ("...create primary swap chain (id=%u)", GetId ());

  impl = new Impl (desc, pixel_format);

  log.Printf ("...primary swap chain successfully created");  
}

PrimarySwapChain::~PrimarySwapChain ()
{ 
  try
  {
    Log log;

    log.Printf ("Destroy primary swap chain (id=%u)...", GetId ());

    if (GetFullscreenState () && impl->desc.fullscreen)
    {
      log.Printf ("...restore fullscreen mode");

      SetFullscreenState (false);
    }

    impl = 0;
    
    log.Printf ("...primary swap chain successfully destroyed");
  }
  catch (...)
  {
    //��������� ��� ����������
  }
}

/*
    ��������� ��������
*/

IAdapter* PrimarySwapChain::GetAdapter ()
{
  return impl->adapter.get ();
}

/*
    ��������� ���������� IDeviceContext
*/

//��������� ���������� ��������
Adapter* PrimarySwapChain::GetAdapterImpl ()
{
  return impl->adapter.get ();
}

//�������� ���������� ������
HDC PrimarySwapChain::GetDC ()
{
  return impl->output_context;
}

//��������� ������� �������� ������� ������
int PrimarySwapChain::GetPixelFormat ()
{
  return impl->pixel_format_index;
}

//���� �� ������������ �������������
bool PrimarySwapChain::HasVSync ()
{
  return impl->desc.vsync;
}

//��������� ������� WGL-����������
const WglExtensionEntries& PrimarySwapChain::GetWglExtensionEntries ()
{
  return impl->wgl_extension_entries;
}

/*
    ��������� �����������
*/

void PrimarySwapChain::GetDesc (SwapChainDesc& out_desc)
{
  out_desc = impl->desc;
}

/*
    ������ ������� ������� ������
*/

IPropertyList* PrimarySwapChain::GetProperties ()
{
  return &impl->properties;
}

/*
    ��������� ���������� ������ � ������������ �������� ������� ����������
*/

IOutput* PrimarySwapChain::GetContainingOutput ()
{
  return impl->adapter->FindContainingOutput (impl->output_window);
}

/*
    ����� �������� ������� ������ � ��������� ������
*/

void PrimarySwapChain::Present ()
{
  try
  {
    impl->Present ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::windows::PrimarySwapChain::Present");

    throw;
  }
}

/*
    ��������� / ������ ��������� full-screen mode
*/

void PrimarySwapChain::SetFullscreenState (bool state)
{
  impl->SetFullscreenState (state, impl->desc.frame_buffer.color_bits);
}

bool PrimarySwapChain::GetFullscreenState ()
{
  return impl->GetFullscreenState ();
}

/*
    �������� �� ������� ����������� �����-������
*/

xtl::connection PrimarySwapChain::RegisterDisplayModeChangeHandler (const EventHandler& handler)
{
  try
  {
    return impl->cdm_signal.connect (handler);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::windows::PrimarySwapChain::RegisterDisplayModeChangeHandler");
    throw;
  }
}
