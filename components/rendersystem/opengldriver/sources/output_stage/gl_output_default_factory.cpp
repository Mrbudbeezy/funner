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
///���������� / ���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetBuffersState            (bool color_buffer_state, bool depth_stencil_buffer_state);
    bool GetColorBufferState        () const { return color_buffer_state; }
    bool GetDepthStencilBufferState () const { return depth_stencil_buffer_state; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind (bool&, bool&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void              SetRenderTargets       (IBindableTexture*, const ViewDesc*, IBindableTexture*, const ViewDesc*);
    IBindableTexture* GetRenderTargetTexture () const { return render_target_texture.get (); }
    IBindableTexture* GetDepthStencilTexture () const { return depth_stencil_texture.get (); }
    const ViewDesc&   GetRenderTargetDesc    () const { return render_target_desc; }
    const ViewDesc&   GetDepthStencilDesc    () const { return depth_stencil_desc; }
    void              UpdateRenderTargets    ();

  private:
    typedef xtl::com_ptr<SwapChainColorBuffer>        ColorBufferPtr;
    typedef xtl::com_ptr<SwapChainDepthStencilBuffer> DepthStencilBufferPtr;
    typedef xtl::com_ptr<IBindableTexture>            TexturePtr;

  private:
    ColorBufferPtr        color_buffer;
    DepthStencilBufferPtr depth_stencil_buffer;
    TexturePtr            render_target_texture;
    TexturePtr            depth_stencil_texture;
    ViewDesc              render_target_desc;
    ViewDesc              depth_stencil_desc;    
    bool                  color_buffer_state;
    bool                  depth_stencil_buffer_state;
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
    FrameBuffer* CreateFrameBuffer (NullView, IBindableTexture*, const ViewDesc&);
    FrameBuffer* CreateFrameBuffer (SwapChainColorBuffer*, NullView);
    FrameBuffer* CreateFrameBuffer (SwapChainColorBuffer*, SwapChainDepthStencilBuffer*);
    FrameBuffer* CreateFrameBuffer (SwapChainColorBuffer*, IBindableTexture*, const ViewDesc&);
    FrameBuffer* CreateFrameBuffer (IBindableTexture*, const ViewDesc&, NullView);
    FrameBuffer* CreateFrameBuffer (IBindableTexture*, const ViewDesc&, SwapChainDepthStencilBuffer*);
    FrameBuffer* CreateFrameBuffer (IBindableTexture*, const ViewDesc&, IBindableTexture*, const ViewDesc&);
    
  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISwapChain* GetDefaultSwapChain ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SwapChainColorBuffer*        GetShadowBuffer          (SwapChainDepthStencilBuffer*);
    SwapChainDepthStencilBuffer* GetShadowBuffer          (SwapChainColorBuffer*);
    void                         GetShadowBuffers         (SwapChainColorBuffer*&, SwapChainDepthStencilBuffer*&);
    SwapChainColorBuffer*        CreateColorBuffer        (SwapChainDepthStencilBuffer*);
    SwapChainDepthStencilBuffer* CreateDepthStencilBuffer (SwapChainColorBuffer*);
    SwapChainFrameBuffer*        CreateShadowFrameBuffer  ();

  private:
    typedef stl::list<SwapChainColorBuffer*>        ColorBufferList;
    typedef stl::list<SwapChainDepthStencilBuffer*> DepthStencilBufferList;

  private:
    ColorBufferList        shadow_color_buffers;
    DepthStencilBufferList shadow_depth_stencil_buffers;
};

/*
===================================================================================================
    SwapChainFrameBuffer
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
 (IBindableTexture* in_render_target_texture,
  const ViewDesc*   in_render_target_desc,
  IBindableTexture* in_depth_stencil_texture,
  const ViewDesc*   in_depth_stencil_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainFrameBuffer::SetRenderTargets";

  if (in_render_target_texture)
  {
    TextureDesc desc;
    
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
        RaiseNotSupported (METHOD_NAME, "Can not bind texture with format %s as render-target", get_name (desc.format));
        break;
      default:
        RaiseInvalidArgument (METHOD_NAME, "desc.format", desc.format);
        break;
    }
  }
  
  if (in_depth_stencil_texture)
  {
    TextureDesc desc;
    
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
        RaiseNotSupported (METHOD_NAME, "Can not bind texture with format %s as depth-stencil", get_name (desc.format));
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

namespace
{

void copy_image (size_t width, size_t height, const BindableTextureDesc& texture_desc, const ViewDesc& view_desc)
{
  glBindTexture (texture_desc.target, texture_desc.id);  
  
  switch (texture_desc.target)
  {
    case GL_TEXTURE_1D:
      glCopyTexSubImage1D (GL_TEXTURE_1D, view_desc.mip_level, 0, 0, 0, width);
      break;
    case GL_TEXTURE_2D:
      glCopyTexSubImage2D (GL_TEXTURE_2D, view_desc.mip_level, 0, 0, 0, 0, width, height);
      break;
    default:
      break;
  }
}

}

void SwapChainFrameBuffer::UpdateRenderTargets ()
{
    //������ ����� ���������� � ������������� ���������, ������ �� ������� ������ MakeConextCurrent
    
    //��������� ���������� � ������� ������
    
  SwapChainDesc swap_chain_desc;

  color_buffer->GetSwapChain ()->GetDesc (swap_chain_desc);
  
  size_t width  = swap_chain_desc.frame_buffer.width,
         height = swap_chain_desc.frame_buffer.height;

    //����������� �������� �� ������ �����
    
  if (render_target_texture)
  {
    BindableTextureDesc bindable_desc;
    TextureDesc         texture_desc;

    render_target_texture->GetDesc (bindable_desc);
    render_target_texture->GetDesc (texture_desc);

    copy_image (width < texture_desc.width ? width : texture_desc.width,
                height < texture_desc.height ? height : texture_desc.height, bindable_desc, render_target_desc);
  }
  
    //����������� �������� �� ������ �������
    
  if (depth_stencil_texture)
  {
    BindableTextureDesc bindable_desc;
    TextureDesc         texture_desc;

    depth_stencil_texture->GetDesc (bindable_desc);
    depth_stencil_texture->GetDesc (texture_desc);

    copy_image (width < texture_desc.width ? width : texture_desc.width,
                height < texture_desc.height ? height : texture_desc.height, bindable_desc, depth_stencil_desc);
  }
  
  CheckErrors ("render::low_level::opengl::SwapChainFrameBuffer::UpdateRenderTargets");
}

/*
===================================================================================================
    DefaultOutputStageResourceFactory
===================================================================================================
*/

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
    ��������� ������� ������ �� ���������
*/

ISwapChain* DefaultOutputStageResourceFactory::GetDefaultSwapChain ()
{
  ISwapChain* swap_chain = GetContextManager ().GetDrawSwapChain ();
  
  if (!swap_chain)
    RaiseInvalidOperation ("render::low_level::opengl::DefaultOutputStageResourceFactory::GetDefaultSwapChain",
                           "Null default swap chain");

  return swap_chain;
}

/*
    ������ � �������� ��������
*/

SwapChainColorBuffer* DefaultOutputStageResourceFactory::CreateColorBuffer (SwapChainDepthStencilBuffer* depth_stencil_buffer)
{
  xtl::com_ptr<ISwapChain> swap_chain (GetContextManager ().CreateCompatibleSwapChain (depth_stencil_buffer->GetContextId ()), false);

  return new SwapChainColorBuffer (GetContextManager (), swap_chain.get (), 1);
}

SwapChainDepthStencilBuffer* DefaultOutputStageResourceFactory::CreateDepthStencilBuffer (SwapChainColorBuffer* color_buffer)
{
  return new SwapChainDepthStencilBuffer (GetContextManager (), color_buffer->GetSwapChain ());
}

namespace
{

template <class List> class list_remover
{
  public:
    typedef typename List::iterator Iter;  
  
    list_remover (List& in_list, Iter in_iter) : list (in_list), iter (in_iter) {}
  
    void operator () () const { list.erase (iter); }
    
  private:
    List& list;
    Iter  iter;
};

}

SwapChainColorBuffer* DefaultOutputStageResourceFactory::GetShadowBuffer (SwapChainDepthStencilBuffer* depth_stencil_buffer)
{
  ContextManager& manager    = GetContextManager ();
  size_t          context_id = depth_stencil_buffer->GetContextId ();
  
    //������� ����� ����������� �� ������� ����� �����
  
  for (ColorBufferList::iterator iter=shadow_color_buffers.begin (), end=shadow_color_buffers.end (); iter!=end; ++iter)
    if (manager.IsCompatible (context_id, (*iter)->GetSwapChain ()))
    {
        //�����������: ����������� ���������� ���� � ������ ������ ��� ��������� ���������� ������

      shadow_color_buffers.splice (shadow_color_buffers.begin (), shadow_color_buffers, iter);

      return *iter;
    }

    //�������� ������ ������ �����

  xtl::com_ptr<SwapChainColorBuffer> color_buffer (CreateColorBuffer (depth_stencil_buffer), false);
  
  shadow_color_buffers.push_front (color_buffer.get ());

  try
  {
    color_buffer->RegisterDestroyHandler (list_remover<ColorBufferList> (shadow_color_buffers, shadow_color_buffers.begin ()), *this);
  }
  catch (...)
  {
    shadow_color_buffers.pop_front ();
    throw;
  }

  color_buffer->AddRef ();

  return color_buffer.get ();
}

SwapChainDepthStencilBuffer* DefaultOutputStageResourceFactory::GetShadowBuffer (SwapChainColorBuffer* color_buffer)
{
  ContextManager& manager    = GetContextManager ();
  ISwapChain*     swap_chain = color_buffer->GetSwapChain ();
  
    //������� ����� ����������� �� ������� ����� �������-��������
    
  for (DepthStencilBufferList::iterator iter=shadow_depth_stencil_buffers.begin (), end=shadow_depth_stencil_buffers.end (); iter!=end; ++iter)
    if (manager.IsCompatible ((*iter)->GetContextId (), swap_chain))
    {
        //�����������: ����������� ���������� ���� � ������ ������ ��� ��������� ���������� ������

      shadow_depth_stencil_buffers.splice (shadow_depth_stencil_buffers.begin (), shadow_depth_stencil_buffers, iter);

      return *iter;
    }
    
    //�������� ������ ������ �������-��������
    
  xtl::com_ptr<SwapChainDepthStencilBuffer> depth_stencil_buffer (CreateDepthStencilBuffer (color_buffer), false);
  
  shadow_depth_stencil_buffers.push_front (depth_stencil_buffer.get ());

  try
  {
    depth_stencil_buffer->RegisterDestroyHandler (list_remover<DepthStencilBufferList> (shadow_depth_stencil_buffers,
      shadow_depth_stencil_buffers.begin ()), *this);
  }
  catch (...)
  {
    shadow_depth_stencil_buffers.pop_front ();
    throw;
  }

  depth_stencil_buffer->AddRef ();

  return depth_stencil_buffer.get ();
}

void DefaultOutputStageResourceFactory::GetShadowBuffers (SwapChainColorBuffer*& color_buffer, SwapChainDepthStencilBuffer*& depth_stencil_buffer)
{
  ContextManager& manager = GetContextManager ();

  if (!shadow_color_buffers.empty () && !shadow_depth_stencil_buffers.empty ())
  {
    for (ColorBufferList::iterator i=shadow_color_buffers.begin (), end=shadow_color_buffers.end (); i!=end; ++i)
    {
      ISwapChain* swap_chain = (*i)->GetSwapChain ();
      
      for (DepthStencilBufferList::iterator j=shadow_depth_stencil_buffers.begin (), end=shadow_depth_stencil_buffers.end (); j!=end; ++j)
        if (manager.IsCompatible ((*j)->GetContextId (), swap_chain))
        {
          color_buffer         = *i;
          depth_stencil_buffer = *j;

            //�����������: ����������� ��������� ����� � ������ ������ ��� ��������� ���������� ������

          shadow_color_buffers.splice (shadow_color_buffers.begin (), shadow_color_buffers, i);
          shadow_depth_stencil_buffers.splice (shadow_depth_stencil_buffers.begin (), shadow_depth_stencil_buffers, j);
          
          return;
        }
    }
  }

  if (!shadow_color_buffers.empty ())
  {
    color_buffer         = shadow_color_buffers.front ();
    depth_stencil_buffer = GetShadowBuffer (color_buffer);
    
    return;
  }

  if (!shadow_depth_stencil_buffers.empty ())
  {
    depth_stencil_buffer = shadow_depth_stencil_buffers.front ();
    color_buffer         = GetShadowBuffer (depth_stencil_buffer);
    
    return;
  }

    //�������� ����� ���� �������

  xtl::com_ptr<ISwapChain>           swap_chain (manager.CreateCompatibleSwapChain (GetDefaultSwapChain ()), false);
  xtl::com_ptr<SwapChainColorBuffer> new_color_buffer (new SwapChainColorBuffer (manager, swap_chain.get (), 1), false);

  color_buffer         = new_color_buffer.get ();
  depth_stencil_buffer = GetShadowBuffer (color_buffer);

  new_color_buffer->AddRef ();
}

SwapChainFrameBuffer* DefaultOutputStageResourceFactory::CreateShadowFrameBuffer ()
{
  try
  {
    SwapChainColorBuffer*        color_buffer = 0;
    SwapChainDepthStencilBuffer* depth_stencil_buffer = 0;
    
    GetShadowBuffers (color_buffer, depth_stencil_buffer);
    
    return new SwapChainFrameBuffer (GetContextManager (), color_buffer, depth_stencil_buffer);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateShadowFrameBuffer");
    
    throw;
  }
}

/*
    �������� �������� ������ ������    
*/

ITexture* DefaultOutputStageResourceFactory::CreateTexture (const TextureDesc& desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::DefaultOutputStageResourceFactory::CreateTexture";
  
  switch (desc.dimension)
  {
    case TextureDimension_2D:
      break;
    case TextureDimension_1D:
    case TextureDimension_3D:
    case TextureDimension_Cubemap:
      RaiseNotSupported (METHOD_NAME, "Can not create output-stage texture with dimension %s", get_name (desc.dimension));
      return 0;
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.dimension", desc.dimension);
      return 0;
  }

  if (desc.generate_mips_enable)
    RaiseNotSupported (METHOD_NAME, "Can not create output-stage texture with automatic mipmap generation");

  switch (desc.format)
  {
    case PixelFormat_RGB8:
    case PixelFormat_RGBA8:
    {
      SwapChainDesc swap_chain_desc;
      ISwapChain*   default_swap_chain = GetDefaultSwapChain ();
      
      default_swap_chain->GetDesc (swap_chain_desc);      
      
      swap_chain_desc.frame_buffer.width  = desc.width;
      swap_chain_desc.frame_buffer.height = desc.height;

      xtl::com_ptr<ISwapChain> swap_chain (GetContextManager ().CreateCompatibleSwapChain (default_swap_chain, swap_chain_desc), false);

      return new SwapChainColorBuffer (GetContextManager (), swap_chain.get (), 1);
    }
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8:
    case PixelFormat_S8:
    {
      RaiseNotImplemented ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateTexture");
      return 0;
    }
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_LA8:
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
      RaiseNotSupported (METHOD_NAME, "Can not create output-stage texture with format=%s", get_name (desc.format));
      return 0;
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.format", desc.format);
      return 0;
  }
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
    SwapChainFrameBuffer* frame_buffer = CreateShadowFrameBuffer ();
    
    frame_buffer->SetBuffersState (false, false);
    
    return frame_buffer;
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
    SwapChainFrameBuffer* frame_buffer = new SwapChainFrameBuffer (GetContextManager (), GetShadowBuffer (depth_stencil_buffer),
                                                                   depth_stencil_buffer);

    frame_buffer->SetBuffersState (false, true);
    
    return frame_buffer;
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(NullView,SwapChainDepthStencilBuffer*)");
    
    throw;
  }
}

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer (NullView, IBindableTexture* texture, const ViewDesc& desc)
{
  try
  {
    SwapChainFrameBuffer* frame_buffer = CreateShadowFrameBuffer ();

    frame_buffer->SetBuffersState  (false, true);
    frame_buffer->SetRenderTargets (0, 0, texture, &desc);

    return frame_buffer;    
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(NullView,IBindableTexture*)");

    throw;
  }
}

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer (SwapChainColorBuffer* color_buffer, NullView)
{
  try
  {
    SwapChainFrameBuffer* frame_buffer = new SwapChainFrameBuffer (GetContextManager (), color_buffer, GetShadowBuffer (color_buffer));
    
    frame_buffer->SetBuffersState (true, false);
    
    return frame_buffer;
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

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer
 (SwapChainColorBuffer* color_buffer,
  IBindableTexture*     texture,
  const ViewDesc&       desc)
{
  try
  {
    SwapChainFrameBuffer* frame_buffer = new SwapChainFrameBuffer (GetContextManager (), color_buffer, GetShadowBuffer (color_buffer));

    frame_buffer->SetRenderTargets (0, 0, texture, &desc);

    return frame_buffer;
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(SwapChainColorBuffer*,IBindableTexture*)");

    throw;
  }
}

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer (IBindableTexture* texture, const ViewDesc& desc, NullView)
{
  try
  {
    SwapChainFrameBuffer* frame_buffer = CreateShadowFrameBuffer ();

    frame_buffer->SetBuffersState  (true, false);
    frame_buffer->SetRenderTargets (texture, &desc, 0, 0);

    return frame_buffer;    
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(IBindableTexture*,NullView)");
    
    throw;
  }
}

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer
 (IBindableTexture*            texture,
  const ViewDesc&              desc,
  SwapChainDepthStencilBuffer* depth_stencil_buffer)
{
  try
  {
    SwapChainFrameBuffer* frame_buffer = new SwapChainFrameBuffer (GetContextManager (), GetShadowBuffer (depth_stencil_buffer),
                                                                   depth_stencil_buffer);

    frame_buffer->SetRenderTargets (texture, &desc, 0, 0);

    return frame_buffer;
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(IBindableTexture*,SwapChainDepthStencilBuffer*)");

    throw;
  }
}

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer
 (IBindableTexture* render_target_texture,
  const ViewDesc&   render_target_desc,
  IBindableTexture* depth_stencil_texture,
  const ViewDesc&   depth_stencil_desc)
{
  try
  {
    SwapChainFrameBuffer* frame_buffer = CreateShadowFrameBuffer ();
    
    frame_buffer->SetRenderTargets (render_target_texture, &render_target_desc, depth_stencil_texture, &depth_stencil_desc);
    
    return frame_buffer;
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(IBindableTexture*,IBindableTexture*)");

    throw;
  }
}

/*
    �������� ������� �������� ��������� ������ �� ���������
*/

OutputStageResourceFactory* OutputStageResourceFactory::CreateDefaultFactory (const ContextManager& manager)
{
  return new DefaultOutputStageResourceFactory (manager);
}
