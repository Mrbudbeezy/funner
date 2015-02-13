#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    SwapChainFrameBuffer::RenderTarget
*/

inline SwapChainFrameBuffer::RenderTarget::RenderTarget ()
{
  memset (&view_desc, 0, sizeof view_desc);
  memset (&mip_level_desc, 0, sizeof mip_level_desc);
  memset (&texture_desc, 0, sizeof texture_desc);  
}

/*
    ����������� / ����������
*/

SwapChainFrameBuffer::SwapChainFrameBuffer (const FrameBufferManagerPtr& manager, View* color_view, View* depth_stencil_view)
  : ContextObject (manager->GetContextManager ()),
    frame_buffer_manager (manager),
    has_texture_targets (false),
    is_color_buffer_active (false)
{
  try
  {
    memset (&dirty_rect, 0, sizeof dirty_rect);
   
    SetColorView         (color_view);
    SetDepthStencilView  (depth_stencil_view);
    FinishInitialization ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::SwapChainFrameBuffer::SwapChainFrameBuffer");
    throw;
  }
}

/*
    ������������� ������� �����������
*/

void SwapChainFrameBuffer::SetColorView (View* view)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainFrameBuffer::SetColorView";  

    //�������� ������� ������ �����

  if (!view)
    return;    
  
    //�������� ������������� �������� � ����������� �������� � ������ �����
    
  ITexture* base_texture = view->GetTexture ();
  
  if (!base_texture)
    throw xtl::format_operation_exception (METHOD_NAME, "Internal error: view with null texture");
  
    //��������� ����� ���������� ������ �����

  is_color_buffer_active = true;

    //��������� ������ ���������� � ��������
    
  if (IRenderTargetTexture* texture = dynamic_cast<IRenderTargetTexture*> (base_texture))
  {
    RenderTarget& render_target = render_targets [RenderTargetType_Color];
    
    render_target.target_texture = texture;    

    view->GetDesc            (render_target.view_desc);
    texture->GetMipLevelDesc (render_target.view_desc.mip_level, render_target.mip_level_desc);
    texture->GetDesc         (render_target.texture_desc);    

    switch (render_target.texture_desc.format)
    {
      case PixelFormat_RGB8:
      case PixelFormat_RGBA8:
      case PixelFormat_L8:
      case PixelFormat_A8:
      case PixelFormat_LA8:
        break;
      case PixelFormat_D16:
      case PixelFormat_D24X8:
      case PixelFormat_D24S8:
      case PixelFormat_D32:
      case PixelFormat_S8:
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
        throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported color render-target texture format=%s", get_name (render_target.texture_desc.format));
      default:
        throw xtl::make_argument_exception (METHOD_NAME, "texture_desc.format", render_target.texture_desc.format);
    }    
    
    has_texture_targets = true;    

    return;
  }
  
    //��������� ������ ���������� � SwapChainColorBuffer
      
  if (SwapChainColorBuffer* buffer = dynamic_cast<SwapChainColorBuffer*> (base_texture))
  {
    color_buffer = buffer;
    
    return;
  }
  
    //���� ������� �������� ����� ����������� ��� - �������� ������ ����� ����������

  throw xtl::format_operation_exception (METHOD_NAME, "Color-view texture has unknown type %s", view->GetTextureTypeName ());
}

void SwapChainFrameBuffer::SetDepthStencilView (View* view)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainFrameBuffer::SetDepthStencilView";

    //�������� ������� ������ ������������� ���������

  if (!view)
    return;

    //�������� ������������� �������� � ����������� �������� � ������ �����

  ITexture* base_texture = view->GetTexture ();
  
  if (!base_texture)
    throw xtl::format_operation_exception (METHOD_NAME, "Internal error: view with null texture");

    //��������� ������ ���������� � ��������

  if (IRenderTargetTexture* texture = dynamic_cast<IRenderTargetTexture*> (base_texture))
  {
    RenderTarget& render_target = render_targets [RenderTargetType_DepthStencil];

    render_target.target_texture = texture;

    view->GetDesc            (render_target.view_desc);
    texture->GetMipLevelDesc (render_target.view_desc.mip_level, render_target.mip_level_desc);    
    texture->GetDesc         (render_target.texture_desc);
    
    switch (render_target.texture_desc.format)
    {
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
        throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported depth-stencil render-target texture format=%s", get_name (render_target.texture_desc.format));
      case PixelFormat_D16:
      case PixelFormat_D24X8:
      case PixelFormat_D24S8:
      case PixelFormat_D32:
      case PixelFormat_S8:
        break;
      default:
        throw xtl::make_argument_exception (METHOD_NAME, "texture_desc.format", render_target.texture_desc.format);
    }    
    
    has_texture_targets = true;    

    return;
  }

    //��������� ������ ���������� � SwapChainDepthStencilBuffer

  if (SwapChainDepthStencilBuffer* buffer = dynamic_cast<SwapChainDepthStencilBuffer*> (base_texture))
  {
    depth_stencil_buffer = buffer;

    return;
  }
  
    //��������� ������ ���������� �� ��������������� �����
    
  if (SwapChainFakeDepthStencilBuffer* buffer = dynamic_cast<SwapChainFakeDepthStencilBuffer*> (base_texture))
  {
    fake_depth_stencil_buffer = buffer;

    return;
  }

    //���� ������� �������� ����� ����������� ��� - �������� ������ ����� ����������

  throw xtl::format_operation_exception (METHOD_NAME, "Depth-stencil-view texture has unknown type %s", view->GetTextureTypeName ());
}

void SwapChainFrameBuffer::FinishInitialization ()
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainFrameBuffer::FinishInitialization";

    //���� ��� ������ ������������������� - �������� �������������

  if (color_buffer && depth_stencil_buffer)
  {
    if (color_buffer->GetSwapChain () != depth_stencil_buffer->GetSwapChain ())
      throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported render targets configuration. Render-target-view and depth-stencil view have different swap chains");

    return;
  }
  
    //�������� �������������� �� ��������������� ������� ������������� ���������

  if (fake_depth_stencil_buffer && color_buffer)
  {
    if (!color_buffer->IsShadow ())
      throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported render targets configuration. Render-target-view is not pbuffer");

    return;
  }

  if (fake_depth_stencil_buffer && !color_buffer)
  {
    frame_buffer_manager->GetShadowBuffers (color_buffer, depth_stencil_buffer);

    return;
  }

    //�������� ������� ���������� ������ �� �������  

  if (color_buffer && !depth_stencil_buffer)
  {
      //����������� ����� ������������� ���������, �� ������������ ����� �����

    depth_stencil_buffer = frame_buffer_manager->GetShadowBuffer (color_buffer.get ());

    return;
  }
  
  if (!color_buffer && depth_stencil_buffer)
  {
      //����������� ����� �����, �� ������������ ����� ������������� ���������

    color_buffer = frame_buffer_manager->GetShadowBuffer (depth_stencil_buffer.get ());    
    
    return;
  }

    //����������� ��� ������ (��������� � ��� ������� ��������)    

  frame_buffer_manager->GetShadowBuffers (color_buffer, depth_stencil_buffer);    
}

/*
    ��������� ������ � �������� OpenGL
*/

void SwapChainFrameBuffer::Bind ()
{
  try
  {
    frame_buffer_manager->SetFrameBuffer (color_buffer->GetSwapChain (),
      is_color_buffer_active ? color_buffer->GetBufferType () : GL_NONE, color_buffer->GetFrameBufferId (),
      color_buffer->GetFrameBufferCacheId ());
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::SwapChainFrameBuffer::Bind");
    throw;
  }
}

/*
    ���������� �� ��������� � ������� ������
*/

void SwapChainFrameBuffer::InvalidateRenderTargets (unsigned int render_target_slot, const Rect& update_rect)
{
  if (render_target_slot)
    throw xtl::format_not_supported_exception ("render::low_level::opengl::SwapChainFrameBuffer::InvalidateRenderTargets(unsigned int,const Rect&)", "Attempt to invalidate render target %u. MRT not supported for this frame buffer", render_target_slot);

  if (update_rect.x < dirty_rect.x)
  {
    dirty_rect.width += dirty_rect.x - update_rect.x;
    dirty_rect.x      = update_rect.x;
  }

  if (update_rect.y < dirty_rect.y)
  {
    dirty_rect.height += dirty_rect.y - update_rect.y;
    dirty_rect.y       = update_rect.y;
  }

  unsigned int right  = update_rect.x + update_rect.width,
               bottom = update_rect.y + update_rect.height;

  if (right > dirty_rect.x + dirty_rect.width)
    dirty_rect.width = right - dirty_rect.x;
    
  if (bottom > dirty_rect.y + dirty_rect.height)
    dirty_rect.height = bottom - dirty_rect.y;
}

void SwapChainFrameBuffer::InvalidateRenderTargets (unsigned int render_target_slot)
{
  if (render_target_slot)
    throw xtl::format_not_supported_exception ("render::low_level::opengl::SwapChainFrameBuffer::InvalidateRenderTargets(unsigned int)", "Attempt to invalidate render target %u. MRT not supported for this frame buffer", render_target_slot);

  unsigned int color_width  = render_targets [RenderTargetType_Color].mip_level_desc.width,
               color_height = render_targets [RenderTargetType_Color].mip_level_desc.height,
               ds_width     = render_targets [RenderTargetType_DepthStencil].mip_level_desc.width,
               ds_height    = render_targets [RenderTargetType_DepthStencil].mip_level_desc.height;

  dirty_rect.x      = 0;
  dirty_rect.y      = 0;
  dirty_rect.width  = color_width < ds_width ? ds_width : color_width;
  dirty_rect.height = color_height < ds_height ? ds_height : color_height;
}

/*
    ���������� ������� �������
*/

void SwapChainFrameBuffer::UpdateRenderTargets ()
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainFrameBuffer::UpdateRenderTargets";

    //���� ������� �������� ����������� ��� ������� ���������� �����, �� ���������� �� �����

  if (!has_texture_targets || !dirty_rect.width || !dirty_rect.height)
    return;

    //��������� ��������� ������ �����

  Bind ();
  
    //����������� ����������� �� ��������� ������� ������� � ��������
        
  for (unsigned int i=0; i<RenderTargetType_Num; i++)
  {
    RenderTarget& render_target = render_targets [i];

    if (!render_target.target_texture)
      continue;

      //��������� ���������� ������� ��������

    const ViewDesc&                view_desc      = render_target.view_desc;
    const MipLevelDesc&            mip_level_desc = render_target.mip_level_desc;
    const RenderTargetTextureDesc& texture_desc   = render_target.texture_desc;

      //���������
      
    unsigned int x      = dirty_rect.x,
                 y      = dirty_rect.y,
                 width  = dirty_rect.width,
                 height = dirty_rect.height;

    if (x > mip_level_desc.width || y > mip_level_desc.height)
      continue;

    if (x + width > mip_level_desc.width)
      width = mip_level_desc.width - x;

    if (y + height > mip_level_desc.height)
      height = mip_level_desc.height - y;

    if (!width || !height)
      continue;

      //��������� ������� ��������

    render_target.target_texture->Bind ();

      //����������� ����������� � ��������
      
    GLenum tex_target = texture_desc.target;

#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)
    switch (tex_target)
    {
      case GL_TEXTURE_1D:
        glCopyTexSubImage1D (GL_TEXTURE_1D, view_desc.mip_level, x, x, y, width);
        break;
      case GL_TEXTURE_CUBE_MAP:
        tex_target = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + view_desc.layer;
      case GL_TEXTURE_2D:
      case GL_TEXTURE_RECTANGLE_ARB:
        glCopyTexSubImage2D (tex_target, view_desc.mip_level, x, y, x, y, width, height);
        break;
      case GL_TEXTURE_3D:
        glCopyTexSubImage3D (tex_target, view_desc.mip_level, x, y, view_desc.layer, x, y, width, height);
        break;
      default:
        throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported texture target 0x%04x", tex_target);
    }
    
#else

    switch (tex_target)
    {
      case GL_TEXTURE_CUBE_MAP:
        tex_target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + view_desc.layer;
      case GL_TEXTURE_2D:
        glCopyTexSubImage2D (tex_target, view_desc.mip_level, x, y, x, y, width, height);
        break;
      default:
        throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported texture target 0x%04x", tex_target);
    }
#endif    
  }
  
    //�������� ������
  
  CheckErrors (METHOD_NAME);

    //������� ������� ����������

  memset (&dirty_rect, 0, sizeof dirty_rect);
}
