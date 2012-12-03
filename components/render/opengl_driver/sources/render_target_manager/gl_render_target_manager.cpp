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
      : owner ()
      , main_state (in_main_state)
    {
    }

    RenderTargetManagerState (ContextObject* in_owner)
      : owner (in_owner)
      , main_state ()
    {
    }

///��������� �������� ����������� ������ �����
    void SetRenderTargetView (size_t view_index, View* view)
    {
      RenderTargetDesc& render_target = GetRenderTarget (view_index, "render::low_level::opengl::RenderTargetManagerState::SetRenderTargetView");
    
      if (render_target.view == view)
        return;

      render_target.view = view;
      
      UpdateNotify ();
    }
    
///��������� �������� ����������� ������ �����
    View* GetRenderTargetView (size_t view_index)
    {
      RenderTargetDesc& render_target = GetRenderTarget (view_index, "render::low_level::opengl::RenderTargetManagerState::GetRenderTargetView");

      return render_target.view.get ();
    }

    void GetRenderTargetViews (View* views [DEVICE_RENDER_TARGET_SLOTS_COUNT])
    {
      for (size_t i=0; i<DEVICE_RENDER_TARGET_SLOTS_COUNT; i++)
        views [i] = render_targets [i].view.get ();
    }

///��������� ��������� ������� �����
    void HasRenderTargetViews (bool state [DEVICE_RENDER_TARGET_SLOTS_COUNT])
    {
      for (size_t i=0; i<DEVICE_RENDER_TARGET_SLOTS_COUNT; i++)
        state [i] = render_targets [i].view.get () != 0;
    }
    
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
    void SetViewport (size_t view_index, const Viewport& in_viewport)
    {
      RenderTargetDesc& render_target = GetRenderTarget (view_index, "render::low_level::opengl::RenderTargetManagerState::SetViewport");

      render_target.viewport                  = in_viewport;
      render_target.need_recalc_viewport_hash = true;

      UpdateNotify ();
    }
    
///��������� ������� ������� ������
    const Viewport& GetViewport (size_t view_index)
    {
      RenderTargetDesc& render_target = GetRenderTarget (view_index, "render::low_level::opengl::RenderTargetManagerState::GetViewport");

      return render_target.viewport;
    }
    
///��������� ������� ������� ���������
    void SetScissor (size_t view_index, const Rect& in_scissor)
    {
      RenderTargetDesc& render_target = GetRenderTarget (view_index, "render::low_level::opengl::RenderTargetManagerState::SetScissor");

      render_target.scissor                  = in_scissor;
      render_target.need_recalc_scissor_hash = true;

      UpdateNotify ();
    }

///��������� ������� ������� ���������
    const Rect& GetScissor (size_t view_index)
    {
      RenderTargetDesc& render_target = GetRenderTarget (view_index, "render::low_level::opengl::RenderTargetManagerState::GetScissor");

      return render_target.scissor;
    }

///��������� ���� ������� ������
    size_t GetViewportHash (size_t view_index)
    {
      RenderTargetDesc& render_target = GetRenderTarget (view_index, "render::low_level::opengl::RenderTargetManagerState::GetViewportHash");

      if (render_target.need_recalc_viewport_hash)
      {
        render_target.viewport_hash             = crc32 (&render_target.viewport, sizeof render_target.viewport);
        render_target.viewport_rect_hash        = crc32 (&render_target.viewport, sizeof Rect);
        render_target.need_recalc_viewport_hash = false;
      }

      return render_target.viewport_hash;
    }

    size_t GetViewportRectHash (size_t view_index)
    {
      GetViewportHash (view_index); //for update

      return render_targets [view_index].viewport_rect_hash;
    }

///��������� ���� ������� ���������
    size_t GetScissorHash (size_t view_index)
    {
      RenderTargetDesc& render_target = GetRenderTarget (view_index, "render::low_level::opengl::RenderTargetManagerState::GetScissorHash");

      if (render_target.need_recalc_scissor_hash)
      {
        render_target.scissor_hash             = crc32 (&render_target.scissor, sizeof render_target.scissor);
        render_target.need_recalc_scissor_hash = false;
      }

      return render_target.scissor_hash;
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
      if (mask.os_render_target_views || mask.rs_viewports || mask.rs_scissors)
      {
        for (size_t i=0; i<DEVICE_RENDER_TARGET_SLOTS_COUNT; i++)
        {
          RenderTargetDesc& render_target = render_targets [i];

          if (mask.os_render_target_views) SetRenderTargetView (i, source.GetRenderTargetView (i));
          if (mask.rs_viewports)           SetViewport (i, source.GetViewport (i));
          if (mask.rs_scissors)            SetScissor (i, source.GetScissor (i));        
        }
      }

      if (mask.os_depth_stencil_view)
        SetDepthStencilView (source.GetDepthStencilView ());
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

///��������� ������ �����
    struct RenderTargetDesc;

    RenderTargetDesc& GetRenderTarget (size_t view_index, const char* source)
    {
      if (view_index >= DEVICE_RENDER_TARGET_SLOTS_COUNT)
        throw xtl::make_range_exception (source, "render_target_slot", view_index, DEVICE_RENDER_TARGET_SLOTS_COUNT);

      return render_targets [view_index];
    }

  private:    
    typedef xtl::trackable_ptr<View>                     ViewPtr;
    typedef xtl::trackable_ptr<RenderTargetManagerState> RenderTargetManagerStatePtr;

    struct RenderTargetDesc
    {
      ViewPtr  view;                      //������� ����������� ������� �����
      Viewport viewport;                  //������� ������
      Rect     scissor;                   //������� ���������
      size_t   viewport_hash;             //��� ������� ������
      size_t   viewport_rect_hash;        //��� ������� ������ ��� ������� ���������
      size_t   scissor_hash;              //��� ������� ���������
      bool     need_recalc_viewport_hash; //���� ������������� ��������� ���� ������� ������
      bool     need_recalc_scissor_hash;  //���� ������������� ��������� ���� ������� ���������

      RenderTargetDesc () 
        : viewport_hash ()
        , viewport_rect_hash ()
        , scissor_hash ()
        , need_recalc_viewport_hash (true)
        , need_recalc_scissor_hash (true)
      {
        memset (&viewport, 0, sizeof viewport);
        memset (&scissor, 0, sizeof scissor);
      }
    };

  private:
    ContextObject*              owner;                                             //�������� ��������� ������
    RenderTargetManagerStatePtr main_state;                                        //�������� ��������� ������
    RenderTargetDesc            render_targets [DEVICE_RENDER_TARGET_SLOTS_COUNT]; //����������� ����� ����������
    ViewPtr                     depth_stencil_view;                                //������� ����������� ������ ������������� ���������
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

      SetRenderTargetView (0, default_render_target_view.get ());
      SetDepthStencilView (default_depth_stencil_view.get ());

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
      
      for (size_t i=0; i<DEVICE_RENDER_TARGET_SLOTS_COUNT; i++)
        SetViewport (i, default_viewport);      
      
      Rect default_scissor;      
      
      memset (&default_scissor, 0, sizeof default_scissor);      

      default_scissor.width  = swap_chain_desc.frame_buffer.width;
      default_scissor.height = swap_chain_desc.frame_buffer.height;        

      for (size_t i=0; i<DEVICE_RENDER_TARGET_SLOTS_COUNT; i++)
        SetScissor (i, default_scissor);
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
    void SetRenderTargetView (size_t render_target_slot, IView* in_render_target_view)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::RenderTargetManager::Impl::SetRenderTargetView";
      
        //���������� ����� �����������

      View* render_target_view = cast_object<View> (GetContextManager (), in_render_target_view, METHOD_NAME, "render_target_view");

        //���������� �������� ��������� ������

      RenderTargetManagerState::SetRenderTargetView (render_target_slot, render_target_view);

        //��������� ����� ������������� ���������� ������� ������� ���������

      need_update_render_targets = true;
    }

    void SetDepthStencilView (IView* in_depth_stencil_view)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::RenderTargetManager::Impl::SetDepthStencilView";
      
        //���������� ����� �����������

      View* depth_stencil_view = cast_object<View> (GetContextManager (), in_depth_stencil_view, METHOD_NAME, "depth_stencil_view");

        //���������� �������� ��������� ������

      RenderTargetManagerState::SetDepthStencilView (depth_stencil_view);

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
      //TODO: MRT support

      try
      {      
          //��������� ����� ����������
        
        BindRenderTargets ();

          //��������� �������� ������ � ���������

        UpdateViewportAndScissor (0);

          //�������� ������

        CheckErrors ("");
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::RenderTargetManager::Bind");
        throw;
      }
    }

///��������� �������� ������ � ���������
    void UpdateViewportAndScissor (size_t render_target_slot)
    {
      try
      {
        const size_t current_viewport_hash = GetContextCacheValue (CacheEntry_ViewportHash0 + render_target_slot),
                     current_scissor_hash  = GetContextCacheValue (CacheEntry_ScissorHash0 + render_target_slot),
                     viewport_hash         = GetViewportHash (render_target_slot),
                     scissor_hash          = GetScissorHash (render_target_slot);

        if (current_viewport_hash != viewport_hash)
        {
          const Viewport& viewport = GetViewport (render_target_slot);

          glViewport (viewport.x, viewport.y, viewport.width, viewport.height);

#ifndef OPENGL_ES_SUPPORT
          glDepthRange  (viewport.min_depth, viewport.max_depth);
#else
          glDepthRangef (viewport.min_depth, viewport.max_depth);
#endif
          
          SetContextCacheValue (CacheEntry_ViewportHash0 + render_target_slot, viewport_hash);
        }
        
        if (current_scissor_hash != scissor_hash)
        {        
          const Rect& scissor = GetScissor (render_target_slot);
          
          glScissor (scissor.x >= 0 ? scissor.x : 0, scissor.y >= 0 ? scissor.y : 0, scissor.width, scissor.height);

          SetContextCacheValue (CacheEntry_ScissorHash0 + render_target_slot, scissor_hash);
        }

          //���������� �� ��������� ������� ������� ���������

        GetCurrentFrameBuffer ().InvalidateRenderTargets (render_target_slot, GetViewport (render_target_slot));
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::low_level::opengl::RenderTargetManager::UpdateViewportAndScissor");
        throw;
      }
    }

///������� ������� ����� � �������-��������
    void ClearViews (size_t clear_flags, size_t views_count, const size_t* render_target_indices, const Color4f* colors, float depth, unsigned char stencil)
    {
      //TODO: MRT support

      try
      {
          //�������� ������������ ����������

        if (views_count && !render_target_indices)
          throw xtl::make_null_argument_exception ("", "render_target_indices");

        if (views_count && !colors)
          throw xtl::make_null_argument_exception ("", "colors");

        if (views_count > 1 || views_count && render_target_indices [0] != 0)
          throw xtl::format_not_supported_exception ("", "MRT not supported");

          //���������� �������, ������� ����� �������

        clear_flags &= clear_views_mask;

          //��������� ����� ���������� � ��������� ���������

        BindRenderTargets ();

          //��������� �������� ��� ����������

        GLbitfield mask = 0;
        
        bool need_restore_color_write_mask   = false,
             need_restore_depth_write_mask   = false,
             need_restore_stencil_write_mask = false,
             need_restore_scissor_test       = false;
             
          //��������� ���������� ������� ������ �����
        
        if ((clear_flags & ClearFlag_RenderTarget) && views_count)
        {
          const Color4f& color = colors [0];

          size_t clear_color_hash = crc32 (&color, sizeof color);
          
          if (GetContextCacheValue (CacheEntry_ClearColorHash0) != clear_color_hash)
          {
            glClearColor (color.red, color.green, color.blue, color.alpha);
            
            SetContextCacheValue (CacheEntry_ClearColorHash0, clear_color_hash);
          }

          if (GetContextCacheValue (CacheEntry_ColorWriteMask0) != ColorWriteFlag_All)
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
          
          if (GetContextCacheValue (CacheEntry_ClearDepthHash) != clear_depth_hash)
          {
#ifndef OPENGL_ES_SUPPORT
            glClearDepth (depth);
#else
            glClearDepthf (depth);
#endif

            SetContextCacheValue (CacheEntry_ClearDepthHash, clear_depth_hash);
          }
          
          if (!GetContextCacheValue (CacheEntry_DepthWriteEnable))
          {
            glDepthMask (GL_TRUE);

            need_restore_depth_write_mask = true;
          }          

          mask |= GL_DEPTH_BUFFER_BIT;        
        }
        
          //��������� ���������� ������� ������ ���������

        if (clear_flags & ClearFlag_Stencil)
        {
          if (GetContextCacheValue (CacheEntry_ClearStencilValue) != stencil)
          {
            glClearStencil (stencil);

            SetContextCacheValue (CacheEntry_ClearStencilValue, stencil);
          }

          if (GetContextCacheValue (CacheEntry_StencilWriteMask) != (unsigned char)(~0))
          {
            glStencilMask (~0);

            need_restore_stencil_write_mask = true;
          }

          mask |= GL_STENCIL_BUFFER_BIT;
        }
        
          //��������� ������� ���������

        size_t current_scissor_state = GetContextCacheValue (CacheEntry_ScissorEnable0);

        if (clear_flags & ClearFlag_ViewportOnly)
        {          
          if (!current_scissor_state)
          {
            glEnable (GL_SCISSOR_TEST);

            need_restore_scissor_test = true;
          }

          size_t viewport_rect_hash = GetViewportRectHash (0);

          if (GetContextCacheValue (CacheEntry_ScissorHash0) != viewport_rect_hash)
          {
            const Viewport& viewport = GetViewport (0);

            glScissor (viewport.x >= 0 ? viewport.x : 0, viewport.y >= 0 ? viewport.y : 0, viewport.width, viewport.height);          

            SetContextCacheValue (CacheEntry_ScissorHash0, viewport_rect_hash);

            StageRebindNotify (Stage_RenderTargetManager);
          }
        }
        else
        {
          if (current_scissor_state)
          {
            glDisable (GL_SCISSOR_TEST);

            need_restore_scissor_test = true;
          }

            //���������� �� ���������� ������� ����������          

          GetCurrentFrameBuffer ().InvalidateRenderTargets (0);
        }

          //������� ��������� �������
          
        glClear (mask);
        
          //�������������� ��������� OpenGL
        
        if (need_restore_color_write_mask)
        {
          size_t mask = GetContextCacheValue (CacheEntry_ColorWriteMask0);

          glColorMask ((mask & ColorWriteFlag_Red) != 0, (mask & ColorWriteFlag_Green) != 0,
                      (mask & ColorWriteFlag_Blue) != 0, (mask & ColorWriteFlag_Alpha) != 0);
        }
        
        if (need_restore_depth_write_mask)
        {
          glDepthMask (GL_FALSE);
        }

        if (need_restore_stencil_write_mask)
        {
          glStencilMask (GetContextCacheValue (CacheEntry_StencilWriteMask));
        }

        if (need_restore_scissor_test)
        {
          if (current_scissor_state) glEnable (GL_SCISSOR_TEST);
          else                       glDisable (GL_SCISSOR_TEST);
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
      view->RegisterDestroyHandler (xtl::bind (&Impl::RemoveView, this, view.get ()), GetTrackable ());
    }

///�������� �����������
    void RemoveView (View* view)
    {
      render_target_registry.ReleaseResources (view);
    }

///��������� �������� ������ ����� (TODO: ��������������!!!!)
    IFrameBuffer& GetCurrentFrameBuffer ()
    {
      View* render_target_views [DEVICE_RENDER_TARGET_SLOTS_COUNT];

      GetRenderTargetViews (render_target_views);

      for (size_t i=1; i<DEVICE_RENDER_TARGET_SLOTS_COUNT; i++)
        if (render_target_views [i])
          throw xtl::format_not_supported_exception ("render::low_level::opengl::RenderTargetManager::GetCurrentFrameBuffer", "MRT is not yet supported");

      return render_target_registry.GetFrameBuffer (render_target_views [0], GetDepthStencilView ());
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

void RenderTargetManager::SetRenderTargetView (size_t render_target_slot, IView* render_target_view)
{
  try
  {
    return impl->SetRenderTargetView (render_target_slot, render_target_view);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::RenderTargetManager::SetRenderTargetView");
    throw;
  }
}

void RenderTargetManager::SetDepthStencilView (IView* depth_stencil_view)
{
  try
  {
    return impl->SetDepthStencilView (depth_stencil_view);          
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::RenderTargetManager::SetDepthStencilView");
    throw;
  }
}

IView* RenderTargetManager::GetRenderTargetView (size_t render_target_slot) const
{
  return impl->GetRenderTargetView (render_target_slot);
}

IView* RenderTargetManager::GetDepthStencilView () const
{
  return impl->GetDepthStencilView ();
}

void RenderTargetManager::HasRenderTargetViews (bool states [DEVICE_RENDER_TARGET_SLOTS_COUNT]) const
{
  impl->HasRenderTargetViews (states);
}

/*
    ��������� �������� ������ � ���������
*/

void RenderTargetManager::SetViewport (size_t render_target_slot, const Viewport& viewport)
{
  impl->SetViewport (render_target_slot, viewport);
}

void RenderTargetManager::SetScissor (size_t render_target_slot, const Rect& scissor_rect)
{
  impl->SetScissor (render_target_slot, scissor_rect);
}

const Viewport& RenderTargetManager::GetViewport (size_t render_target_slot) const
{
  return impl->GetViewport (render_target_slot);
}

const Rect& RenderTargetManager::GetScissor (size_t render_target_slot) const
{
  return impl->GetScissor (render_target_slot);
}


/*
    ������� ������� ���������
*/

void RenderTargetManager::ClearRenderTargetView (size_t render_target_slot, const Color4f& color)
{  
  impl->ClearViews (ClearFlag_RenderTarget, 1, &render_target_slot, &color, 0.0f, 0);
}

void RenderTargetManager::ClearDepthStencilView (size_t clear_flags, float depth, unsigned char stencil)
{
  size_t  view_index = 0;
  Color4f clear_color;

  impl->ClearViews (ClearFlag_Depth | ClearFlag_Stencil, 0, &view_index, &clear_color, depth, stencil);
}

void RenderTargetManager::ClearViews (size_t clear_flags, size_t views_count, const size_t* view_indices, const Color4f* colors, float depth, unsigned char stencil)
{
  impl->ClearViews (clear_flags, views_count, view_indices, colors, depth, stencil);
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
