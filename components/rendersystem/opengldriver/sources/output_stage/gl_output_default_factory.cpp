#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
===================================================================================================
    ���������� ����������
===================================================================================================
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������ ����� ��� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SwapChainFrameBuffer: public FrameBuffer, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SwapChainFrameBuffer (const ContextManager&        manager,
                          SwapChainColorBuffer*        color_buffer,
                          SwapChainDepthStencilBuffer* depth_stencil_buffer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateRenderTargets ();
    
  private:
    typedef xtl::com_ptr<SwapChainColorBuffer>        ColorBufferPtr;
    typedef xtl::com_ptr<SwapChainDepthStencilBuffer> DepthStencilBufferPtr;

  private:
    ColorBufferPtr        color_buffer;         //����� �����
    DepthStencilBufferPtr depth_stencil_buffer; //����� �������-��������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� �������� ��������� ��������� OpenGL �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class DefaultOutputStageResourceFactory: public OutputStageResourceFactory, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    DefaultOutputStageResourceFactory  (const ContextManager& manager);
    ~DefaultOutputStageResourceFactory ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ITexture* CreateTexture             (const TextureDesc&);
    ITexture* CreateRenderTargetTexture (ISwapChain* swap_chain, size_t buffer_index);
    ITexture* CreateDepthStencilTexture (ISwapChain* swap_chain);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FrameBuffer* CreateFrameBuffer (NullView, NullView);
    FrameBuffer* CreateFrameBuffer (NullView, SwapChainDepthStencilBuffer*);
    FrameBuffer* CreateFrameBuffer (NullView, IBindableTexture*);
    FrameBuffer* CreateFrameBuffer (SwapChainColorBuffer*, NullView);
    FrameBuffer* CreateFrameBuffer (SwapChainColorBuffer*, SwapChainDepthStencilBuffer*);
    FrameBuffer* CreateFrameBuffer (SwapChainColorBuffer*, IBindableTexture*);
    FrameBuffer* CreateFrameBuffer (IBindableTexture*, NullView);
    FrameBuffer* CreateFrameBuffer (IBindableTexture*, SwapChainDepthStencilBuffer*);
    FrameBuffer* CreateFrameBuffer (IBindableTexture*, IBindableTexture*);
};

/*
===================================================================================================
    SwapChainFrameBuffer
===================================================================================================
*/

/*
===================================================================================================
    DefaultOutputStageResourceFactory
===================================================================================================
*/

/*
    ����������� / ����������
*/

SwapChainFrameBuffer::SwapChainFrameBuffer
 (const ContextManager&        manager,
  SwapChainColorBuffer*        in_color_buffer,
  SwapChainDepthStencilBuffer* in_depth_stencil_buffer)
    : ContextObject (manager),
      color_buffer (in_color_buffer),
      depth_stencil_buffer (in_depth_stencil_buffer)
{
}

/*
    ��������� ������ � �������� OpenGL
*/

void SwapChainFrameBuffer::Bind ()
{
  RaiseNotImplemented ("render::low_level::opengl::SwapChainDepthStencilBuffer::Bind");
}

/*
    ���������� ������� ������� ���������
*/

void SwapChainFrameBuffer::UpdateRenderTargets ()
{
  RaiseNotImplemented ("render::low_level::opengl::SwapChainDepthStencilBuffer::UpdateRenderTargets");
}

/*
    ����������� / ����������
*/

DefaultOutputStageResourceFactory::DefaultOutputStageResourceFactory (const ContextManager& manager)
  : ContextObject (manager)
{
}

DefaultOutputStageResourceFactory::~DefaultOutputStageResourceFactory ()
{
}

/*
    �������� �������� ������ ������    
*/

ITexture* DefaultOutputStageResourceFactory::CreateTexture (const TextureDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateTexture");
  return 0;
}

ITexture* DefaultOutputStageResourceFactory::CreateRenderTargetTexture (ISwapChain* swap_chain, size_t buffer_index)
{
  try
  {
    return new SwapChainColorBuffer (GetContextManager (), swap_chain, buffer_index);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateRenderTargetTexture");

    throw;
  }
}

ITexture* DefaultOutputStageResourceFactory::CreateDepthStencilTexture (ISwapChain* swap_chain)
{
  try
  {
    return new SwapChainDepthStencilBuffer (GetContextManager (), swap_chain);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateDepthStencilTexture");

    throw;
  }
}

/*
    �������� ����� �������
*/

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer (NullView, NullView)
{
  try
  {
    return new SwapChainFrameBuffer (GetContextManager (), 0, 0);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(NullView,NullView)");
    
    throw;
  }
}

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer (NullView, SwapChainDepthStencilBuffer* depth_stencil_buffer)
{
  try
  {
    return new SwapChainFrameBuffer (GetContextManager (), 0, depth_stencil_buffer);    
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(NullView,SwapChainDepthStencilBuffer*)");
    
    throw;
  }
}

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer (NullView, IBindableTexture*)
{
  RaiseNotImplemented ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(NullView,IBindableTexture*)");
  return 0;
}

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer (SwapChainColorBuffer* color_buffer, NullView)
{
  try
  {
    return new SwapChainFrameBuffer (GetContextManager (), color_buffer, 0);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(SwapChainColorBuffer*,NullView)");

    throw;
  }
}

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer
 (SwapChainColorBuffer*        color_buffer,
  SwapChainDepthStencilBuffer* depth_stencil_buffer)
{
  try
  {
    return new SwapChainFrameBuffer (GetContextManager (), color_buffer, depth_stencil_buffer);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(SwapChainColorBuffer*,SwapChainDepthStencilBuffer*)");

    throw;
  }
}

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer (SwapChainColorBuffer*, IBindableTexture*)
{
  RaiseNotImplemented ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(SwapChainColorBuffer*,IBindableTexture*)");
  return 0;
}

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer (IBindableTexture*, NullView)
{
  RaiseNotImplemented ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(IBindableTexture*,NullView)");
  return 0;
}

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer (IBindableTexture*, SwapChainDepthStencilBuffer*)
{
  RaiseNotImplemented ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(IBindableTexture*,SwapChainDepthStencilBuffer*)");
  return 0;
}

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer (IBindableTexture*, IBindableTexture*)
{
  RaiseNotImplemented ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(IBindableTexture*,IBindableTexture*)");
  return 0;
}

/*
    �������� ������� �������� ��������� ������ �� ���������
*/

OutputStageResourceFactory* OutputStageResourceFactory::CreateDefaultFactory (const ContextManager& manager)
{
  return new DefaultOutputStageResourceFactory (manager);
}
