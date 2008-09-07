#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
===================================================================================================
    �������
===================================================================================================
*/

namespace
{

/*
    ��������� ��� ������ �����
*/

struct FrameBufferHolder: public xtl::trackable, public xtl::reference_counter
{
  View*                       render_target_view; //������� ����������� ������ �����
  View*                       depth_stencil_view; //������� ����������� ������ �������-��������
  stl::auto_ptr<IFrameBuffer> frame_buffer;       //����� �����

  FrameBufferHolder (View* in_render_target_view, View* in_depth_stencil_view, FrameBufferManager& manager) :
    render_target_view (in_render_target_view),
    depth_stencil_view (in_depth_stencil_view),
    frame_buffer       (manager.CreateFrameBuffer (in_render_target_view, in_depth_stencil_view)) { }
};

/*
    ��������� ��� ��������� ������� �����
*/

struct FrameBufferManagerHolder
{
  FrameBufferManager frame_buffer_manager;  //�������� ������� �����

  FrameBufferManagerHolder (const ContextManager& manager, ISwapChain* default_swap_chain) : frame_buffer_manager (manager, default_swap_chain) {}
  ~FrameBufferManagerHolder () { frame_buffer_manager.UnregisterAllCreaters (); }  
};

/*
    �������� ��������� ��������� ������� ����� ��������� OpenGL
*/

class FrameBufferManagerState: public IStageState
{
  public:  
///������������
    FrameBufferManagerState (FrameBufferManagerState* in_main_state)
    {
      Init (in_main_state, 0);
    }

    FrameBufferManagerState (ContextObject* in_owner)
    {
      Init (0, in_owner);
    }

///��������� �������� ����������� ������ �����
    void SetRenderTargetView (View* in_render_target_view)
    {
      if (render_target_view == in_render_target_view)
        return;

      render_target_view = in_render_target_view;
      
      UpdateNotify ();
    }
    
///��������� �������� ����������� ������ �����
    View* GetRenderTargetView () { return render_target_view.get (); }
    
///��������� �������� ����������� ������ ������������� ���������
    void SetDepthStencilView (View* in_depth_stencil_view)
    {
      if (depth_stencil_view == in_depth_stencil_view)
        return;

      depth_stencil_view = in_depth_stencil_view;

      UpdateNotify ();
    }    

///��������� �������� ����������� ������ ������������� ���������
    View* GetDepthStencilView () { return depth_stencil_view.get (); }    

///��������� ������� ������� ������
    void SetViewport (const Viewport& in_viewport)
    {
      viewport                  = in_viewport;
      need_recalc_viewport_hash = true;

      UpdateNotify ();
    }
    
///��������� ������� ������� ������
    const Viewport& GetViewport () { return viewport; }
    
///��������� ������� ������� ���������
    void SetScissor (const Rect& in_scissor)
    {
      scissor                  = in_scissor;
      need_recalc_scissor_hash = true;

      UpdateNotify ();
    }

///��������� ������� ������� ���������
    const Rect& GetScissor () { return scissor; }

///��������� ���� ������� ������
    size_t GetViewportHash ()
    {
      if (need_recalc_viewport_hash)
      {
        viewport_hash             = crc32 (&viewport, sizeof viewport);
        need_recalc_viewport_hash = false;
      }

      return viewport_hash;
    }

///��������� ���� ������� ���������
    size_t GetScissorHash () const
    {
      if (need_recalc_scissor_hash)
      {
        scissor_hash             = crc32 (&scissor, sizeof scissor);
        need_recalc_scissor_hash = false;
      }

      return scissor_hash;
    }    

///������ ���������
    void Capture (const StateBlockMask& mask)
    {
      if (main_state)
        Copy (*main_state, mask);
    }
    
///�������������� ���������
    void Apply (const StateBlockMask& mask)
    {
      if (main_state)
        main_state->Copy (*this, mask);
    }

  private:    
///����������� ���������
    void Copy (FrameBufferManagerState& source, const StateBlockMask& mask)
    {
      if (mask.os_render_target_view)
        SetRenderTargetView (source.GetRenderTargetView ());

      if (mask.os_depth_stencil_view)
        SetDepthStencilView (source.GetDepthStencilView ());

      if (mask.rs_viewport)
        SetViewport (source.GetViewport ());

      if (mask.rs_scissor)
        SetScissor (source.GetScissor ());        
    }
    
///���������� �� ���������� ��������� ������
    void UpdateNotify ()
    {
      if (!owner)
        return;

      owner->GetContextManager ().StageRebindNotify (Stage_FrameBufferManager);
      owner->GetContextManager ().StageRebindNotify (Stage_Output); ////������!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }

  private:
    FrameBufferManagerState (const FrameBufferManagerState&); //no impl
    
    void Init (FrameBufferManagerState* in_main_state, ContextObject* in_owner)
    {
      owner                     = in_owner;
      main_state                = in_main_state;
      need_recalc_viewport_hash = true;
      need_recalc_scissor_hash  = true;

      memset (&viewport, 0, sizeof viewport);
      memset (&scissor, 0, sizeof scissor);
    }

  private:    
    typedef xtl::trackable_ptr<View>                    ViewPtr;
    typedef xtl::trackable_ptr<FrameBufferManagerState> FrameBufferManagerStatePtr;

  private:
    ContextObject*             owner;                     //�������� ��������� ������
    FrameBufferManagerStatePtr main_state;                //�������� ��������� ������
    ViewPtr                    render_target_view;        //������� ����������� ������ �����
    ViewPtr                    depth_stencil_view;        //������� ����������� ������ ������������� ���������
    Viewport                   viewport;                  //������� ������
    Rect                       scissor;                   //������� ���������
    size_t                     viewport_hash;             //��� ������� ������
    size_t                     scissor_hash;              //��� ������� ���������
    bool                       need_recalc_viewport_hash; //���� ������������� ��������� ���� ������� ������
    bool                       need_recalc_scissor_hash;  //���� ������������� ��������� ���� ������� ���������
};

}

/*
===================================================================================================
    FrameBufferManager
===================================================================================================
*/

/*
    �������� ���������� ��������� ������� ����� ��������� OpenGL
*/

struct FrameBufferManager::Impl: public ContextObject, public FrameBufferManagerHolder, public FrameBufferManagerState
{
  public:
    Impl (ContextManager& context_manager, ISwapChain* swap_chain) :
      ContextObject (context_manager),
      FrameBufferManagerHolder (context_manager, swap_chain),
      FrameBufferManagerState (static_cast<ContextObject*> (this)),
      need_update_render_targets (false)
    {
        //����������� ��������� ������� ����� �� ���������
        
      register_swap_chain_manager (frame_buffer_manager);

        //������������� �����������

      SwapChainDesc swap_chain_desc;

      swap_chain->GetDesc (swap_chain_desc);        

      xtl::com_ptr<ITexture> color_texture (frame_buffer_manager.CreateColorBuffer (swap_chain, swap_chain_desc.buffers_count > 1 ? 1 : 0), false),
                             depth_stencil_texture (frame_buffer_manager.CreateDepthStencilBuffer (swap_chain), false);
                             
      ViewDesc view_desc;

      memset (&view_desc, 0, sizeof (view_desc));

      default_render_target_view = ViewPtr (CreateView (color_texture.get (), view_desc), false);
      default_depth_stencil_view = ViewPtr (CreateView (depth_stencil_texture.get (), view_desc), false);    

      SetRenderTargets (default_render_target_view.get (), default_depth_stencil_view.get ());
      
        //��������� �������� ������ ������

      BindRenderTargets ();      
      
        //����������� ��������������� ��������� ������� �����
        
      if (GetCaps ().has_ext_framebuffer_object)
        register_fbo_manager (frame_buffer_manager);        

        //������������� �������� ��������� � ������        

      Viewport default_viewport;
      Rect     default_scissor;

      memset (&default_viewport, 0, sizeof default_viewport);
      memset (&default_scissor, 0, sizeof default_scissor);

      default_viewport.width     = swap_chain_desc.frame_buffer.width;
      default_viewport.height    = swap_chain_desc.frame_buffer.height;
      default_viewport.min_depth = 0.0f;
      default_viewport.max_depth = 1.0f;

      default_scissor.width  = swap_chain_desc.frame_buffer.width;
      default_scissor.height = swap_chain_desc.frame_buffer.height;
      
        //������������� ����� �������, ������� ����� �������
        
      clear_views_mask = ~0;

      if (!swap_chain_desc.frame_buffer.color_bits && !swap_chain_desc.frame_buffer.alpha_bits)
        clear_views_mask &= ~ClearFlag_RenderTarget;

      if (!swap_chain_desc.frame_buffer.depth_bits)
        clear_views_mask &= ~ClearFlag_Depth;

      if (!swap_chain_desc.frame_buffer.stencil_bits)
        clear_views_mask &= ~ClearFlag_Stencil;

        //��������� ��������� �� ���������

      SetViewport (default_viewport);
      SetScissor  (default_scissor);
    }
    
      //�������� �����������
    View* CreateView (ITexture* texture, const ViewDesc& desc)
    {
      ViewPtr view (new View (GetContextManager (), texture, desc), false);

      AddView (view);

      view->AddRef ();

      return view.get ();
    }
    
      //��������� ������� �����������
    void SetRenderTargets (IView* in_render_target_view, IView* in_depth_stencil_view)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::FrameBufferManager::Impl::SetRenderTargets";
      
        //���������� ����� �����������

      View *render_target_view = cast_object<View> (GetContextManager (), in_render_target_view, METHOD_NAME, "render_target_view"),
           *depth_stencil_view = cast_object<View> (GetContextManager (), in_depth_stencil_view, METHOD_NAME, "depth_stencil_view");

        //���������� �������� ��������� ������

      SetRenderTargetView (render_target_view);
      SetDepthStencilView (depth_stencil_view);

        //��������� ����� ������������� ���������� ������� ������� ���������

      need_update_render_targets = true;
    }

      //���������� ������� ������� ����� � �������-��������
    void ClearViews (size_t clear_flags, const Color4f& color, float depth, unsigned char stencil)
    {
      try
      {
          //���������� �������, ������� ����� �������

        clear_flags &= clear_views_mask;

          //��������� ����� ���������� � ��������� ���������

        BindRenderTargets ();

          //��������� �������� ��� ����������

        size_t* context_cache = &GetContextDataTable (Stage_Output)[0];

        GLbitfield mask = 0;
        
        bool need_restore_color_write_mask   = false,
             need_restore_depth_write_mask   = false,
             need_restore_stencil_write_mask = false,
             need_restore_scissor_test       = false;
             
          //��������� ���������� ������� ������ �����
        
        if (clear_flags & ClearFlag_RenderTarget)
        {
          size_t clear_color_hash = crc32 (&color, sizeof color);
          
          if (context_cache [FrameBufferManagerCache_ClearColorHash] != clear_color_hash)
          {
            glClearColor (color.red, color.green, color.blue, color.alpha);
            
            context_cache [FrameBufferManagerCache_ClearColorHash] = clear_color_hash;
          }
          
          if (context_cache [FrameBufferManagerCache_ColorWriteMask] != ColorWriteFlag_All)
          {
            glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            
            need_restore_color_write_mask = true;
          }
          
          mask |= GL_COLOR_BUFFER_BIT;
        }
        
          //��������� ���������� ������� ������ �������

        if (clear_flags & ClearFlag_Depth)
        {
          size_t clear_depth_hash = crc32 (&depth, sizeof depth);
          
          if (context_cache [FrameBufferManagerCache_ClearDepthHash] != clear_depth_hash)
          {
            glClearDepth (depth);
            
            context_cache [FrameBufferManagerCache_ClearDepthHash] = clear_depth_hash;
          }
          
          if (!context_cache [FrameBufferManagerCache_DepthWriteEnable])
          {
            glDepthMask (GL_TRUE);
            
            need_restore_depth_write_mask = true;
          }          

          mask |= GL_DEPTH_BUFFER_BIT;        
        }
        
          //��������� ���������� ������� ������ ���������

        if (clear_flags & ClearFlag_Stencil)
        {
          if (context_cache [FrameBufferManagerCache_ClearStencilValue] != stencil)
          {
            glClearStencil (stencil);
            
            context_cache [FrameBufferManagerCache_ClearStencilValue] = stencil;
          }

          if (context_cache [FrameBufferManagerCache_StencilWriteMask] != unsigned char (~0))
          {
            glStencilMask (~0);
            
            need_restore_stencil_write_mask = true;
          }                    

          mask |= GL_STENCIL_BUFFER_BIT;
        }
        
          //��������� ������� ���������
          
        if (clear_flags & ClearFlag_ViewportOnly)
        {          
          if (!context_cache [FrameBufferManagerCache_ScissorEnable])
          {
            glEnable (GL_SCISSOR_TEST);

            need_restore_scissor_test = true;
          }

          if (context_cache [FrameBufferManagerCache_ScissorHash] != GetViewportHash ())
          {
            const Viewport& viewport = GetViewport ();

            glScissor (viewport.x, viewport.y, viewport.width, viewport.height);          

            context_cache [FrameBufferManagerCache_ScissorHash] = GetViewportHash ();

            StageRebindNotify (Stage_Output);
          }
        }
        else
        {
            //���������� �� ���������� ������� ����������          

          GetCurrentFrameBuffer ().InvalidateRenderTargets ();
        }

          //������� ��������� �������
          
        glClear (mask);
        
          //�������������� ��������� OpenGL
        
        if (need_restore_color_write_mask)
        {
          size_t mask = context_cache [FrameBufferManagerCache_ColorWriteMask];

          glColorMask ((mask & ColorWriteFlag_Red) != 0, (mask & ColorWriteFlag_Green) != 0,
                      (mask & ColorWriteFlag_Blue) != 0, (mask & ColorWriteFlag_Alpha) != 0);
        }
        
        if (need_restore_depth_write_mask)
        {
          glDepthMask (GL_FALSE);
        }
        
        if (need_restore_stencil_write_mask)
        {
          glStencilMask (context_cache [FrameBufferManagerCache_StencilWriteMask]);
        }
        
        if (need_restore_scissor_test)
        {
          glDisable (GL_SCISSOR_TEST);
        }

        CheckErrors ("glClear");
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::FrameBufferManager::ClearViews");

        throw;
      }
    }    

      //���������� ������� ������� ���������
    void UpdateRenderTargets ()
    {
      if (updatable_frame_buffer_holder)
        updatable_frame_buffer_holder->frame_buffer->UpdateRenderTargets ();

      need_update_render_targets = false;
    }

      //��������� ��������� ������ � �������� OpenGL
    void Bind ()
    {
      try
      {      
          //��������� ����� ����������
        
        BindRenderTargets ();

          //��������� �������� ������ � ���������

        size_t *context_cache         = &GetContextDataTable (Stage_FrameBufferManager)[0],
               &current_viewport_hash = context_cache [FrameBufferManagerCache_ViewportHash],
               &current_scissor_hash  = context_cache [FrameBufferManagerCache_ScissorHash];

        if (current_viewport_hash != GetViewportHash ())
        {
          const Viewport& viewport = GetViewport ();

          glViewport   (viewport.x, viewport.y, viewport.width, viewport.height);
          glDepthRange (viewport.min_depth, viewport.max_depth);

          current_viewport_hash = GetViewportHash ();
        }
        
        if (current_scissor_hash != GetScissorHash ())
        {        
          const Rect& scissor  = GetScissor ();          

          glScissor (scissor.x, scissor.y, scissor.width, scissor.height);

          current_scissor_hash = GetScissorHash ();
        }

          //���������� �� ��������� ������� ������� ���������

        GetCurrentFrameBuffer ().InvalidateRenderTargets (GetViewport ());

          //�������� ������

        CheckErrors ("");
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::FrameBufferManager::Bind");
        throw;
      }
    }
    
  private:
    typedef xtl::intrusive_ptr<FrameBufferHolder> FrameBufferHolderPtr;
    typedef xtl::trackable_ptr<FrameBufferHolder> FrameBufferHolderTrackablePtr;
    typedef xtl::com_ptr<View>                    ViewPtr;
    typedef stl::list<FrameBufferHolderPtr>       FrameBufferHolderList;
    typedef xtl::com_ptr<ISwapChain>              SwapChainPtr;

  private:
      //�������� ������ ����� �����
    FrameBufferHolderPtr CreateFrameBufferHolder (View* render_target_view, View* depth_stencil_view)
    {
      try
      {
          //�������� ������������ ������������
          
        if (render_target_view)
        {
          if (!(render_target_view->GetBindFlags () & BindFlag_RenderTarget))
            throw xtl::format_exception<xtl::bad_argument> ("", "Render-target view has wrong bind flags %s", get_name ((BindFlag)render_target_view->GetBindFlags ()));
            
            //�������� ������������ ������� ��������
            
          ITexture* texture = render_target_view->GetTexture ();
            
          TextureDesc desc;
          
          texture->GetDesc (desc);
          
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
              throw xtl::format_not_supported_exception ("", "Unsupported render-target view texture format=%s", get_name (desc.format));
              break;
            default:
              throw xtl::make_argument_exception ("", "texture_desc.format", desc.format);
              break;
          }
        }

        if (depth_stencil_view)
        {
          if (!(depth_stencil_view->GetBindFlags () & BindFlag_DepthStencil))
            throw xtl::format_exception<xtl::bad_argument> ("", "Depth-stencil view has wrong bind flags %s", get_name ((BindFlag)depth_stencil_view->GetBindFlags ()));
            
            //�������� ������������ ������� ��������
            
          ITexture* texture = depth_stencil_view->GetTexture ();

          TextureDesc desc;

          texture->GetDesc (desc);

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
              throw xtl::format_not_supported_exception ("", "Unsupported depth-stencil view texture format=%s", get_name (desc.format));
              break;
            case PixelFormat_D16:
            case PixelFormat_D24X8:
            case PixelFormat_D24S8:
            case PixelFormat_S8:
              break;
            default:
              throw xtl::make_argument_exception ("", "texture_desc.format", desc.format);
              break;
          }
        }

        return FrameBufferHolderPtr (new FrameBufferHolder (render_target_view, depth_stencil_view, frame_buffer_manager), false);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::FrameBufferManager::Impl::CreateFrameBufferHolder");
        throw;
      }      
    }
  
      //��������� ������ ����� �� ������������
    FrameBufferHolder* GetFrameBufferHolder (View* render_target_view, View* depth_stencil_view)
    {            
        //����� ������ � ������ ��� ���������
      
      for (FrameBufferHolderList::iterator iter=frame_buffers.begin (), end=frame_buffers.end (); iter!=end; ++iter)
        if ((*iter)->render_target_view == render_target_view && (*iter)->depth_stencil_view == depth_stencil_view)
        {
            //�����������: ����������� ���������� ���� � ������ ������ ��� ��������� ���������� ������

          frame_buffers.splice (frame_buffers.begin (), frame_buffers, iter);

          return &**iter;
        }        
        
        //�������� ������ ������ �����

      FrameBufferHolderPtr frame_buffer_holder = CreateFrameBufferHolder (render_target_view, depth_stencil_view);

        //���������� ������ ������ ����� � ������ ���������

      frame_buffers.push_front (frame_buffer_holder);

      return &*frame_buffers.front ();
    }
    
      //���������� �����������
    void AddView (const xtl::com_ptr<View>& view)
    {
      RegisterDestroyHandler (xtl::bind (&Impl::RemoveView, this, view.get ()), GetTrackable ());
    }

      //�������� �����������
    void RemoveView (View* view)
    {
        //�������� ���� ������� �����, � ������� ������������ ��������� �����������
      
      for (FrameBufferHolderList::iterator iter=frame_buffers.begin (), end=frame_buffers.end (); iter!=end;)
      {
        if ((*iter)->render_target_view == view || (*iter)->depth_stencil_view == view)
        {
          FrameBufferHolderList::iterator tmp = iter;
          
          ++tmp;
          
          frame_buffers.erase (iter);
          
          iter = tmp;
        }
        else ++iter;
      }
    }
    
      //��������� �������� ������ �����
    IFrameBuffer& GetCurrentFrameBuffer ()
    {
      return *GetFrameBufferHolder (GetRenderTargetView (), GetDepthStencilView ())->frame_buffer;
    }    

      //��������� ������� ������� ������� ��������� � �������� OpenGL
      ///��������������!!!! ��� ����������� ���������
    void BindRenderTargets ()
    {
      if (need_update_render_targets)
        UpdateRenderTargets ();

      FrameBufferHolder* frame_buffer_holder = GetFrameBufferHolder (GetRenderTargetView (), GetDepthStencilView ());

      updatable_frame_buffer_holder = frame_buffer_holder;

      frame_buffer_holder->frame_buffer->Bind ();

      MakeContextCurrent ();
    }    

  private:
    FrameBufferHolderList         frame_buffers;                 //������ �����
    bool                          need_update_render_targets;    //����, ����������� �� ������������� ���������� ������� ������� ���������
    FrameBufferHolderTrackablePtr updatable_frame_buffer_holder; //����������� ������ �����
    ViewPtr                       default_render_target_view;    //����������� ������ ����� �� ���������
    ViewPtr                       default_depth_stencil_view;    //����������� ������ �������-�������� �� ���������
    size_t                        clear_views_mask;              //����� �������, ������� ����� ������� (���������� ��� ������ � Direct3D wrapper)
};

/*
    ����������� / ����������
*/

FrameBufferManager::FrameBufferManager (ContextManager& context_manager, ISwapChain* swap_chain)
  : impl (new Impl (context_manager, swap_chain))
{
}

FrameBufferManager::~FrameBufferManager ()
{
}

/*
    �������� ������� ��������� ����������
*/

IStageState* FrameBufferManager::CreateStageState ()
{
  return new FrameBufferManagerState (static_cast<FrameBufferManagerState*> (&*impl));
}

/*
    �������� �������
*/

ITexture* FrameBufferManager::CreateTexture (const TextureDesc& desc)
{
  try
  {
    return impl->frame_buffer_manager.CreateRenderBuffer (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::FrameBufferManager::CreateTexture");

    throw;
  }
}

ITexture* FrameBufferManager::CreateRenderTargetTexture (ISwapChain* swap_chain, size_t buffer_index)
{
  try
  {
    return impl->frame_buffer_manager.CreateColorBuffer (swap_chain, buffer_index);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::FrameBufferManager::CreateRenderTargetTexture");
    
    throw;
  }
}

ITexture* FrameBufferManager::CreateDepthStencilTexture (ISwapChain* swap_chain)
{
  try
  {
    return impl->frame_buffer_manager.CreateDepthStencilBuffer (swap_chain);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::FrameBufferManager::CreateDepthStencilTexture");
    
    throw;
  }
}

/*
    �������� �����������
*/

IView* FrameBufferManager::CreateView (ITexture* texture, const ViewDesc& desc)
{
  try
  {
    return impl->CreateView (texture, desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::FrameBufferManager::CreateView");

    throw;
  }
}

/*
    ����� ������� �����������
*/

void FrameBufferManager::SetRenderTargets (IView* render_target_view, IView* depth_stencil_view)
{
  try
  {
    return impl->SetRenderTargets (render_target_view, depth_stencil_view);          
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::FrameBufferManager::SetRenderTargets");

    throw;
  }
}

IView* FrameBufferManager::GetRenderTargetView () const
{
  return impl->GetRenderTargetView ();
}

IView* FrameBufferManager::GetDepthStencilView () const
{
  return impl->GetDepthStencilView ();
}

/*
    ��������� �������� ������ � ���������
*/

void FrameBufferManager::SetViewport (const Viewport& viewport)
{
  impl->SetViewport (viewport);
}

void FrameBufferManager::SetScissor (const Rect& scissor_rect)
{
  impl->SetScissor (scissor_rect);
}

const Viewport& FrameBufferManager::GetViewport () const
{
  return impl->GetViewport ();
}

const Rect& FrameBufferManager::GetScissor () const
{
  return impl->GetScissor ();
}


/*
    ������� ������� ���������
*/

void FrameBufferManager::ClearRenderTargetView (const Color4f& color)
{  
  impl->ClearViews (ClearFlag_RenderTarget, color, 0.0f, 0);
}

void FrameBufferManager::ClearDepthStencilView (size_t clear_flags, float depth, unsigned char stencil)
{
  Color4f clear_color;

  impl->ClearViews (ClearFlag_Depth | ClearFlag_Stencil, clear_color, depth, stencil);
}

void FrameBufferManager::ClearViews (size_t clear_flags, const Color4f& color, float depth, unsigned char stencil)
{
  impl->ClearViews (clear_flags, color, depth, stencil);
}

/*
    ���������� �� ��������� ������� ������� ��������� / ���������� ������� ������� ���������
*/

void FrameBufferManager::UpdateRenderTargets ()
{
  try
  {
    impl->UpdateRenderTargets ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::FrameBufferManager::UpdateRenderTargets");
    throw;
  }
}

/*
    ��������� ��������� ������ � �������� OpenGL
*/

void FrameBufferManager::Bind ()
{
  impl->Bind ();
}
