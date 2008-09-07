#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

namespace
{

/*
===================================================================================================
    �������� ��������� ��������� ������� ������� ����������
===================================================================================================    
*/

class RenderTargetManagerState: public IStageState
{
  public:  
///������������
    RenderTargetManagerState (RenderTargetManagerState* in_main_state)
    {
      Init (in_main_state, 0);
    }

    RenderTargetManagerState (ContextObject* in_owner)
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
    size_t GetScissorHash ()
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
    void Copy (RenderTargetManagerState& source, const StateBlockMask& mask)
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

      owner->GetContextManager ().StageRebindNotify (Stage_RenderTargetManager);
    }

  private:
    RenderTargetManagerState (const RenderTargetManagerState&); //no impl
    
    void Init (RenderTargetManagerState* in_main_state, ContextObject* in_owner)
    {
      owner                     = in_owner;
      main_state                = in_main_state;
      need_recalc_viewport_hash = true;
      need_recalc_scissor_hash  = true;

      memset (&viewport, 0, sizeof viewport);
      memset (&scissor, 0, sizeof scissor);
    }

  private:    
    typedef xtl::trackable_ptr<View>                     ViewPtr;
    typedef xtl::trackable_ptr<RenderTargetManagerState> RenderTargetManagerStatePtr;

  private:
    ContextObject*              owner;                     //�������� ��������� ������
    RenderTargetManagerStatePtr main_state;                //�������� ��������� ������
    ViewPtr                     render_target_view;        //������� ����������� ������ �����
    ViewPtr                     depth_stencil_view;        //������� ����������� ������ ������������� ���������
    Viewport                    viewport;                  //������� ������
    Rect                        scissor;                   //������� ���������
    size_t                      viewport_hash;             //��� ������� ������
    size_t                      scissor_hash;              //��� ������� ���������
    bool                        need_recalc_viewport_hash; //���� ������������� ��������� ���� ������� ������
    bool                        need_recalc_scissor_hash;  //���� ������������� ��������� ���� ������� ���������
};

}

/*
===================================================================================================
    �������� ���������� ��������� ������� ������� ����������
===================================================================================================
*/

struct RenderTargetManager::Impl: public ContextObject, public RenderTargetManagerState
{
  public:
///�����������
    Impl (const ContextManager& context_manager, ISwapChain* swap_chain) :
      ContextObject (context_manager),      
      RenderTargetManagerState (static_cast<ContextObject*> (this)),
      need_update_render_targets (false)
    {
        //����������� ���������� ������� �����

      register_swap_chain_manager (render_target_registry, GetContextManager (), swap_chain);

      if (GetCaps ().has_ext_framebuffer_object)
        register_fbo_manager (render_target_registry, GetContextManager (), swap_chain);
      
        //������������� �����������

      SwapChainDesc swap_chain_desc;

      swap_chain->GetDesc (swap_chain_desc);        

      xtl::com_ptr<ITexture> color_texture (render_target_registry.CreateColorBuffer (swap_chain, swap_chain_desc.buffers_count > 1 ? 1 : 0), false),
                             depth_stencil_texture (render_target_registry.CreateDepthStencilBuffer (swap_chain), false);

      ViewDesc view_desc;

      memset (&view_desc, 0, sizeof (view_desc));

      default_render_target_view = ViewPtr (CreateView (color_texture.get (), view_desc), false);
      default_depth_stencil_view = ViewPtr (CreateView (depth_stencil_texture.get (), view_desc), false);    

      SetRenderTargets (default_render_target_view.get (), default_depth_stencil_view.get ());

        //��������� �������� ������ �����

      BindRenderTargets ();      

        //������������� ����� �������, ������� ����� �������
        
      clear_views_mask = ~0;

      if (!swap_chain_desc.frame_buffer.color_bits && !swap_chain_desc.frame_buffer.alpha_bits)
        clear_views_mask &= ~ClearFlag_RenderTarget;

      if (!swap_chain_desc.frame_buffer.depth_bits)
        clear_views_mask &= ~ClearFlag_Depth;

      if (!swap_chain_desc.frame_buffer.stencil_bits)
        clear_views_mask &= ~ClearFlag_Stencil;

        //������������� ������� ������

      Viewport default_viewport;

      memset (&default_viewport, 0, sizeof default_viewport);

      default_viewport.width     = swap_chain_desc.frame_buffer.width;
      default_viewport.height    = swap_chain_desc.frame_buffer.height;
      default_viewport.min_depth = 0.0f;
      default_viewport.max_depth = 1.0f;
      
        //������������� ������� ���������
      
      SetViewport (default_viewport);      
      
      Rect default_scissor;      
      
      memset (&default_scissor, 0, sizeof default_scissor);      

      default_scissor.width  = swap_chain_desc.frame_buffer.width;
      default_scissor.height = swap_chain_desc.frame_buffer.height;        

      SetScissor (default_scissor);
    }
    
///��������� ������� ������� �����
    RenderTargetRegistry& GetRenderTargetRegistry () { return render_target_registry; }
    
///�������� �����������
    View* CreateView (ITexture* texture, const ViewDesc& desc)
    {
      ViewPtr view (new View (GetContextManager (), texture, desc), false);

      AddView (view);

      view->AddRef ();

      return view.get ();
    }
    
///��������� ������� �����������
    void SetRenderTargets (IView* in_render_target_view, IView* in_depth_stencil_view)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::RenderTargetManager::Impl::SetRenderTargets";
      
        //���������� ����� �����������

      View *render_target_view = cast_object<View> (GetContextManager (), in_render_target_view, METHOD_NAME, "render_target_view"),
           *depth_stencil_view = cast_object<View> (GetContextManager (), in_depth_stencil_view, METHOD_NAME, "depth_stencil_view");

        //���������� �������� ��������� ������

      SetRenderTargetView (render_target_view);
      SetDepthStencilView (depth_stencil_view);

        //��������� ����� ������������� ���������� ������� ������� ���������

      need_update_render_targets = true;
    }

///���������� ������� ������� ���������
    void UpdateRenderTargets ()
    {
      if (updatable_frame_buffer)
        updatable_frame_buffer->UpdateRenderTargets ();

      need_update_render_targets = false;
    }

///��������� ��������� ��������� � �������� OpenGL
    void Bind ()
    {
      try
      {      
          //��������� ����� ����������
        
        BindRenderTargets ();

          //��������� �������� ������ � ���������

        size_t *context_cache         = &GetContextDataTable (Stage_RenderTargetManager)[0],
               &current_viewport_hash = context_cache [RenderTargetManagerCache_ViewportHash],
               &current_scissor_hash  = context_cache [RenderTargetManagerCache_ScissorHash];

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
        exception.touch ("render::low_level::opengl::RenderTargetManager::Bind");
        throw;
      }
    }

///������� ������� ����� � �������-��������
    void ClearViews (size_t clear_flags, const Color4f& color, float depth, unsigned char stencil)
    {
      try
      {
          //���������� �������, ������� ����� �������

        clear_flags &= clear_views_mask;

          //��������� ����� ���������� � ��������� ���������

        BindRenderTargets ();

          //��������� �������� ��� ����������

        size_t *context_cache        = &GetContextDataTable (Stage_RenderTargetManager)[0],
               *context_common_cache = &GetContextDataTable (Stage_Common)[0];

        GLbitfield mask = 0;
        
        bool need_restore_color_write_mask   = false,
             need_restore_depth_write_mask   = false,
             need_restore_stencil_write_mask = false,
             need_restore_scissor_test       = false;
             
          //��������� ���������� ������� ������ �����
        
        if (clear_flags & ClearFlag_RenderTarget)
        {
          size_t clear_color_hash = crc32 (&color, sizeof color);
          
          if (context_cache [RenderTargetManagerCache_ClearColorHash] != clear_color_hash)
          {
            glClearColor (color.red, color.green, color.blue, color.alpha);
            
            context_cache [RenderTargetManagerCache_ClearColorHash] = clear_color_hash;
          }
          
          if (context_common_cache [CommonCache_ColorWriteMask] != ColorWriteFlag_All)
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
          
          if (context_cache [RenderTargetManagerCache_ClearDepthHash] != clear_depth_hash)
          {
            glClearDepth (depth);
            
            context_cache [RenderTargetManagerCache_ClearDepthHash] = clear_depth_hash;
          }
          
          if (!context_common_cache [CommonCache_DepthWriteEnable])
          {
            glDepthMask (GL_TRUE);
            
            need_restore_depth_write_mask = true;
          }          

          mask |= GL_DEPTH_BUFFER_BIT;        
        }
        
          //��������� ���������� ������� ������ ���������

        if (clear_flags & ClearFlag_Stencil)
        {
          if (context_cache [RenderTargetManagerCache_ClearStencilValue] != stencil)
          {
            glClearStencil (stencil);
            
            context_cache [RenderTargetManagerCache_ClearStencilValue] = stencil;
          }

          if (context_common_cache [CommonCache_StencilWriteMask] != unsigned char (~0))
          {
            glStencilMask (~0);
            
            need_restore_stencil_write_mask = true;
          }                    

          mask |= GL_STENCIL_BUFFER_BIT;
        }
        
          //��������� ������� ���������
          
        if (clear_flags & ClearFlag_ViewportOnly)
        {          
          if (!context_common_cache [CommonCache_ScissorEnable])
          {
            glEnable (GL_SCISSOR_TEST);

            need_restore_scissor_test = true;
          }

          if (context_cache [RenderTargetManagerCache_ScissorHash] != GetViewportHash ())
          {
            const Viewport& viewport = GetViewport ();

            glScissor (viewport.x, viewport.y, viewport.width, viewport.height);          

            context_cache [RenderTargetManagerCache_ScissorHash] = GetViewportHash ();

            StageRebindNotify (Stage_RenderTargetManager);
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
          size_t mask = context_common_cache [CommonCache_ColorWriteMask];

          glColorMask ((mask & ColorWriteFlag_Red) != 0, (mask & ColorWriteFlag_Green) != 0,
                      (mask & ColorWriteFlag_Blue) != 0, (mask & ColorWriteFlag_Alpha) != 0);
        }
        
        if (need_restore_depth_write_mask)
        {
          glDepthMask (GL_FALSE);
        }
        
        if (need_restore_stencil_write_mask)
        {
          glStencilMask (context_common_cache [CommonCache_StencilWriteMask]);
        }
        
        if (need_restore_scissor_test)
        {
          glDisable (GL_SCISSOR_TEST);
        }

        CheckErrors ("glClear");
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::RenderTargetManager::ClearViews");

        throw;
      }
    }    
    
  private:
    typedef xtl::trackable_ptr<IFrameBuffer> FrameBufferPtr;
    typedef xtl::com_ptr<View>               ViewPtr;

  private:
///���������� �����������
    void AddView (const xtl::com_ptr<View>& view)
    {
      RegisterDestroyHandler (xtl::bind (&Impl::RemoveView, this, view.get ()), GetTrackable ());
    }

///�������� �����������
    void RemoveView (View* view)
    {
      render_target_registry.ReleaseResources (view);
    }

///��������� �������� ������ ����� (��������������!!!!)
    IFrameBuffer& GetCurrentFrameBuffer ()
    {
      return render_target_registry.GetFrameBuffer (GetRenderTargetView (), GetDepthStencilView ());
    }

///��������� ������� ������� ������� ��������� � �������� OpenGL
    void BindRenderTargets ()
    {
      if (need_update_render_targets)
        UpdateRenderTargets ();

      IFrameBuffer& frame_buffer = GetCurrentFrameBuffer ();

      updatable_frame_buffer = &frame_buffer;

      frame_buffer.Bind ();

      MakeContextCurrent ();
    }

  private:
    RenderTargetRegistry render_target_registry;     //������ ������� �����
    bool                 need_update_render_targets; //����, ����������� �� ������������� ���������� ������� ������� ���������
    FrameBufferPtr       updatable_frame_buffer;     //����������� ������ �����
    ViewPtr              default_render_target_view; //����������� ������ ����� �� ���������
    ViewPtr              default_depth_stencil_view; //����������� ������ �������-�������� �� ���������
    size_t               clear_views_mask;           //����� �������, ������� ����� ������� (���������� ��� ������ � Direct3D wrapper)
};

/*
    ����������� / ����������
*/

RenderTargetManager::RenderTargetManager (const ContextManager& context_manager, ISwapChain* swap_chain)
  : impl (new Impl (context_manager, swap_chain))
{
}

RenderTargetManager::~RenderTargetManager ()
{
}

/*
    �������� ������� ��������� ����������
*/

IStageState* RenderTargetManager::CreateStageState ()
{
  return new RenderTargetManagerState (static_cast<RenderTargetManagerState*> (&*impl));
}

/*
    �������� �������
*/

ITexture* RenderTargetManager::CreateTexture (const TextureDesc& desc)
{
  try
  {
    return impl->GetRenderTargetRegistry ().CreateRenderBuffer (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::RenderTargetManager::CreateTexture");

    throw;
  }
}

ITexture* RenderTargetManager::CreateRenderTargetTexture (ISwapChain* swap_chain, size_t buffer_index)
{
  try
  {
    return impl->GetRenderTargetRegistry ().CreateColorBuffer (swap_chain, buffer_index);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::RenderTargetManager::CreateRenderTargetTexture");
    
    throw;
  }
}

ITexture* RenderTargetManager::CreateDepthStencilTexture (ISwapChain* swap_chain)
{
  try
  {
    return impl->GetRenderTargetRegistry ().CreateDepthStencilBuffer (swap_chain);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::RenderTargetManager::CreateDepthStencilTexture");
    
    throw;
  }
}

/*
    �������� �����������
*/

IView* RenderTargetManager::CreateView (ITexture* texture, const ViewDesc& desc)
{
  try
  {
    return impl->CreateView (texture, desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::RenderTargetManager::CreateView");

    throw;
  }
}

/*
    ����� ������� �����������
*/

void RenderTargetManager::SetRenderTargets (IView* render_target_view, IView* depth_stencil_view)
{
  try
  {
    return impl->SetRenderTargets (render_target_view, depth_stencil_view);          
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::RenderTargetManager::SetRenderTargets");

    throw;
  }
}

IView* RenderTargetManager::GetRenderTargetView () const
{
  return impl->GetRenderTargetView ();
}

IView* RenderTargetManager::GetDepthStencilView () const
{
  return impl->GetDepthStencilView ();
}

/*
    ��������� �������� ������ � ���������
*/

void RenderTargetManager::SetViewport (const Viewport& viewport)
{
  impl->SetViewport (viewport);
}

void RenderTargetManager::SetScissor (const Rect& scissor_rect)
{
  impl->SetScissor (scissor_rect);
}

const Viewport& RenderTargetManager::GetViewport () const
{
  return impl->GetViewport ();
}

const Rect& RenderTargetManager::GetScissor () const
{
  return impl->GetScissor ();
}


/*
    ������� ������� ���������
*/

void RenderTargetManager::ClearRenderTargetView (const Color4f& color)
{  
  impl->ClearViews (ClearFlag_RenderTarget, color, 0.0f, 0);
}

void RenderTargetManager::ClearDepthStencilView (size_t clear_flags, float depth, unsigned char stencil)
{
  Color4f clear_color;

  impl->ClearViews (ClearFlag_Depth | ClearFlag_Stencil, clear_color, depth, stencil);
}

void RenderTargetManager::ClearViews (size_t clear_flags, const Color4f& color, float depth, unsigned char stencil)
{
  impl->ClearViews (clear_flags, color, depth, stencil);
}

/*
    ���������� �� ��������� ������� ������� ��������� / ���������� ������� ������� ���������
*/

void RenderTargetManager::UpdateRenderTargets ()
{
  try
  {
    impl->UpdateRenderTargets ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::RenderTargetManager::UpdateRenderTargets");
    throw;
  }
}

/*
    ��������� ��������� ������ � �������� OpenGL
*/

void RenderTargetManager::Bind ()
{
  impl->Bind ();
}
