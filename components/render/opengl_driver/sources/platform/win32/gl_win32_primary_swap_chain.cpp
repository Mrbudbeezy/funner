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

/*
    �������� ������� ������
*/

class SwapChainProperyList: virtual public IPropertyList
{
  public:
///�����������
    SwapChainProperyList (HDC dc, const WglExtensionEntries* in_wgl_extension_entries)
      : device_context (dc),
        wgl_extension_entries (in_wgl_extension_entries) {}  

///���������� �������
    size_t GetSize () { return 1; }

///��������� �����
    const char* GetKey (size_t index)
    {
      if (index)
        throw xtl::make_range_exception ("render::low_level::opengl::windows::SwapChainProperyList::GetKey", "index", index, 1u);

      return "gl_extensions";
    }
    
///��������� ��������
    const char* GetValue (size_t index)
    {
      if (index)
        throw xtl::make_range_exception ("render::low_level::opengl::windows::SwapChainProperyList::GetValue", "index", index, 1u);
        
      if (wgl_extension_entries->GetExtensionsStringARB)
      {
        const char* result = wgl_extension_entries->GetExtensionsStringARB (device_context);
        
        return result ? result : "";
      }

      if (wgl_extension_entries->GetExtensionsStringEXT)
      {
        const char* result = wgl_extension_entries->GetExtensionsStringEXT ();

        return result ? result : "";
      }

      return "";
    }
    
  private:
    HDC                        device_context;
    const WglExtensionEntries* wgl_extension_entries;
};

}

/*
    �������� ���������� PrimarySwapChain
*/

typedef xtl::com_ptr<Adapter> AdapterPtr;
typedef xtl::signal<void ()>  ChangeDisplayModeSignal;

struct PrimarySwapChain::Impl: private IWindowListener
{
  AdapterPtr              adapter;                //�������
  WglExtensionEntries     wgl_extension_entries;  //������� WGL-����������
  HWND                    output_window;          //���� ������
  HDC                     output_context;         //�������� ������
  DummyWindow             listener_window;        //�������� ����
  SwapChainDesc           desc;                   //���������� ������� ������
  ChangeDisplayModeSignal cdm_signal;             //������, ����������� �� ��������� �����-������
  SwapChainProperyList    properties;             //�������� ������� ������

///�����������
  Impl (const SwapChainDesc& in_desc, const PixelFormatDesc& pixel_format)
    : adapter (pixel_format.adapter),
      output_window (check_output_window (in_desc)),
      output_context (::GetDC (output_window)),
      listener_window (output_window, this),
      properties (output_context, &wgl_extension_entries)
  {
      //�������� ������������ ��������

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
      SetFullscreenState (true, in_desc.frame_buffer.color_bits);

      //��������� �������� ������� �������� ��� ��������� ���������

    adapter->SetPixelFormat (output_context, pixel_format.pixel_format_index);

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
    desc.fullscreen                = GetFullscreenState ();
    desc.vsync                     = in_desc.vsync;
    desc.window_handle             = in_desc.window_handle;
  }    
  
///����������
  ~Impl ()
  {
    ::ReleaseDC (output_window, output_context);
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
  
///��������� ������� �������� ����
  void OnDestroy ()
  {
      //���� ������� �������� ������ �������� ���������� ������ ���� - ���������� ������� ��������

    if (adapter->GetCurrentDC () == output_context)
      adapter->MakeCurrent (0, 0);
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
  : impl (new Impl (desc, pixel_format))
{
}

PrimarySwapChain::~PrimarySwapChain ()
{ 
  try
  {
    if (GetFullscreenState () && impl->desc.fullscreen)
      SetFullscreenState (false);
  }
  catch (...)
  {
    //��������� ��� ����������
  }
}

/*
    ��������� ��������
*/

Adapter* PrimarySwapChain::GetAdapterImpl ()
{
  return impl->adapter.get ();
}

IAdapter* PrimarySwapChain::GetAdapter ()
{
  return impl->adapter.get ();
}

/*
    ��������� ���������� IDeviceContext
*/

//�������� ���������� ������
HDC PrimarySwapChain::GetDC ()
{
  return impl->output_context;
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
    impl->adapter->SwapBuffers (impl->output_context);
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
