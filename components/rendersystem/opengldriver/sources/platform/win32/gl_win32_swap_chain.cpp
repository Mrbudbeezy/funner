#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    ����������� / ����������
*/

SwapChain::SwapChain (const DeviceContextPtr& in_device_context, IOutput* in_output, const SwapChainDesc& in_desc)
  : device_context (in_device_context), output (in_output)
{
  try
  {
    ThreadLock lock;
    
      //������������� � ��������� ��������� WGLEW

    try
    {
      init_wglew_context       (in_desc, &wglew_context);
      set_current_glew_context (0, &wglew_context);
    }
    catch (std::exception& exception)
    {
          //�������� printf �� ����� � ���-�����!!!!!
      printf ("SwapChain::SwapChain: Error at initialize WGLEWContext. %s\n", exception.what ());      
    }
    catch (...)
    {
      printf ("SwapChain::SwapChain: Unknown exception at initialize WGLEWContext\n");
    }    

      //��������� ������� ��������

    set_pixel_format (GetDC (), in_desc); //???
    
      //��������� �������������� ������� ��������
      
    get_pixel_format (GetDC (), desc);
    
      //����������� ����������� �����
    
    desc.vsync         = in_desc.vsync;
    desc.window_handle = in_desc.window_handle;

      //�������� ���������/������ ��������� FullScreen-mode!!!       

      //������ �������� ��������� WGLEW

    set_current_glew_context (0, 0);
  }
  catch (...)
  {
    set_current_glew_context (0, 0);
    throw;
  }
}

SwapChain::~SwapChain ()
{
}

/*
    ��������� �����������
*/

void SwapChain::GetDesc (SwapChainDesc& out_desc)
{
  out_desc = desc;
}

/*
    ��������� ���������� ������ � ������������ �������� ������� ����������
*/

IOutput* SwapChain::GetContainingOutput ()
{
  return get_pointer (output);
}

/*
    ����� �������� ������� ������ � ��������� ������
*/

void SwapChain::Present ()
{
  if (!SwapBuffers (device_context->GetDC ()))
    raise_error (format ("render::low_level::opengl::SwapChain::Present(device-name='%s')", output->GetName ()).c_str ());
}

/*
    ��������� / ������ ��������� full-screen mode
*/

void SwapChain::SetFullscreenState (bool state)
{
  RaiseNotImplemented ("render::low_level::opengl::SwapChain::SetFullscreenState");
}

bool SwapChain::GetFullscreenState ()
{
  RaiseNotImplemented ("render::low_level::opengl::SwapChain::SetFullscreenState");
  return false;
}

/*
    �������� ������� ������
*/

namespace render
{

namespace low_level
{

namespace opengl
{

ISwapChain* create_swap_chain (OutputManager& output_manager, const SwapChainDesc& swap_chain_desc)
{
  HWND window = (HWND)swap_chain_desc.window_handle;
  
  if (!window)
    RaiseNullArgument ("render::low_level::opengl::create_swap_chain", "swap_chain_desc.window_handle");

  IOutput* output = output_manager.FindContainingOutput ((void*)window);

  if (!output)
  {
    char title [128];

    GetWindowText (window, title, sizeof (title));

    RaiseInvalidOperation ("render::low_level::opengl::create_swap_chain", "Can not find containing output for window '%s'", title);
  }

  return new SwapChain (DeviceContextPtr (new WindowDeviceContext (window)), output, swap_chain_desc);
}

ISwapChain* create_swap_chain (IOutput* output, const SwapChainDesc& swap_chain_desc)
{
  return new SwapChain (DeviceContextPtr (new OutputDeviceContext (output)), output, swap_chain_desc);
}

}

}

}
