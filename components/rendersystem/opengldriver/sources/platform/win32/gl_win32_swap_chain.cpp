#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    ����������� / ����������
*/

SwapChain::SwapChain (OutputManager&, const SwapChainDesc& in_desc)
  : output (0), output_window ((HWND)in_desc.window_handle)
{
  if (!output_window)
    RaiseNullArgument ("render::low_level::opengl::SwapChain::SwapChain", "swap_chain_desc.window_handle");

    //��������� ��������� ���������� ������
    
  output_device_context = ::GetDC (output_window);
  
  if (!output_device_context)
    raise_error ("GetDC");

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

    set_pixel_format (output_device_context, in_desc);
    
      //��������� �������������� ������� ��������
      
    get_pixel_format (output_device_context, desc);
    
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
    ReleaseDC (output_window, output_device_context);
    throw;
  }
}

SwapChain::~SwapChain ()
{
  ReleaseDC (output_window, output_device_context);
}

/*
    ��������� �����������
*/

void SwapChain::GetDesc (SwapChainDesc& out_desc)
{
  out_desc = desc;
}

/*
    ��������� ���������� ������
*/

IOutput* SwapChain::GetOutput ()
{
  RaiseNotImplemented ("render::low_level::opengl::SwapChain::GetOutput");
  return get_pointer (output);
}

/*
    ����� �������� ������� ������ � ��������� ������
*/

void SwapChain::Present ()
{
  if (!SwapBuffers (output_device_context))
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
  return new SwapChain (output_manager, swap_chain_desc);
}

}

}

}
