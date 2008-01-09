#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
===================================================================================================
    SwapChainColorBuffer
===================================================================================================
*/

/*
    �����������
*/

SwapChainColorBuffer::SwapChainColorBuffer (const ContextManager& manager, ISwapChain* in_swap_chain, size_t in_buffer_index)
  : ColorBuffer (manager),
    swap_chain (in_swap_chain),
    buffer_index (in_buffer_index)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainColorBuffer::SwapChainColorBuffer";

  if (!swap_chain)
    RaiseNullArgument (METHOD_NAME, "swap_chain");
    
  SwapChainDesc desc;

  swap_chain->GetDesc (desc);

  if (buffer_index >= desc.buffers_count)
    RaiseOutOfRange (METHOD_NAME, "buffer_index", buffer_index, desc.buffers_count);
    
  switch (buffer_index)
  {
    case 0:  buffer_type = GL_FRONT; break;
    default: buffer_type = GL_BACK;  break;
  }
}

/*
    ��������� �������� ������
*/

void SwapChainColorBuffer::GetSize (size_t& width, size_t& height)
{
  SwapChainDesc desc;

  swap_chain->GetDesc (desc);
  
  width  = desc.frame_buffer.width;
  height = desc.frame_buffer.height;
}
    
/*
    ��������� � �������� OpenGL
*/

void SwapChainColorBuffer::Bind ()
{
  RaiseNotImplemented ("render::low_level::opengl::SwapChainColorBuffer::Bind");
}

/*
===================================================================================================
    SwapDepthStencilBuffer
===================================================================================================
*/

/*
    ����������� / ����������
*/

SwapChainDepthStencilBuffer::SwapChainDepthStencilBuffer (const ContextManager& manager, ISwapChain* in_swap_chain)
  : DepthStencilBuffer (manager),
    swap_chain (in_swap_chain)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainDepthStencilBuffer::SwapChainDepthStencilBuffer";

  if (!swap_chain)
    RaiseNullArgument (METHOD_NAME, "swap_chain");

  context_id = GetContextManager ().CreateContext (swap_chain.get ());
}

SwapChainDepthStencilBuffer::~SwapChainDepthStencilBuffer ()
{
  GetContextManager ().DeleteContext (context_id);
}

/*
    ��������� �������� ������
*/

void SwapChainDepthStencilBuffer::GetSize (size_t& width, size_t& height)
{
  SwapChainDesc desc;

  swap_chain->GetDesc (desc);
  
  width  = desc.frame_buffer.width;
  height = desc.frame_buffer.height;
}
    
/*
    ��������� � �������� OpenGL
*/

void SwapChainDepthStencilBuffer::Bind ()
{
  RaiseNotImplemented ("render::low_level::opengl::SwapChainDepthStencilBuffer::Bind");
}
