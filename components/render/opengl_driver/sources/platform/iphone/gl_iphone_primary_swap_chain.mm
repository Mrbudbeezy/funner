#include <shared/gl.h>

#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl::iphone;

/*
    �������� ���������� PrimarySwapChain
*/

typedef xtl::com_ptr<Adapter> AdapterPtr;

struct PrimarySwapChain::Impl
{
  Log             log;                       //��������
  AdapterPtr      adapter;                   //������� ������� ������
  SwapChainDesc   desc;                      //���������� ������� ������
  EAGLContext*    context;                   //������������� ��������
  GLuint          frame_buffer;              //����� �����
  GLuint          depth_buffer;              //����� �������
  GLuint          stencil_buffer;            //����� �������
  GLuint          render_buffer;             //����� ����������

///�����������
  Impl (const SwapChainDesc& in_desc, Adapter* in_adapter)
    : adapter (in_adapter), context (0)
  {
      //�������� ������������ ����������

    if (!in_desc.window_handle)
      throw xtl::make_null_argument_exception ("", "in_desc.window_handle");

      //���������� ����������� ������� ������

    memset (&desc, 0, sizeof (desc));

    CGRect window_frame = ((UIView*)in_desc.window_handle).frame;

    desc.frame_buffer.width        = window_frame.size.width;
    desc.frame_buffer.height       = window_frame.size.height;
    desc.frame_buffer.color_bits   = 24;
    desc.frame_buffer.alpha_bits   = 8;
    desc.frame_buffer.depth_bits   = in_desc.frame_buffer.depth_bits ? 16 : 0;
    desc.frame_buffer.stencil_bits = in_desc.frame_buffer.stencil_bits ? 8 : 0;
    desc.samples_count             = 0;
    desc.buffers_count             = 2;
    desc.swap_method               = SwapMethod_Discard;
    desc.vsync                     = true;
    desc.fullscreen                = false;
    desc.window_handle             = in_desc.window_handle;    //?????????���������� ������������� �� ��������� �������� ����

    log.Printf ("...choose pixel format (RGB/A: %u/%u, D/S: %u/%u, Samples: %u)",
      desc.frame_buffer.color_bits, desc.frame_buffer.alpha_bits, desc.frame_buffer.depth_bits,
      desc.frame_buffer.stencil_bits, desc.samples_count);
  }

///����������
  ~Impl ()
  {
    SetContext (0);

    log.Printf ("...release resources");
  }

  ///��������� ������ ���������
  void SetContext (EAGLContext* new_context)
  {
    static const char* METHOD_NAME = "render::low_level::opengl::iphone::PrimarySwapChain::Impl::SetContext";

    if (context == new_context)
      return;

    //??????���������� ��������� ������ ����� ������� ��� �������/��������� ����������

    //�������� �������� �������� ���������
    if (context)
    {
      SetContextCurrent ();

      glDeleteFramebuffersOES (1, &frame_buffer);
      glDeleteRenderbuffersOES (1, &render_buffer);

      if (desc.frame_buffer.depth_bits)
        glDeleteRenderbuffersOES (1, &depth_buffer);

      if (desc.frame_buffer.stencil_bits)
        glDeleteRenderbuffersOES (1, &stencil_buffer);

      try
      {
        CheckErrors (METHOD_NAME);
      }
      catch (...)
      {
        //���������� ������
      }
    }

    context = new_context;

    if (!context)
      return;

    SetContextCurrent ();
    printf ("Generating buffers\n");
    fflush (stdout);

    try
    {
      glGenFramebuffersOES (1, &frame_buffer);
      printf ("Framebuffer generated\n");
      fflush (stdout);
      glBindFramebufferOES (GL_FRAMEBUFFER_OES, frame_buffer);

      glGenRenderbuffersOES (1, &render_buffer);
      glBindRenderbufferOES (GL_RENDERBUFFER_OES, render_buffer);

      CheckErrors (METHOD_NAME);

      CAEAGLLayer* eagl_layer = (CAEAGLLayer*)[(UIView*)(desc.window_handle) layer];

      eagl_layer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:NO],
                                       kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];

      if (![context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:eagl_layer])
        throw xtl::format_operation_exception ("render::low_level::opengl::iphone::PrimarySwapChain::Impl::SetContext", "Can't set context");

      glFramebufferRenderbufferOES (GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, render_buffer);

      if (desc.frame_buffer.depth_bits)
      {
        glGenRenderbuffersOES (1, &depth_buffer);
        glBindRenderbufferOES (GL_RENDERBUFFER_OES, depth_buffer);
        glRenderbufferStorageOES (GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, desc.frame_buffer.width, desc.frame_buffer.height);
        glFramebufferRenderbufferOES (GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depth_buffer);
      }

      if (desc.frame_buffer.stencil_bits)
      {
        glGenRenderbuffersOES (1, &stencil_buffer);
        glBindRenderbufferOES (GL_RENDERBUFFER_OES, stencil_buffer);
        glRenderbufferStorageOES (GL_RENDERBUFFER_OES, GL_STENCIL_INDEX8_OES, desc.frame_buffer.width, desc.frame_buffer.height);
        glFramebufferRenderbufferOES (GL_FRAMEBUFFER_OES, GL_STENCIL_ATTACHMENT_OES, GL_RENDERBUFFER_OES, stencil_buffer);
      }

      GLenum status = glCheckFramebufferStatusOES (GL_FRAMEBUFFER_OES);

      if (status != GL_FRAMEBUFFER_COMPLETE_OES)
        throw xtl::format_operation_exception ("render::low_level::opengl::iphone::PrimarySwapChain::Impl::SetContext", "Failed to make complete framebuffer object");
    }
    catch (...)
    {
      SetContext (0);
      throw;
    }
  }

  ///����� �������� ������� ������ � ��������� ������
  void Present ()
  {
    if (!context)
      return;

    EAGLContext *current_context = [EAGLContext currentContext];

    if (current_context != context)
      [EAGLContext setCurrentContext:context];

    glBindRenderbufferOES (GL_RENDERBUFFER_OES, render_buffer);

    if (![context presentRenderbuffer:GL_RENDERBUFFER_OES])
      throw xtl::format_operation_exception ("render::low_level::opengl::iphone::PrimarySwapChain::Impl::Present", "Failed to swap renderbuffer");
  }

  private:
    ///��������� �������� ���������
    void SetContextCurrent ()
    {
      EAGLContext *current_context = [EAGLContext currentContext];

      if (current_context != context)
        [EAGLContext setCurrentContext:context];
    }

    ///�������� ������ OpenGL
    void CheckErrors (const char* source)
    {
      GLenum error = glGetError ();

      switch (error)
      {
        case GL_NO_ERROR:
          break;
        case GL_INVALID_ENUM:
          throw xtl::format_operation_exception (source, "OpenGL error: invalid enum");
        case GL_INVALID_VALUE:
          throw xtl::format_operation_exception (source, "OpenGL error: invalid value");
        case GL_INVALID_OPERATION:
          throw xtl::format_operation_exception (source, "OpenGL error: invalid operation");
        case GL_STACK_OVERFLOW:
          throw xtl::format_operation_exception (source, "OpenGL error: stack overflow");
        case GL_STACK_UNDERFLOW:
          throw xtl::format_operation_exception (source, "OpenGL error: stack underflow");
        case GL_OUT_OF_MEMORY:
          throw xtl::format_operation_exception (source, "OpenGL error: out of memory");
        default:
          throw xtl::format_operation_exception (source, "OpenGL error: code=0x%04x", error);
      }
    }
};

/*
    ����������� / ����������
*/

PrimarySwapChain::PrimarySwapChain (const SwapChainDesc& desc, Adapter* adapter)
{
  Log log;

  log.Printf ("...create primary swap chain (id=%u)", GetId ());

  try
  {
    impl = new Impl (desc, adapter);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::iphone::PrimarySwapChain::PrimarySwapChain");
    throw;
  }

  log.Printf ("...primary swap chain successfully created");
}

PrimarySwapChain::~PrimarySwapChain ()
{
  try
  {
    Log log;

    log.Printf ("Destroy primary swap chain (id=%u)...", GetId ());

    impl = 0;

    log.Printf ("...primary swap chain successfully destroyed");
  }
  catch (...)
  {
    //��������� ��� ����������
  }
}

/*
    ��������� ��������
*/

IAdapter* PrimarySwapChain::GetAdapter ()
{
  return impl->adapter.get ();
}

/*
    ��������� �����������
*/

void PrimarySwapChain::GetDesc (SwapChainDesc& out_desc)
{
  out_desc = impl->desc;
}

/*
    ��������� ���������� ������ � ������������ �������� ������� ����������
*/

IOutput* PrimarySwapChain::GetContainingOutput ()
{
  return impl->adapter->GetOutput (0);
}

/*
    ����� �������� ������� ������ � ��������� ������
*/

void PrimarySwapChain::Present ()
{
  try
  {
    impl->Present ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::iphone::PrimarySwapChain::Present");

    throw;
  }
}

/*
    ��������� / ������ ��������� full-screen mode
*/

void PrimarySwapChain::SetFullscreenState (bool state)
{
  if (state)
    throw xtl::format_not_supported_exception ("render::low_level::opengl::iphone::PrimarySwapChain::SetFullscreenState",
                                               "Changing fullscreen state not supported");
}

bool PrimarySwapChain::GetFullscreenState ()
{
  return false;
}

/*
   ���������/��������� ���������
*/

void PrimarySwapChain::SetContext (EAGLContext* context)
{
  impl->SetContext (context);
}

EAGLContext* PrimarySwapChain::GetContext ()
{
  return impl->context;
}
