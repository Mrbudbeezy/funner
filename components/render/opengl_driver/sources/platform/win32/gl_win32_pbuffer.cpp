#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    ����������� / ����������
*/

PBuffer::PBuffer (PrimarySwapChain* swap_chain, const SwapChainDesc& in_desc)
  : primary_swap_chain (swap_chain), desc (in_desc), pbuffer (0), output_context (0), create_largest_flag (false)
{
  Create ();
}

PBuffer::PBuffer (PrimarySwapChain* swap_chain)
  : primary_swap_chain (swap_chain), pbuffer (0), output_context (0), create_largest_flag (true)
{
    //���������� �����������

  swap_chain->GetDesc (desc);

    //�������� ������

  Create ();
}

PBuffer::~PBuffer ()
{
  Destroy ();
}

/*
    �������� / ����������� ������
*/

void PBuffer::Create ()
{
    //��������� ��������� WGLEW

  WGLEWScope wglew_scope (primary_swap_chain->GetWGLEWContext ());

  try
  {
    if (!WGLEW_ARB_pbuffer)
      raise_not_supported ("render::low_level::opengl::PBuffer::PBuffer", "PBuffer does not supported");
      
    HDC primary_device_context = primary_swap_chain->GetDC ();
      
      //����� ����������� ������� ��������
      
    int pixel_format = choose_pixel_format (primary_device_context, desc);
    
      //�������� PBuffer

    int  pbuffer_attributes []  = {0, 0, WGL_PBUFFER_LARGEST_ARB, 1, 0, 0};
    int* pbuffer_attributes_ptr = create_largest_flag ? pbuffer_attributes + 2 : pbuffer_attributes;

    pbuffer = wglCreatePbufferARB (primary_device_context, pixel_format, desc.frame_buffer.width, desc.frame_buffer.height,
                                   pbuffer_attributes_ptr);

    if (!pbuffer)
      raise_error ("wglCreatePbufferARB");
      
      //��������� ��������� ������
      
    output_context = wglGetPbufferDCARB (pbuffer);
    
    if (!output_context)
      raise_error ("wglGetPbufferDCARB");            
      
      //��������� ����������� ������� ��������

    get_pixel_format (output_context, pixel_format, desc);
 
      //��������� �������� ���������� PBuffer'�
      
    int width, height;

    if (!wglQueryPbufferARB (pbuffer, WGL_PBUFFER_WIDTH_ARB, &width) || !wglQueryPbufferARB (pbuffer, WGL_PBUFFER_HEIGHT_ARB, &height))
      raise_error ("wglQueryPBufferARB");

    desc.frame_buffer.width  = (size_t)width;
    desc.frame_buffer.height = (size_t)height;

      //����������� ����������� �����

    desc.vsync         = false;
    desc.fullscreen    = false;
    desc.window_handle = 0;
  }
  catch (...)
  {
    if (output_context) wglReleasePbufferDCARB (pbuffer, output_context);
    if (pbuffer)        wglDestroyPbufferARB (pbuffer);
    
    output_context = 0;
    pbuffer = 0;

    throw;
  }
}

void PBuffer::Destroy ()
{
  try
  {
    WGLEWScope wglew_scope (primary_swap_chain->GetWGLEWContext ());

    wglReleasePbufferDCARB (pbuffer, output_context);
    wglDestroyPbufferARB   (pbuffer);
  }
  catch (...)
  {
    //��������� ��� ����������
  }
  
  output_context = 0;  
  pbuffer = 0;
}


/*
    ��������� �����������
*/

void PBuffer::GetDesc (SwapChainDesc& out_desc)
{
  out_desc = desc;
}

/*
    ��������� ���������� ������ � ������������ �������� ������� ����������
*/

IOutput* PBuffer::GetContainingOutput ()
{
  return primary_swap_chain->GetContainingOutput ();
}

/*
    ����� �������� ������� ������ � ��������� ������
*/

void PBuffer::Present ()
{
}

/*
    ��������� / ������ ��������� full-screen mode
*/

void PBuffer::SetFullscreenState (bool)
{
}

bool PBuffer::GetFullscreenState ()
{
  return false;
}

/*
    �������� ���������� ������ / WGLEW ��������
*/

HDC PBuffer::GetDC ()
{
  return output_context;
/*  set_current_glew_context (0, primary_swap_chain->GetWGLEWContext ());

  int is_lost;
  
  if (!wglQueryPbufferARB (pbuffer, WGL_PBUFFER_LOST_ARB))
    raise_error ("wglQueryPBufferARB");

  if (is_lost)
  {
    Destroy ();
    Create ();
  }

  return output_context;*/
}

const WGLEWContext* PBuffer::GetWGLEWContext ()
{
  return primary_swap_chain->GetWGLEWContext ();
}
