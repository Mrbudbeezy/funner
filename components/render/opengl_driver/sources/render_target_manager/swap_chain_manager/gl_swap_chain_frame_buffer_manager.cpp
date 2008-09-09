#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    ��������� / ��������
*/

SwapChainFrameBufferManager::SwapChainFrameBufferManager (const ContextManager& context_manager, ISwapChain* in_default_swap_chain)
  : ContextObject (context_manager),
    default_swap_chain (in_default_swap_chain)

{
}

SwapChainFrameBufferManager::~SwapChainFrameBufferManager ()
{
}

/*
    ����� � ⥭��묨 ���ࠬ�
*/

ColorBufferPtr SwapChainFrameBufferManager::CreateShadowColorBuffer (SwapChainDepthStencilBuffer* depth_stencil_buffer)
{
  xtl::com_ptr<ISwapChain> swap_chain (GetContextManager ().CreateCompatibleSwapChain (depth_stencil_buffer->GetSwapChain ()), false);

  return ColorBufferPtr (new SwapChainColorBuffer (this, swap_chain.get (), 1), false);
}

DepthStencilBufferPtr SwapChainFrameBufferManager::CreateShadowDepthStencilBuffer (SwapChainColorBuffer* color_buffer)
{
  return DepthStencilBufferPtr (new SwapChainDepthStencilBuffer (this, color_buffer->GetSwapChain ()), false);
}

ColorBufferPtr SwapChainFrameBufferManager::GetShadowBuffer (SwapChainDepthStencilBuffer* depth_stencil_buffer)
{
    //����⪠ ������ �������騩 ⥭���� ���� 梥�
    
  if (shadow_color_buffer)
    return shadow_color_buffer.get ();

    //ᮧ����� ������ ���� 梥�

  ColorBufferPtr color_buffer = CreateShadowColorBuffer (depth_stencil_buffer);

  shadow_color_buffer = color_buffer.get ();
  
  return color_buffer;
}

DepthStencilBufferPtr SwapChainFrameBufferManager::GetShadowBuffer (SwapChainColorBuffer* color_buffer)
{
    //����⪠ ������ �������騩 ⥭���� ���� �����ᥫ쭮�� ���祭��
    
  if (shadow_depth_stencil_buffer)
    return shadow_depth_stencil_buffer.get ();

    //ᮧ����� ������ ���� �����ᥫ쭮�� ���祭��

  DepthStencilBufferPtr depth_stencil_buffer = CreateShadowDepthStencilBuffer (color_buffer);
  
  shadow_depth_stencil_buffer = depth_stencil_buffer.get ();
  
  return depth_stencil_buffer;
}

void SwapChainFrameBufferManager::GetShadowBuffers (ColorBufferPtr& color_buffer, DepthStencilBufferPtr& depth_stencil_buffer)
{
    //����⪠ ������ �������騥 �����
    
  if (shadow_depth_stencil_buffer && shadow_color_buffer)
  {
    color_buffer         = shadow_color_buffer.get ();
    depth_stencil_buffer = shadow_depth_stencil_buffer.get ();

    return;
  }
      
    //ᮧ����� ���� 梥�

  if (!shadow_color_buffer && shadow_depth_stencil_buffer)
  {
    DepthStencilBufferPtr new_depth_stencil_buffer = shadow_depth_stencil_buffer.get ();
    ColorBufferPtr        new_color_buffer         = GetShadowBuffer (new_depth_stencil_buffer.get ());

    color_buffer         = new_color_buffer;
    depth_stencil_buffer = new_depth_stencil_buffer;
    
    return;
  }

    //ᮧ����� ���� �����ᥫ쭮�� ���祭��

  if (shadow_color_buffer && !shadow_depth_stencil_buffer)
  {
    ColorBufferPtr        new_color_buffer         = shadow_color_buffer.get ();
    DepthStencilBufferPtr new_depth_stencil_buffer = GetShadowBuffer (new_color_buffer.get ());
    
    color_buffer         = new_color_buffer;
    depth_stencil_buffer = new_depth_stencil_buffer;    
    
    return;
  }

    //�᫨ ��� ���� ���������� - ᮧ����� PBuffer-�

  SwapChainPtr swap_chain (GetContextManager ().CreateCompatibleSwapChain (default_swap_chain.get ()), false);

    //ᮧ����� ⥭����� ���� 梥�

  ColorBufferPtr new_color_buffer (new SwapChainColorBuffer (this, swap_chain.get (), 1), false);  

  shadow_color_buffer = new_color_buffer.get ();    

    //ᮧ����� ⥭����� ���� �����ᥫ쭮�� ���祭��

  DepthStencilBufferPtr new_depth_stencil_buffer = GetShadowBuffer (new_color_buffer.get ());

    //������ ���祭��

  color_buffer         = new_color_buffer;
  depth_stencil_buffer = new_depth_stencil_buffer;
}

/*
    �������� ���஢ ७��ਭ��
*/

ITexture* SwapChainFrameBufferManager::CreateRenderBuffer (const TextureDesc& desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainFrameBufferManager::CreateRenderBuffer";
  
  switch (desc.format)
  {
    case PixelFormat_RGB8:
    case PixelFormat_RGBA8:
    {
      try
      {
        SwapChainDesc swap_chain_desc;

        default_swap_chain->GetDesc (swap_chain_desc);

        swap_chain_desc.frame_buffer.width  = desc.width;
        swap_chain_desc.frame_buffer.height = desc.height;

        SwapChainPtr swap_chain (GetContextManager ().CreateCompatibleSwapChain (default_swap_chain.get (), swap_chain_desc), false);

        return new SwapChainColorBuffer (this, swap_chain.get (), desc);
      }
      catch (xtl::exception& exception)
      {
        exception.touch (METHOD_NAME);
        
        throw;
      }
    }
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8:
    {
      try
      {
        return new SwapChainFakeDepthStencilBuffer (GetContextManager (), desc);
      }
      catch (xtl::exception& exception)
      {
        exception.touch (METHOD_NAME);

        throw;
      }
    }
    case PixelFormat_S8:
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_LA8:
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
      throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create render-target texture with format=%s", get_name (desc.format));
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.format", desc.format);
  }
}

ITexture* SwapChainFrameBufferManager::CreateColorBuffer (ISwapChain* swap_chain, size_t buffer_index)
{
  try
  {
    return new SwapChainColorBuffer (this, swap_chain, buffer_index);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::SwapChainFrameBufferManager::CreateColorBuffer");

    throw;
  }
}

ITexture* SwapChainFrameBufferManager::CreateDepthStencilBuffer (ISwapChain* swap_chain)
{
  try
  {
    return new SwapChainDepthStencilBuffer (this, swap_chain);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::SwapChainFrameBufferManager::CreateDepthStencilBuffer");

    throw;
  }
}

/*
    �������� ���஢ ����
*/

//�஢�ઠ ���������� ᮧ����� ���� ����
bool SwapChainFrameBufferManager::IsSupported (View* color_view, View* depth_stencil_view)
{
    //�஢�ઠ ᮢ���⨬��� 梥⮢��� �⮡ࠦ����

  if (color_view)
  {
    ITexture* base_texture = color_view->GetTexture ();
    
    if (!dynamic_cast<IRenderTargetTexture*> (base_texture) && !dynamic_cast<SwapChainColorBuffer*> (base_texture))
      return false;
  }

    //�஢�ઠ ᮢ���⨬��� �⮡ࠦ���� ���� �����ᥫ쭮�� ���祭��
    
  if (depth_stencil_view)
  {
    ITexture* base_texture = depth_stencil_view->GetTexture ();
    
    if (!dynamic_cast<IRenderTargetTexture*> (base_texture) && !dynamic_cast<SwapChainDepthStencilBuffer*> (base_texture) &&
        !dynamic_cast<SwapChainFakeDepthStencilBuffer*> (base_texture))
      return false;
  }

  return true;  
}

//ᮧ����� ���� ����
IFrameBuffer* SwapChainFrameBufferManager::CreateFrameBuffer (View* color_view, View* depth_stencil_view)
{
  try
  {
    if (!color_view && !depth_stencil_view)
      return new SwapChainNullFrameBuffer (this);

    return new SwapChainFrameBuffer (this, color_view, depth_stencil_view);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::SwapChainFrameBufferManager::CreateFrameBuffer");

    throw;
  }  
}

/*
    ��⠭���� ��⨢���� ���� ����
*/

void SwapChainFrameBufferManager::SetFrameBuffer (ISwapChain* swap_chain, GLenum buffer_type)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainFrameBufferManager::SetFrameBuffer";
  
  if (!swap_chain)
  {
    buffer_type = GL_NONE;
    swap_chain  = default_swap_chain.get ();
  }

    //��⠭���� ��⨢��� 楯�窨 ������

  GetContextManager ().SetSwapChain (swap_chain);

    //�஢�ઠ ����室����� �����⠭���� ����

  const size_t current_buffer_type = GetContextCacheValue (CacheEntry_BufferAttachment),
               current_fbo         = GetContextCacheValue (CacheEntry_FrameBufferId);

  if (current_buffer_type == buffer_type && !current_fbo)
    return;

    //��⠭���� ⥪�饣� ���⥪�� OpenGL

  MakeContextCurrent ();

    //��⠭���� ���� ���� �� 㬮�砭��

  if (GetCaps ().has_ext_framebuffer_object && current_fbo)
    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);

    //��⠭���� ⥪�饣� ���� �⥭�� � ���ᮢ��

  if (current_buffer_type != buffer_type)
  {
    glDrawBuffer (buffer_type);
    glReadBuffer (buffer_type);
  }

    //�஢�ઠ �訡��

  CheckErrors (METHOD_NAME);

    //��⠭���� ���祭�� ���-��६�����  
    
  SetContextCacheValue (CacheEntry_BufferAttachment, buffer_type);
  SetContextCacheValue (CacheEntry_FrameBufferId,    0);
}

/*
    ��������� ��ࠡ��稪�� ᮧ����� ���஢ ७��ਭ�� � ���஢ ����
*/

namespace render
{

namespace low_level
{

namespace opengl
{

void register_swap_chain_manager (RenderTargetRegistry& registry, const ContextManager& context_manager, ISwapChain* default_swap_chain)
{
  FrameBufferManagerPtr manager (new SwapChainFrameBufferManager (context_manager, default_swap_chain), false);

    //ॣ������ ��ࠡ��稪�� ᮧ����� ���஢ ७��ਭ��

  registry.RegisterCreater (ColorBufferCreater (xtl::bind (&SwapChainFrameBufferManager::CreateColorBuffer, manager, _1, _2)));
  registry.RegisterCreater (DepthStencilBufferCreater (xtl::bind (&SwapChainFrameBufferManager::CreateDepthStencilBuffer, manager, _1)));
  registry.RegisterCreater (RenderBufferCreater (xtl::bind (&SwapChainFrameBufferManager::CreateRenderBuffer, manager, _1)));

    //ॣ������ ��ࠡ��稪�� ᮧ����� ���஢ ����

  registry.RegisterCreater (&SwapChainFrameBufferManager::IsSupported, xtl::bind (&SwapChainFrameBufferManager::CreateFrameBuffer, manager, _1, _2));
}

}

}

}
