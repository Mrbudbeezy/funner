#include "shared.h"

using namespace render::low_level;
using namespace render::mid_level::window_driver;

namespace
{

typedef xtl::com_ptr<ITexture> TexturePtr;
typedef xtl::com_ptr<IView>    ViewPtr;

}

/*
   ����� �����
*/

/*
   ����������� / ����������
*/

FrameBuffer::FrameBuffer (IDevice& device, ISwapChain& in_swap_chain)
  : swap_chain (&in_swap_chain)
{
  try
  {
      //��������� ����������� ������� ������

    SwapChainDesc swap_chain_desc;

    swap_chain->GetDesc (swap_chain_desc);
    
    TexturePtr texture;
    ViewPtr    view;
    ViewDesc   view_desc;

    memset (&view_desc, 0, sizeof view_desc);
    
      //�������� ������ �����      

    texture      = TexturePtr (device.CreateRenderTargetTexture (swap_chain.get (), swap_chain_desc.buffers_count > 1 ? 1 : 0), false);
    view         = ViewPtr (device.CreateView (texture.get (), view_desc), false);
    color_buffer = RenderTargetPtr (new RenderTarget (view.get (), RenderTargetType_Color), false);    
    
      //�������� ������ ������������� ���������

    texture              = TexturePtr (device.CreateDepthStencilTexture (swap_chain.get ()), false);
    view                 = ViewPtr (device.CreateView (texture.get (), view_desc), false);
    depth_stencil_buffer = RenderTargetPtr (new RenderTarget (view.get (), RenderTargetType_DepthStencil), false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::mid_level::window_driver::FrameBuffer::FrameBuffer");
    throw;
  }
}

FrameBuffer::~FrameBuffer ()
{
}

/*
   ��������� ������ ����� � ������ ������������� ���������
*/

render::mid_level::IRenderTarget* FrameBuffer::GetColorBuffer ()
{
  return color_buffer.get ();
}

render::mid_level::IRenderTarget* FrameBuffer::GetDepthStencilBuffer ()
{
  return depth_stencil_buffer.get ();
}

/*
   ��������� ������� / �������� ������� ������
*/

void FrameBuffer::SetSize (size_t width, size_t height)
{
  color_buffer->SetSize (width, height);
  depth_stencil_buffer->SetSize (width, height);
}

void FrameBuffer::SetViewportOffset (int x, int y)
{
  color_buffer->SetViewportOffset (x, y);
  depth_stencil_buffer->SetViewportOffset (x, y);
}
