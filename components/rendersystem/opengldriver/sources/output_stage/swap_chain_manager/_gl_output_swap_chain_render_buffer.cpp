#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
===================================================================================================
    SwapChainRenderBuffer
===================================================================================================
*/

/*
    �����������
*/

SwapChainRenderBuffer::SwapChainRenderBuffer (const FrameBufferManager& manager, RenderTargetType target_type)
  : RenderBuffer (manager.GetContextManager (), target_type),
    frame_buffer_manager (manager)
{
}

SwapChainRenderBuffer::SwapChainRenderBuffer (const FrameBufferManager& manager, const TextureDesc& desc)
  : RenderBuffer (manager.GetContextManager (), desc),
    frame_buffer_manager (manager)
{
}

/*
    ��������� ��������� ������ �����
*/

void SwapChainRenderBuffer::SetFrameBuffer (size_t context_id, ISwapChain* swap_chain, GLenum buffer_type)
{
  frame_buffer_manager.SetFrameBuffer (context_id, swap_chain, buffer_type);
  frame_buffer_manager.SetFrameBufferActivity (buffer_type != GL_NONE, context_id != 0);
}

/*
===================================================================================================
    SwapChainColorBuffer
===================================================================================================
*/

/*
    �����������
*/

SwapChainColorBuffer::SwapChainColorBuffer
 (const FrameBufferManager& manager,
  ISwapChain*               in_swap_chain,
  size_t                    in_buffer_index)
  : SwapChainRenderBuffer (manager, RenderTargetType_Color),
    swap_chain (in_swap_chain),
    buffer_index (in_buffer_index)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainColorBuffer::SwapChainColorBuffer";
  
    //�������� ������������ ���������� ����������
  
  if (!swap_chain)
    RaiseNullArgument (METHOD_NAME, "swap_chain");

  SwapChainDesc swap_chain_desc;

  swap_chain->GetDesc (swap_chain_desc);

  if (buffer_index >= swap_chain_desc.buffers_count)
    RaiseOutOfRange (METHOD_NAME, "buffer_index", buffer_index, swap_chain_desc.buffers_count);
    
    //��������� �������� ������ �����

  switch (buffer_index)
  {
    case 0:  buffer_type = GL_FRONT; break;
    default: buffer_type = GL_BACK;  break;
  }
  
    //��������� �������� ������
    
  SetSize (swap_chain_desc.frame_buffer.width, swap_chain_desc.frame_buffer.height);
}

SwapChainColorBuffer::SwapChainColorBuffer (const FrameBufferManager& manager, ISwapChain* in_swap_chain, const TextureDesc& desc)
  : SwapChainRenderBuffer (manager, desc),
    swap_chain (in_swap_chain),
    buffer_index (1),
    buffer_type (GL_BACK)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainColorBuffer::SwapChainColorBuffer";

    //�������� ������������ ���������� ����������

  if (!swap_chain)
    RaiseNullArgument (METHOD_NAME, "swap_chain");   
}

/*
    ��������� � �������� OpenGL
*/

void SwapChainColorBuffer::Bind ()
{
  try    
  {
    SetFrameBuffer (0, swap_chain.get (), buffer_type);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::SwapChainColorBuffer::Bind");

    throw;
  }
}

/*
===================================================================================================
    SwapDepthStencilBuffer
===================================================================================================
*/

/*
    ����������� / ����������
*/

SwapChainDepthStencilBuffer::SwapChainDepthStencilBuffer (const FrameBufferManager& manager, ISwapChain* swap_chain)
  : SwapChainRenderBuffer (manager, RenderTargetType_DepthStencil)
{
  try
  {
      //�������� ������������ ���������� ����������
    
    if (!swap_chain)
      RaiseNullArgument ("", "swap_chain");
      
      //��������� �������� ������

    SwapChainDesc swap_chain_desc;

    swap_chain->GetDesc (swap_chain_desc);
    
    SetSize (swap_chain_desc.frame_buffer.width, swap_chain_desc.frame_buffer.height);    

      //�������� ���������

    context_id = GetContextManager ().CreateContext (swap_chain);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::SwapChainDepthStencilBuffer::SwapChainDepthStencilBuffer");
    throw;
  }
}

SwapChainDepthStencilBuffer::SwapChainDepthStencilBuffer (const FrameBufferManager& manager, ISwapChain* swap_chain, const TextureDesc& desc)
  : SwapChainRenderBuffer (manager, desc)
{
  try
  {
      //�������� ���������� ����������

    if (!swap_chain)
      RaiseNullArgument ("", "swap_chain");

    SwapChainDesc swap_chain_desc;

    swap_chain->GetDesc (swap_chain_desc);

    if (desc.width > swap_chain_desc.frame_buffer.width)
      RaiseNotSupported ("", "Depth-stencil buffer width=%u is greater than swap-chain desc.width=%u", desc.width,
        swap_chain_desc.frame_buffer.width);

    if (desc.height > swap_chain_desc.frame_buffer.height)
      RaiseNotSupported ("", "Depth-stencil buffer height=%u is greater than swap-chain desc.height=%u", desc.height,
        swap_chain_desc.frame_buffer.height);

      //�������� ���������  

    context_id = GetContextManager ().CreateContext (swap_chain);    
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::SwapChainDepthStencilBuffer::SwapChainDepthStencilBuffer");
    throw;
  }
}

SwapChainDepthStencilBuffer::~SwapChainDepthStencilBuffer ()
{
  GetContextManager ().DeleteContext (context_id);
}

   
/*
    ��������� � �������� OpenGL
*/

void SwapChainDepthStencilBuffer::Bind ()
{
  try
  {
    SetFrameBuffer (context_id, 0, 0);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::SwapChainDepthStencilBuffer::Bind");
    
    throw;
  }  
}
