#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    ����������� / ����������
*/

SwapChainFrameBuffer::SwapChainFrameBuffer
 (const ContextManager&        manager,
  SwapChainColorBuffer*        in_color_buffer,
  SwapChainDepthStencilBuffer* in_depth_stencil_buffer)
    : ContextObject (manager),
      color_buffer (in_color_buffer),
      depth_stencil_buffer (in_depth_stencil_buffer),
      color_buffer_state (true),
      depth_stencil_buffer_state (true)
{
  memset (&render_target_desc, 0, sizeof (render_target_desc));
  memset (&depth_stencil_desc, 0, sizeof (depth_stencil_desc));
}

/*
    ��������� ������ � �������� OpenGL
*/

void SwapChainFrameBuffer::Bind (bool& out_color_buffer_state, bool& out_depth_stencil_buffer_state)
{
  size_t      context_id  = depth_stencil_buffer ? depth_stencil_buffer->GetContextId () : 0;
  ISwapChain* swap_chain  = color_buffer->GetSwapChain ();
  GLenum      buffer_type = color_buffer_state ? color_buffer->GetBufferType () : GL_NONE;
  
    //��������� ��������� ���������

  GetContextManager ().SetContext (context_id, swap_chain, swap_chain);
  
    //��������� �������� ��������� OpenGL

  MakeContextCurrent ();
  
    //��������� ������ ����� �� ���������
    
  static Extension EXT_framebuffer_object = "GL_EXT_framebuffer_object";
  
  if (IsSupported (EXT_framebuffer_object))
    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
  
    //��������� �������� ������ ������ � ���������    

  glReadBuffer (buffer_type);
  glDrawBuffer (buffer_type);
  
    //�������� ������

  CheckErrors ("render::low_level::opengl::SwapChainFrameBuffer::Bind");
  
    //��������� ������ ��������� ������� ���������
    
  out_color_buffer_state         = color_buffer_state;
  out_depth_stencil_buffer_state = depth_stencil_buffer_state;
}

/*
    ���������� / ���������� �������
*/

void SwapChainFrameBuffer::SetBuffersState (bool in_color_buffer_state, bool in_depth_stencil_buffer_state)
{
  color_buffer_state         = in_color_buffer_state;
  depth_stencil_buffer_state = in_depth_stencil_buffer_state;
}

/*
    ������ � ����������
*/

void SwapChainFrameBuffer::SetRenderTargets
 (IRenderTargetTexture* in_render_target_texture,
  const ViewDesc*   in_render_target_desc,
  IRenderTargetTexture* in_depth_stencil_texture,
  const ViewDesc*   in_depth_stencil_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainFrameBuffer::SetRenderTargets";

  if (in_render_target_texture)
  {
    RenderTargetTextureDesc desc;
    
    in_render_target_texture->GetDesc (desc);
    
    switch (desc.format)
    {
      case PixelFormat_RGB8:
      case PixelFormat_RGBA8:
      case PixelFormat_L8:
      case PixelFormat_A8:
      case PixelFormat_LA8:
      case PixelFormat_DXT1:
      case PixelFormat_DXT3:
      case PixelFormat_DXT5:
        break;
      case PixelFormat_D16:
      case PixelFormat_D24X8:
      case PixelFormat_D24S8:
      case PixelFormat_S8:
        RaiseNotSupported (METHOD_NAME, "Unsupported render-target texture format=%s", get_name (desc.format));
        break;
      default:
        RaiseInvalidArgument (METHOD_NAME, "desc.format", desc.format);
        break;
    }
  }
  
  if (in_depth_stencil_texture)
  {
    RenderTargetTextureDesc desc;
    
    in_depth_stencil_texture->GetDesc (desc);
    
    switch (desc.format)
    {
      case PixelFormat_RGB8:
      case PixelFormat_RGBA8:
      case PixelFormat_L8:
      case PixelFormat_A8:
      case PixelFormat_LA8:
      case PixelFormat_DXT1:
      case PixelFormat_DXT3:
      case PixelFormat_DXT5:
        RaiseNotSupported (METHOD_NAME, "Unsopported depth-stencil texture format=%s", get_name (desc.format));
        break;
      case PixelFormat_D16:
      case PixelFormat_D24X8:
      case PixelFormat_D24S8:
      case PixelFormat_S8:
        break;
      default:
        RaiseInvalidArgument (METHOD_NAME, "desc.format", desc.format);
        break;
    }
  }

  render_target_texture = in_render_target_texture;
  depth_stencil_texture = in_depth_stencil_texture;    
  
  if (in_render_target_desc)
  {
    render_target_desc = *in_render_target_desc;
  }
  else
  {
    memset (&render_target_desc, 0, sizeof (render_target_desc));
  }
  
  if (in_depth_stencil_desc)
  {
    depth_stencil_desc = *in_depth_stencil_desc;
  }
  else
  {
    memset (&depth_stencil_desc, 0, sizeof (depth_stencil_desc));
  }
}

/*
    ����������� ����������� � ��������
*/

void SwapChainFrameBuffer::CopyImage (size_t width, size_t height, const RenderTargetTextureDesc& texture_desc, const ViewDesc& view_desc)
{
  glBindTexture (texture_desc.target, texture_desc.id);  
  
  switch (texture_desc.target)
  {
    case GL_TEXTURE_1D:
      glCopyTexSubImage1D (GL_TEXTURE_1D, view_desc.mip_level, 0, 0, 0, width);
      break;
    case GL_TEXTURE_2D:
    case GL_TEXTURE_RECTANGLE_ARB:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB:
      glCopyTexSubImage2D (texture_desc.target, view_desc.mip_level, 0, 0, 0, 0, width, height);
      break;
    case GL_TEXTURE_3D:
      glCopyTexSubImage3D (texture_desc.target, view_desc.mip_level, 0, 0, view_desc.layer, 0, 0, width, height);
      break;
    default:
      RaiseNotSupported ("render::low_level::opengl::SwapChainFrameBuffer::CopyImage", "Unsupported textarget=0x%04x", texture_desc.target);
      break;
  }
}

void SwapChainFrameBuffer::UpdateRenderTargets ()
{
    //��������� ��������� ������ �����

  bool states [2];

  Bind (states [0], states [1]);
    
    //��������� ���������� � ������� ������
    
  SwapChainDesc swap_chain_desc;

  color_buffer->GetSwapChain ()->GetDesc (swap_chain_desc);
  
  size_t width  = swap_chain_desc.frame_buffer.width,
         height = swap_chain_desc.frame_buffer.height;

    //����������� �������� �� ������ �����
    
  if (render_target_texture)
  {
    RenderTargetTextureDesc tex_desc;

    render_target_texture->GetDesc (tex_desc);

    CopyImage (width < tex_desc.width ? width : tex_desc.width, height < tex_desc.height ? height : tex_desc.height, tex_desc, render_target_desc);
  }
  
    //����������� �������� �� ������ �������
    
  if (depth_stencil_texture)
  {
    RenderTargetTextureDesc tex_desc;

    depth_stencil_texture->GetDesc (tex_desc);

    CopyImage (width < tex_desc.width ? width : tex_desc.width, height < tex_desc.height ? height : tex_desc.height, tex_desc, depth_stencil_desc);
  }
  
  CheckErrors ("render::low_level::opengl::SwapChainFrameBuffer::UpdateRenderTargets");
}
