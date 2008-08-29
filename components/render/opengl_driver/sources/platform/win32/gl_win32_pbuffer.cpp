#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl::windows;
using namespace common;

/*
    �������� ���������� PBuffer
*/

typedef xtl::com_ptr<PrimarySwapChain> SwapChainPtr;

struct PBuffer::Impl
{
  SwapChainPtr               primary_swap_chain;  //�������� ������� ������
  SwapChainDesc              desc;                //���������� ������
  HPBUFFERARB                pbuffer;             //���������� PBuffer'�
  HDC                        output_context;      //�������� ���������� ������
  int                        pixel_format_index;  //������ ������� ��������
  bool                       create_largest_flag; //����, ��������������� � ������������� �������� ����������� ���������� pbuffer'�
  xtl::auto_connection       cds_connection;      //���������� � ��������, ����������� �� ��������� �����-������
  const WglExtensionEntries* wgl_extension_entries; //������� WGL-����������  

///����������� / ����������
  Impl (PrimarySwapChain* swap_chain)
    : primary_swap_chain (swap_chain),
      pbuffer (0),
      output_context (0)
  {
    static const char* METHOD_NAME = "render::low_level::opengl::windows::PBuffer::Impl::Impl";

    if (!swap_chain)
      throw xtl::make_null_argument_exception (METHOD_NAME, "swap_chain");

    cds_connection = swap_chain->RegisterDisplayModeChangeHandler (xtl::bind (&PBuffer::Impl::OnDisplayModeChange, this));
    wgl_extension_entries = &swap_chain->GetWglExtensionEntries ();
    
    pixel_format_index = swap_chain->GetPixelFormat ();

    if (!has_extension (get_wgl_extensions_string (*wgl_extension_entries, swap_chain->GetDC ()).c_str (), "WGL_ARB_pbuffer"))
      throw xtl::format_not_supported_exception (METHOD_NAME, "PBuffer does not supported");        

    swap_chain->GetDesc (desc);

    desc.vsync         = false;
    desc.fullscreen    = false;
    desc.window_handle = 0;
  }

  ~Impl ()
  {
    Destroy ();
  }

///��������� ������� ����� �����-������
  void OnDisplayModeChange ()
  {
      //�������� ��������� P-buffer

    int is_lost = 0;

    if (!wgl_extension_entries->QueryPbufferARB (pbuffer, WGL_PBUFFER_LOST_ARB, &is_lost))
      raise_error ("wglQueryPBufferARB");

      //���� ����� ������� ��� ���������� - ���������� ���

    if (is_lost)    
    {
      Destroy ();
      Create ();
    }
  }

///�������� / ����������� ������
  void Create ()
  {
    try
    {
      HDC primary_device_context = primary_swap_chain->GetDC ();
        
        //����� ����������� ������� ��������
        
      int pixel_format = primary_swap_chain->GetPixelFormat ();
      
        //�������� PBuffer

      int  pbuffer_attributes []  = {0, 0, WGL_PBUFFER_LARGEST_ARB, 1, 0, 0};
      int* pbuffer_attributes_ptr = create_largest_flag ? pbuffer_attributes + 2 : pbuffer_attributes;

      pbuffer = wgl_extension_entries->CreatePbufferARB (primary_device_context, pixel_format, desc.frame_buffer.width, desc.frame_buffer.height,
                                                         pbuffer_attributes_ptr);

      if (!pbuffer)
        raise_error ("wglCreatePbufferARB");
        
        //��������� ��������� ������
        
      output_context = wgl_extension_entries->GetPbufferDCARB (pbuffer);
      
      if (!output_context)
        raise_error ("wglGetPbufferDCARB");            
        
        //��������� �������� ���������� PBuffer'�
        
      int width, height;

      if (!wgl_extension_entries->QueryPbufferARB (pbuffer, WGL_PBUFFER_WIDTH_ARB, &width) || 
          !wgl_extension_entries->QueryPbufferARB (pbuffer, WGL_PBUFFER_HEIGHT_ARB, &height))
        raise_error ("wglQueryPBufferARB");

      desc.frame_buffer.width  = (size_t)width;
      desc.frame_buffer.height = (size_t)height;
    }
    catch (...)
    {
      if (output_context) wgl_extension_entries->ReleasePbufferDCARB (pbuffer, output_context);
      if (pbuffer)        wgl_extension_entries->DestroyPbufferARB (pbuffer);
      
      output_context = 0;
      pbuffer = 0;

      throw;
    }
  }

  void Destroy ()
  {
    try
    {
      wgl_extension_entries->ReleasePbufferDCARB (pbuffer, output_context);
      wgl_extension_entries->DestroyPbufferARB   (pbuffer);
    }
    catch (...)
    {
      //��������� ��� ����������
    }
    
    output_context = 0;  
    pbuffer = 0;
  }
};

/*
    ����������� / ����������
*/

PBuffer::PBuffer (PrimarySwapChain* swap_chain, size_t width, size_t height)
  : impl (new Impl (swap_chain))
{
  impl->create_largest_flag = false;

    //���������� �����������

  impl->desc.frame_buffer.width  = width;
  impl->desc.frame_buffer.height = height;

    //�������� ������

  impl->Create ();
}

PBuffer::PBuffer (PrimarySwapChain* swap_chain)
  : impl (new Impl (swap_chain))
{
  impl->create_largest_flag = true;

    //�������� ������

  impl->Create ();
}

PBuffer::~PBuffer ()
{
}

/*
    ��������� �����������
*/

void PBuffer::GetDesc (SwapChainDesc& out_desc)
{
  out_desc = impl->desc;
}

/*
    ��������� ���������� ������ � ������������ �������� ������� ����������
*/

IOutput* PBuffer::GetContainingOutput ()
{
  return impl->primary_swap_chain->GetContainingOutput ();
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
    ��������� ��������
*/

IAdapter* PBuffer::GetAdapter ()
{
  return impl->primary_swap_chain->GetAdapter ();
}

/*
   ��������� ���������� ISwapChainImpl
*/

//��������� ���������� ��������
Adapter* PBuffer::GetAdapterImpl ()
{
  return impl->primary_swap_chain->GetAdapterImpl ();
}

//��������� ��������� ���������� ������
HDC PBuffer::GetDC ()
{
  return impl->output_context;
}

//��������� ������� �������� ������� ������
int PBuffer::GetPixelFormat ()
{
  return impl->pixel_format_index;
}

//���� �� ������������ �������������
bool PBuffer::HasVSync ()
{
  return false;
}

//��������� ����� �����
const WglExtensionEntries& PBuffer::GetWglExtensionEntries ()
{
  return *impl->wgl_extension_entries;
}
