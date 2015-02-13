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

SwapChainRenderBuffer::SwapChainRenderBuffer (const FrameBufferManagerPtr& manager, RenderTargetType target_type, ISwapChain* in_swap_chain)
  : RenderBuffer (manager->GetContextManager (), target_type)
  , frame_buffer_manager (manager)
  , swap_chain (in_swap_chain)
  , frame_buffer_id (0)
  , frame_buffer_cache_id (GetId ())
{
  try
  {
      //�������� ������������ ���������� ����������

    if (!in_swap_chain)
      throw xtl::make_null_argument_exception ("", "swap_chain");

    if (ISwapChainFrameBuffer* swap_chain_frame_buffer = dynamic_cast<ISwapChainFrameBuffer*> (in_swap_chain))
      frame_buffer_id = swap_chain_frame_buffer->GetFrameBufferId ();

    if (Object* object = dynamic_cast<Object*> (in_swap_chain))
      frame_buffer_cache_id = object->GetId ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::SwapChainRenderBuffer::SwapChainRenderBuffer(const FrameBufferManagerPtr&,RenderTargetType,ISwapChain*)");
    throw;
  }
}

SwapChainRenderBuffer::SwapChainRenderBuffer (const FrameBufferManagerPtr& manager, ISwapChain* in_swap_chain, const TextureDesc& desc)
  : RenderBuffer (manager->GetContextManager (), desc)
  , frame_buffer_manager (manager)
  , swap_chain (in_swap_chain)
  , frame_buffer_id (0)
  , frame_buffer_cache_id (GetId ())  
{
  try
  {
      //�������� ������������ ���������� ����������

    if (!in_swap_chain)
      throw xtl::make_null_argument_exception ("", "swap_chain");

    if (ISwapChainFrameBuffer* swap_chain_frame_buffer = dynamic_cast<ISwapChainFrameBuffer*> (in_swap_chain))
      frame_buffer_id = swap_chain_frame_buffer->GetFrameBufferId ();
    
    if (Object* object = dynamic_cast<Object*> (in_swap_chain))
      frame_buffer_cache_id = object->GetId ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::SwapChainRenderBuffer::SwapChainRenderBuffer(const FrameBufferManagerPtr&,ISwapChain*,const TextureDesc&)");
    throw;
  }
}

/*
    ��������� ��������� ������ �����
*/

void SwapChainRenderBuffer::SetFrameBuffer (GLenum buffer_type)
{
  try
  {
    frame_buffer_manager->SetFrameBuffer (swap_chain.get (), buffer_type, frame_buffer_id, frame_buffer_cache_id);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::SwapChainRenderBuffer::SetFrameBuffer");
    throw;
  }
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
 (const FrameBufferManagerPtr& manager,
  ISwapChain*                  swap_chain,
  unsigned int                 in_buffer_index)
  : SwapChainRenderBuffer (manager, RenderTargetType_Color, swap_chain),
    buffer_index (in_buffer_index),
    is_shadow (false)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainColorBuffer::SwapChainColorBuffer";
  
    //�������� ������������ ���������� ����������

  SwapChainDesc swap_chain_desc;

  swap_chain->GetDesc (swap_chain_desc);

  if (buffer_index >= swap_chain_desc.buffers_count)
    throw xtl::make_range_exception (METHOD_NAME, "buffer_index", buffer_index, swap_chain_desc.buffers_count);
    
    //��������� �������� ������ �����

  switch (buffer_index)
  {
    case 0:  buffer_type = GL_FRONT; break;
    default: buffer_type = GL_BACK;  break;
  }
  
    //��������� �������� ������
    
  SetSize (swap_chain_desc.frame_buffer.width, swap_chain_desc.frame_buffer.height);
}

SwapChainColorBuffer::SwapChainColorBuffer (const FrameBufferManagerPtr& manager, ISwapChain* swap_chain, const TextureDesc& desc)
  : SwapChainRenderBuffer (manager, swap_chain, desc),
    buffer_index (0),
    buffer_type (GL_FRONT),
    is_shadow (true)
{
    //��������� ������� ������
    
  SwapChainDesc swap_chain_desc;

  memset (&swap_chain_desc, 0, sizeof (swap_chain_desc));

  swap_chain->GetDesc (swap_chain_desc);

  if (swap_chain_desc.buffers_count > 1)
  {    
    buffer_index = 1;
    buffer_type  = GL_BACK;
  }
}

/*
    ��������� � �������� OpenGL
*/

void SwapChainColorBuffer::Bind ()
{
  try    
  {
    SetFrameBuffer (buffer_type);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::SwapChainColorBuffer::Bind");
    throw;
  }
}

/*
===================================================================================================
    SwapChainDepthStencilBuffer
===================================================================================================
*/

/*
    ����������� / ����������
*/

SwapChainDepthStencilBuffer::SwapChainDepthStencilBuffer (const FrameBufferManagerPtr& manager, ISwapChain* swap_chain)
  : SwapChainRenderBuffer (manager, RenderTargetType_DepthStencil, swap_chain)
{
  try
  {
      //��������� �������� ������

    SwapChainDesc swap_chain_desc;    

    swap_chain->GetDesc (swap_chain_desc);

    SetSize (swap_chain_desc.frame_buffer.width, swap_chain_desc.frame_buffer.height);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::SwapChainDepthStencilBuffer::SwapChainDepthStencilBuffer");
    throw;
  }
}

SwapChainDepthStencilBuffer::SwapChainDepthStencilBuffer (const FrameBufferManagerPtr& manager, ISwapChain* swap_chain, const TextureDesc& desc)
  : SwapChainRenderBuffer (manager, swap_chain, desc)
{
  try
  {
      //�������� ���������� ����������

    SwapChainDesc swap_chain_desc;

    swap_chain->GetDesc (swap_chain_desc);

    if (desc.width > swap_chain_desc.frame_buffer.width)
      throw xtl::format_not_supported_exception ("", "Depth-stencil buffer width=%u is greater than swap-chain desc.width=%u", desc.width,
        swap_chain_desc.frame_buffer.width);

    if (desc.height > swap_chain_desc.frame_buffer.height)
      throw xtl::format_not_supported_exception ("", "Depth-stencil buffer height=%u is greater than swap-chain desc.height=%u", desc.height,
        swap_chain_desc.frame_buffer.height);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::SwapChainDepthStencilBuffer::SwapChainDepthStencilBuffer");
    throw;
  }
}

SwapChainDepthStencilBuffer::~SwapChainDepthStencilBuffer ()
{
}
   
/*
    ��������� � �������� OpenGL
*/

void SwapChainDepthStencilBuffer::Bind ()
{
  try
  {
    SetFrameBuffer (GL_NONE);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::SwapChainDepthStencilBuffer::Bind");
    throw;
  }  
}

/*
===================================================================================================
    SwapChainFakeDepthStencilBuffer
===================================================================================================
*/

/*
    ����������� / ����������
*/

SwapChainFakeDepthStencilBuffer::SwapChainFakeDepthStencilBuffer (const ContextManager& manager, const TextureDesc& in_desc)
  : ContextObject (manager),
    desc (in_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainFakeDepthStencilBuffer::SwapChainFakeDepthStencilBuffer";

    //�������� ������������ �����������

  switch (desc.format)
  {
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8:
    case PixelFormat_D32:
    case PixelFormat_S8:
      break;
    case PixelFormat_RGB8:
    case PixelFormat_RGBA8:   
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_LA8:
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
    case PixelFormat_RGB_PVRTC2:
    case PixelFormat_RGB_PVRTC4:
    case PixelFormat_RGBA_PVRTC2:
    case PixelFormat_RGBA_PVRTC4:    
    case PixelFormat_ATC_RGB_AMD:
    case PixelFormat_ATC_RGBA_EXPLICIT_ALPHA_AMD:
    case PixelFormat_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
      throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported desc.format=%s", get_name (desc.format));
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.format", desc.format);
  }    
  
  if (desc.bind_flags & ~BindFlag_DepthStencil)
    throw xtl::make_argument_exception (METHOD_NAME, "Unsupported bindable flags desc.bind_flags=%s", get_name ((BindFlag)desc.bind_flags));

  switch (desc.access_flags)
  {
    case 0:
      break;
    case AccessFlag_Read:
    case AccessFlag_Write:
    case AccessFlag_ReadWrite:
      throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported desc.access_flags=%s (incompatible with pbuffer emulation mode)", get_name ((AccessFlag)desc.access_flags));
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.access_flags", (size_t)desc.access_flags);
  }
  
  switch (desc.usage_mode)  
  {
    case UsageMode_Default:
    case UsageMode_Static:
    case UsageMode_Dynamic:
    case UsageMode_Stream:
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.usage_mode", desc.usage_mode);
  }
  
  switch (desc.dimension)
  {
    case TextureDimension_2D:
      break;
    case TextureDimension_1D:
    case TextureDimension_3D:
    case TextureDimension_Cubemap:
      throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported render-buffer dimension desc.dimension=%s", get_name (desc.dimension));
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.dimension", desc.dimension);
  }

  if (desc.generate_mips_enable)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Automatic mip-map generation for render-buffers not supported");
}

SwapChainFakeDepthStencilBuffer::~SwapChainFakeDepthStencilBuffer ()
{
}

/*
    ��������� �����������
*/

void SwapChainFakeDepthStencilBuffer::GetDesc (TextureDesc& out_desc)
{
  out_desc = desc;
}

/*
    ������ � �������
*/

void SwapChainFakeDepthStencilBuffer::SetData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height, PixelFormat source_format, const void* buffer, IDeviceContext*)
{
  throw xtl::format_operation_exception ("render::low_level::opengl::SwapChainFakeDepthStencilBuffer::SetData",
    "Can't set render buffer data (no AccessFlag_Write in desc.access_flags)");
}

void SwapChainFakeDepthStencilBuffer::GetData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height, PixelFormat target_format, void* buffer, IDeviceContext*)
{
  throw xtl::format_operation_exception ("render::low_level::opengl::SwapChainFakeDepthStencilBuffer::GetData",
    "Can't get render buffer data (no AccessFlag_Read in desc.access_flags)");
}
