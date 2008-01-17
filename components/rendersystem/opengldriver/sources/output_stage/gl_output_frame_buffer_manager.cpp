#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    ����������� / ����������
*/

FrameBufferManager::FrameBufferManager (const ContextManager& manager, ISwapChain* in_default_swap_chain)
  : ContextObject (manager), default_swap_chain (in_default_swap_chain)
{
}

FrameBufferManager::~FrameBufferManager ()
{
}

/*
    ������ � �������� ��������
*/

FrameBufferManager::ColorBufferPtr FrameBufferManager::CreateColorBuffer (SwapChainDepthStencilBuffer* depth_stencil_buffer)
{
  xtl::com_ptr<ISwapChain> swap_chain (GetContextManager ().CreateCompatibleSwapChain (depth_stencil_buffer->GetContextId ()), false);

  return ColorBufferPtr (new SwapChainColorBuffer (GetContextManager (), swap_chain.get (), 1), false);
}

FrameBufferManager::DepthStencilBufferPtr FrameBufferManager::CreateDepthStencilBuffer (SwapChainColorBuffer* color_buffer)
{
  return DepthStencilBufferPtr (new SwapChainDepthStencilBuffer (GetContextManager (), color_buffer->GetSwapChain ()), false);
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

FrameBufferManager::ColorBufferPtr FrameBufferManager::GetShadowBuffer (SwapChainDepthStencilBuffer* depth_stencil_buffer)
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

  ColorBufferPtr color_buffer = CreateColorBuffer (depth_stencil_buffer);
  
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

  return color_buffer;
}

FrameBufferManager::DepthStencilBufferPtr FrameBufferManager::GetShadowBuffer (SwapChainColorBuffer* color_buffer)
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
    
  DepthStencilBufferPtr depth_stencil_buffer = CreateDepthStencilBuffer (color_buffer);
  
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

  return depth_stencil_buffer;
}

void FrameBufferManager::GetShadowBuffers (ColorBufferPtr& color_buffer, DepthStencilBufferPtr& depth_stencil_buffer)
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
    depth_stencil_buffer = GetShadowBuffer (color_buffer.get ());
    
    return;
  }

  if (!shadow_depth_stencil_buffers.empty ())
  {
    depth_stencil_buffer = shadow_depth_stencil_buffers.front ();
    color_buffer         = GetShadowBuffer (depth_stencil_buffer.get ());
    
    return;
  }

    //�������� ����� ���� �������

  SwapChainPtr          swap_chain (GetContextManager ().CreateCompatibleSwapChain (GetDefaultSwapChain ()), false);
  ColorBufferPtr        new_color_buffer (new SwapChainColorBuffer (manager, swap_chain.get (), 1), false);  
  DepthStencilBufferPtr new_depth_stencil_buffer = GetShadowBuffer (new_color_buffer.get ());

  shadow_color_buffers.push_front (new_color_buffer.get ());

  try
  {
    new_color_buffer->RegisterDestroyHandler (list_remover<ColorBufferList> (shadow_color_buffers, shadow_color_buffers.begin ()), *this);
  }
  catch (...)
  {
    shadow_color_buffers.pop_front ();
    throw;
  }

  color_buffer         = new_color_buffer;
  depth_stencil_buffer = new_depth_stencil_buffer;
}

SwapChainFrameBuffer* FrameBufferManager::CreateShadowFrameBuffer ()
{
  try
  {
    ColorBufferPtr        color_buffer;
    DepthStencilBufferPtr depth_stencil_buffer;
    
    GetShadowBuffers (color_buffer, depth_stencil_buffer);
    
    return new SwapChainFrameBuffer (GetContextManager (), color_buffer.get (), depth_stencil_buffer.get ());
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateShadowFrameBuffer");
    
    throw;
  }
}

/*
    �������� �������� ������ ������    
*/

ITexture* FrameBufferManager::CreateTexture (const TextureDesc& desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FrameBufferManager::CreateTexture";
  
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
      try
      {
        SwapChainDesc swap_chain_desc;
        ISwapChain*   default_swap_chain = GetDefaultSwapChain ();
        
        default_swap_chain->GetDesc (swap_chain_desc);
        
        swap_chain_desc.frame_buffer.width  = desc.width;
        swap_chain_desc.frame_buffer.height = desc.height;

        SwapChainPtr swap_chain (GetContextManager ().CreateCompatibleSwapChain (default_swap_chain, swap_chain_desc), false);

        return new SwapChainColorBuffer (GetContextManager (), swap_chain.get (), 1);
      }
      catch (common::Exception& exception)
      {
        exception.Touch (METHOD_NAME);
        
        throw;
      }
    }
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8:
    {
      try
      {
        ISwapChain* default_swap_chain = GetDefaultSwapChain ();

        return new SwapChainDepthStencilBuffer (GetContextManager (), default_swap_chain, desc.width, desc.height);
      }
      catch (common::Exception& exception)
      {
        exception.Touch (METHOD_NAME);
        
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
      RaiseNotSupported (METHOD_NAME, "Can not create output-stage texture with format=%s", get_name (desc.format));
      return 0;
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.format", desc.format);
      return 0;
  }
}

ITexture* FrameBufferManager::CreateRenderTargetTexture (ISwapChain* swap_chain, size_t buffer_index)
{
  try
  {
    return new SwapChainColorBuffer (GetContextManager (), swap_chain, buffer_index);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateRenderTargetTexture");

    throw;
  }
}

ITexture* FrameBufferManager::CreateDepthStencilTexture (ISwapChain* swap_chain)
{
  try
  {
    return new SwapChainDepthStencilBuffer (GetContextManager (), swap_chain);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateDepthStencilTexture");

    throw;
  }
}

/*
    ���������� �������� ������� �����
*/

template <class T>
FrameBuffer* FrameBufferManager::CreateFrameBufferImpl
 (const T&        render_target,
  const ViewDesc& render_target_desc,
  View*           depth_stencil_view)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FrameBufferManager::CreateFrameBuffer";

  ViewType depth_stencil_type = depth_stencil_view ? depth_stencil_view->GetType () : ViewType_Null;

  switch (depth_stencil_type)
  {
    case ViewType_Null:
    {
      ViewDesc null_view_desc;
      
      memset (&null_view_desc, 0, sizeof (null_view_desc));
      
      return CreateFrameBuffer (render_target, render_target_desc, NullView (), null_view_desc);
    }
    case ViewType_SwapChainColorBuffer:
      RaiseNotSupported (METHOD_NAME, "Unsupported depth-stencil view type 'ViewType_SwapChainColorBuffer'");
      return 0;
    case ViewType_SwapChainDepthStencilBuffer:
    {
      ViewDesc depth_stencil_desc;

      depth_stencil_view->GetDesc (depth_stencil_desc);      

      return CreateFrameBuffer (render_target, render_target_desc, depth_stencil_view->GetSwapChainDepthStencilBuffer (),
                                depth_stencil_desc);
    }
    case ViewType_Texture:
    {
      ViewDesc depth_stencil_desc;
      
      depth_stencil_view->GetDesc (depth_stencil_desc);
      
      return CreateFrameBuffer (render_target, render_target_desc, depth_stencil_view->GetBindableTexture (), depth_stencil_desc);
    }
    default:
      RaiseNotSupported (METHOD_NAME, "Unsupported depth-stencil view type '%s'", typeid (depth_stencil_view->GetTexture ()).name ());
      return 0;
  }
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer (View* render_target_view, View* depth_stencil_view)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FrameBufferManager::CreateFrameBuffer";

  ViewType render_target_type = render_target_view ? render_target_view->GetType () : ViewType_Null;

  switch (render_target_type)
  {
    case ViewType_Null:
    {
      ViewDesc null_view_desc;
      
      memset (&null_view_desc, 0, sizeof (null_view_desc));
      
      return CreateFrameBufferImpl (NullView (), null_view_desc, depth_stencil_view);
    }
    case ViewType_SwapChainColorBuffer:
    {
      ViewDesc render_target_desc;
      
      render_target_view->GetDesc (render_target_desc);      
      
      return CreateFrameBufferImpl (render_target_view->GetSwapChainColorBuffer (), render_target_desc, depth_stencil_view);
    }
    case ViewType_SwapChainDepthStencilBuffer:
      RaiseNotSupported (METHOD_NAME, "Unsupported render-target view type 'ViewType_SwapChainDepthStencilBuffer'");
      return 0;
    case ViewType_Texture:
    {
      ViewDesc render_target_desc;
      
      render_target_view->GetDesc (render_target_desc);
      
      return CreateFrameBufferImpl (render_target_view->GetBindableTexture (), render_target_desc, depth_stencil_view);
    }
    default:
      RaiseNotSupported (METHOD_NAME, "Unsupported render-target view type '%s'", typeid (render_target_view->GetTexture ()).name ());
      return 0;
  }
}

/*
    �������� ������� ����� � ��������� ������������
*/

FrameBuffer* FrameBufferManager::CreateFrameBuffer (NullView, const ViewDesc&, NullView, const ViewDesc&)
{
  try
  {
    SwapChainFrameBuffer* frame_buffer = CreateShadowFrameBuffer ();
    
    frame_buffer->SetBuffersState (false, false);
    
    return frame_buffer;
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(NullView,NullView)");
    
    throw;
  }
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer (NullView, const ViewDesc&, SwapChainDepthStencilBuffer* depth_stencil_buffer, const ViewDesc&)
{
  try
  {
    SwapChainFrameBuffer* frame_buffer = new SwapChainFrameBuffer (GetContextManager (), GetShadowBuffer (depth_stencil_buffer).get (),
                                                                   depth_stencil_buffer);

    frame_buffer->SetBuffersState (false, true);
    
    return frame_buffer;
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(NullView,SwapChainDepthStencilBuffer*)");
    
    throw;
  }
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer (NullView, const ViewDesc&, IBindableTexture* texture, const ViewDesc& desc)
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
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(NullView,IBindableTexture*)");

    throw;
  }
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer (SwapChainColorBuffer* color_buffer, const ViewDesc&, NullView, const ViewDesc&)
{
  try
  {
    SwapChainFrameBuffer* frame_buffer = new SwapChainFrameBuffer (GetContextManager (), color_buffer,
                                                                   GetShadowBuffer (color_buffer).get ());
    
    frame_buffer->SetBuffersState (true, false);
    
    return frame_buffer;
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(SwapChainColorBuffer*,NullView)");

    throw;
  }
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer (SwapChainColorBuffer* color_buffer, const ViewDesc&, SwapChainDepthStencilBuffer* depth_stencil_buffer, const ViewDesc&)
{
  try
  {
    return new SwapChainFrameBuffer (GetContextManager (), color_buffer, depth_stencil_buffer);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(SwapChainColorBuffer*,SwapChainDepthStencilBuffer*)");

    throw;
  }
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer (SwapChainColorBuffer* color_buffer, const ViewDesc&, IBindableTexture* texture, const ViewDesc& desc)
{
  try
  {
    SwapChainFrameBuffer* frame_buffer = new SwapChainFrameBuffer (GetContextManager (), color_buffer,
                                                                   GetShadowBuffer (color_buffer).get ());

    frame_buffer->SetRenderTargets (0, 0, texture, &desc);

    return frame_buffer;
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(SwapChainColorBuffer*,IBindableTexture*)");

    throw;
  }
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer (IBindableTexture* texture, const ViewDesc& desc, NullView, const ViewDesc&)
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
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(IBindableTexture*,NullView)");
    
    throw;
  }
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer (IBindableTexture* texture, const ViewDesc& desc, SwapChainDepthStencilBuffer* depth_stencil_buffer, const ViewDesc&)
{
  try
  {
    SwapChainFrameBuffer* frame_buffer = new SwapChainFrameBuffer (GetContextManager (), GetShadowBuffer (depth_stencil_buffer).get (),
                                                                   depth_stencil_buffer);

    frame_buffer->SetRenderTargets (texture, &desc, 0, 0);

    return frame_buffer;
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(IBindableTexture*,SwapChainDepthStencilBuffer*)");

    throw;
  }
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer
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
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(IBindableTexture*,IBindableTexture*)");

    throw;
  }
}
