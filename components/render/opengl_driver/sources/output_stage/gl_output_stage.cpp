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
  View*         render_target_view; //������� ����������� ������ �����
  View*         depth_stencil_view; //������� ����������� ������ �������-��������
  IFrameBuffer* frame_buffer;       //����� �����

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
    �������� ��������� ��������� ������ ��������� OpenGL
*/

class OutputStageState: public IStageState
{
  public:  
      //������������
    OutputStageState (OutputStageState* in_main_state)
    {
      Init (in_main_state, 0);
    }

    OutputStageState (ContextObject* in_owner)
    {
      Init (0, in_owner);
    }

      //��������� �������� ��������� ��������� ���������� ������
    void SetBlendState (BlendState* state)
    {
      if (state == blend_state)
        return;

      blend_state = state;
      
      UpdateNotify ();
    }

      //��������� �������� ��������� ��������� ���������� ������
    BlendState* GetBlendState () const { return blend_state.get (); }
    
      //��������� �������� ��������� ��������� ������������� ���������
    void SetDepthStencilState (DepthStencilState* state)
    {
      if (state == depth_stencil_state)
        return;

      depth_stencil_state = state;

      UpdateNotify ();      
    }

      //��������� �������� ��������� ��������� ������������� ���������
    DepthStencilState* GetDepthStencilState () const { return depth_stencil_state.get (); }
    
      //��������� �������� ������ ���������
    void SetStencilReference (size_t reference)
    {
      stencil_reference = reference;

      UpdateNotify ();
    }

      //��������� �������� ������ ���������
    size_t GetStencilReference () const { return stencil_reference; }

      //��������� ������� ����������� ���������
    void SetRenderTargetView (View* in_render_target_view)
    {
      if (render_target_view == in_render_target_view)
        return;

      render_target_view = in_render_target_view;
      
      UpdateNotify ();
    }
    
    void SetDepthStencilView (View* in_depth_stencil_view)
    {
      if (depth_stencil_view == in_depth_stencil_view)
        return;

      depth_stencil_view = in_depth_stencil_view;

      UpdateNotify ();
    }    

      //��������� �������� ����������� ������ �����
    View* GetRenderTargetView () const { return render_target_view.get (); }

      //��������� �������� ����������� ������ ������������� ���������
    View* GetDepthStencilView () const { return depth_stencil_view.get (); }    

      //��������� ��������� �������������
    void SetRasterizerState (RasterizerState* state)
    {
      if (state == rasterizer_state)
        return;
        
      rasterizer_state = state;
      
      UpdateNotify ();
    }

      //��������� ��������� �������������
    RasterizerState* GetRasterizerState () const { return rasterizer_state.get (); }
    
      //��������� ������� ������� ������
    void SetViewport (const Viewport& in_viewport)
    {
      viewport                  = in_viewport;
      need_recalc_viewport_hash = true;

      UpdateNotify ();
    }
    
      //��������� ������� ������� ������
    const Viewport& GetViewport () const { return viewport; }
    
      //��������� ������� ������� ���������
    void SetScissor (const Rect& in_scissor)
    {
      scissor                  = in_scissor;
      need_recalc_scissor_hash = true;

      UpdateNotify ();
    }

      //��������� ������� ������� ���������
    const Rect& GetScissor () const { return scissor; }

      //��������� ���� ������� ������
    size_t GetViewportHash () const
    {
      if (need_recalc_viewport_hash)
      {
        viewport_hash             = crc32 (&viewport, sizeof viewport);
        need_recalc_viewport_hash = false;
      }

      return viewport_hash;
    }

      //��������� ���� ������� ���������
    size_t GetScissorHash () const
    {
      if (need_recalc_scissor_hash)
      {
        scissor_hash             = crc32 (&scissor, sizeof scissor);
        need_recalc_scissor_hash = false;
      }

      return scissor_hash;
    }    

      //������ ���������
    void Capture (const StateBlockMask& mask)
    {
      if (main_state)
        Copy (*main_state, mask);
    }
    
      //�������������� ���������
    void Apply (const StateBlockMask& mask)
    {
      if (main_state)
        main_state->Copy (*this, mask);
    }

  private:    
      //����������� ���������
    void Copy (OutputStageState& source, const StateBlockMask& mask)
    {
      if (mask.os_blend_state)
        SetBlendState (source.GetBlendState ());

      if (mask.os_depth_stencil_state)
        SetDepthStencilState (source.GetDepthStencilState ());
        
      if (mask.os_render_target_view)
        SetRenderTargetView (source.GetRenderTargetView ());

      if (mask.os_depth_stencil_view)
        SetDepthStencilView (source.GetDepthStencilView ());

      if (mask.rs_state)
        SetRasterizerState (source.GetRasterizerState ());

      if (mask.rs_viewport)
        SetViewport (source.GetViewport ());

      if (mask.rs_scissor)
        SetScissor (source.GetScissor ());        
    }
    
      //���������� �� ���������� ��������� ������
    void UpdateNotify ()
    {
      if (!owner)
        return;

      owner->GetContextManager ().StageRebindNotify (Stage_Output);
    }

  private:
    OutputStageState (const OutputStageState&); //no impl
    
    void Init (OutputStageState* in_main_state, ContextObject* in_owner)
    {
      owner                     = in_owner;
      main_state                = in_main_state;
      stencil_reference         = 0;
      need_recalc_viewport_hash = true;
      need_recalc_scissor_hash  = true;

      memset (&viewport, 0, sizeof viewport);
      memset (&scissor, 0, sizeof scissor);
    }

  private:    
    typedef xtl::trackable_ptr<View>               ViewPtr;
    typedef xtl::trackable_ptr<BlendState>         BlendStatePtr;
    typedef xtl::trackable_ptr<DepthStencilState>  DepthStencilStatePtr;
    typedef xtl::trackable_ptr<OutputStageState>   OutputStageStatePtr;
    typedef xtl::trackable_ptr<RasterizerState>    RasterizerStatePtr;

  private:
    ContextObject*       owner;                     //�������� ��������� ������
    OutputStageStatePtr  main_state;                //�������� ��������� ������
    BlendStatePtr        blend_state;               //������� ��������� ��������� ���������� ������
    DepthStencilStatePtr depth_stencil_state;       //������� ��������� ��������� ������������� ���������
    size_t               stencil_reference;         //������� �������� ���������
    ViewPtr              render_target_view;        //������� ����������� ������ �����
    ViewPtr              depth_stencil_view;        //������� ����������� ������ ������������� ���������
    RasterizerStatePtr   rasterizer_state;          //��������� ������ ������������
    Viewport             viewport;                  //������� ������
    Rect                 scissor;                   //������� ���������
    mutable size_t       viewport_hash;             //��� ������� ������
    mutable size_t       scissor_hash;              //��� ������� ���������
    mutable bool         need_recalc_viewport_hash; //���� ������������� ��������� ���� ������� ������
    mutable bool         need_recalc_scissor_hash;  //���� ������������� ��������� ���� ������� ���������
};

}

/*
===================================================================================================
    OutputStage
===================================================================================================
*/

/*
    �������� ���������� ��������� ������ ��������� OpenGL
*/

struct OutputStage::Impl: public ContextObject, public FrameBufferManagerHolder, public OutputStageState
{
  public:
    Impl (ContextManager& context_manager, ISwapChain* swap_chain) :
      ContextObject (context_manager),
      FrameBufferManagerHolder (context_manager, swap_chain),
      OutputStageState (static_cast<ContextObject*> (this)),
      need_update_render_targets (false)
    {
        //����������� ��������� ������� ����� �� ���������
        
      register_swap_chain_manager (frame_buffer_manager);

        //������������� �����������
        
      xtl::com_ptr<ITexture> color_texture (frame_buffer_manager.CreateColorBuffer (swap_chain, 1), false),
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

        //������������� BlendState
          
      BlendDesc blend_desc;
      
      memset (&blend_desc, 0, sizeof (blend_desc));

      blend_desc.blend_enable     = false;
      blend_desc.color_write_mask = ColorWriteFlag_All;

      default_blend_state = BlendStatePtr (new BlendState (GetContextManager (), blend_desc), false);
      
      blend_desc.color_write_mask = 0;
      
      null_blend_state = BlendStatePtr (new BlendState (GetContextManager (), blend_desc), false);
      
        //������������� DepthStencilState
        
      DepthStencilDesc depth_stencil_desc;
      
      memset (&depth_stencil_desc, 0, sizeof (depth_stencil_desc));
      
      depth_stencil_desc.depth_test_enable  = true;
      depth_stencil_desc.depth_write_enable = true;
      depth_stencil_desc.depth_compare_mode = CompareMode_Less;
      
      default_depth_stencil_state = DepthStencilStatePtr (new DepthStencilState (GetContextManager (), depth_stencil_desc), false);
      
      depth_stencil_desc.depth_test_enable  = false;
      depth_stencil_desc.depth_write_enable = false;
      depth_stencil_desc.depth_compare_mode = CompareMode_AlwaysPass;
      
      null_depth_stencil_state = DepthStencilStatePtr (new DepthStencilState (GetContextManager (), depth_stencil_desc), false);
      
        //������������� ��������� ������������� �� ���������

      RasterizerDesc rasterizer_desc;

      memset (&rasterizer_desc, 0, sizeof rasterizer_desc);

      rasterizer_desc.fill_mode               = FillMode_Solid;
      rasterizer_desc.cull_mode               = CullMode_None;
      rasterizer_desc.front_counter_clockwise = true;
      rasterizer_desc.depth_bias              = 0;
      rasterizer_desc.scissor_enable          = false;
      rasterizer_desc.multisample_enable      = false;
      rasterizer_desc.antialiased_line_enable = false;

      default_rasterizer_state = RasterizerStatePtr (new RasterizerState (GetContextManager (), rasterizer_desc), false);
      
        //������������� �������� ��������� � ������
        
      SwapChainDesc swap_chain_desc;

      swap_chain->GetDesc (swap_chain_desc);

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

        //��������� ��������� �� ���������

      SetBlendState        (&*default_blend_state);
      SetDepthStencilState (&*default_depth_stencil_state);
      SetRasterizerState   (&*default_rasterizer_state);
      SetViewport          (default_viewport);
      SetScissor           (default_scissor);
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
      static const char* METHOD_NAME = "render::low_level::opengl::OutputStage::Impl::SetRenderTargets";
      
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
          
          if (context_cache [OutputStageCache_ClearColorHash] != clear_color_hash)
          {
            glClearColor (color.red, color.green, color.blue, color.alpha);
            
            context_cache [OutputStageCache_ClearColorHash] = clear_color_hash;
          }
          
          if (context_cache [OutputStageCache_ColorWriteMask] != ColorWriteFlag_All)
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
          
          if (context_cache [OutputStageCache_ClearDepthHash] != clear_depth_hash)
          {
            glClearDepth (depth);
            
            context_cache [OutputStageCache_ClearDepthHash] = clear_depth_hash;
          }
          
          if (!context_cache [OutputStageCache_DepthWriteEnable])
          {
            glDepthMask (GL_TRUE);
            
            need_restore_depth_write_mask = true;
          }          

          mask |= GL_DEPTH_BUFFER_BIT;        
        }
        
          //��������� ���������� ������� ������ ���������

        if (clear_flags & ClearFlag_Stencil)
        {
          if (context_cache [OutputStageCache_ClearStencilValue] != stencil)
          {
            glClearStencil (stencil);
            
            context_cache [OutputStageCache_ClearStencilValue] = stencil;
          }

          if (context_cache [OutputStageCache_StencilWriteMask] != unsigned char (~0))
          {
            glStencilMask (~0);
            
            need_restore_stencil_write_mask = true;
          }                    

          mask |= GL_STENCIL_BUFFER_BIT;
        }
        
          //��������� ������� ���������
          
        if (clear_flags & ClearFlag_ViewportOnly)
        {          
          if (!context_cache [OutputStageCache_ScissorEnable])
          {
            glEnable (GL_SCISSOR_TEST);

            need_restore_scissor_test = true;
          }

          if (context_cache [OutputStageCache_ScissorHash] != GetViewportHash ())
          {
            const Viewport& viewport = GetViewport ();

            glScissor (viewport.x, viewport.y, viewport.width, viewport.height);          

            context_cache [OutputStageCache_ScissorHash] = GetViewportHash ();

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
          size_t mask = context_cache [OutputStageCache_ColorWriteMask];

          glColorMask ((mask & ColorWriteFlag_Red) != 0, (mask & ColorWriteFlag_Green) != 0,
                      (mask & ColorWriteFlag_Blue) != 0, (mask & ColorWriteFlag_Alpha) != 0);
        }
        
        if (need_restore_depth_write_mask)
        {
          glDepthMask (GL_FALSE);
        }
        
        if (need_restore_stencil_write_mask)
        {
          glStencilMask (context_cache [OutputStageCache_StencilWriteMask]);
        }
        
        if (need_restore_scissor_test)
        {
          glDisable (GL_SCISSOR_TEST);
        }

        CheckErrors ("glClear");
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::OutputStage::ClearViews");

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
        
          //��������� ��������� ��������� ���������� ������        

        BlendState* blend_state = GetBlendState ();        

        if (blend_state && frame_buffer_manager.IsActiveColorBuffer ())
        {
          blend_state->Bind ();      
        }
        else
        {
          null_blend_state->Bind ();
        }
        
          //��������� ��������� ��������� ������������� ���������

        DepthStencilState* depth_stencil_state = GetDepthStencilState ();        

        if (depth_stencil_state && frame_buffer_manager.IsActiveDepthStencilBuffer ())
        {
          depth_stencil_state->Bind (GetStencilReference ());
        }
        else
        {
          null_depth_stencil_state->Bind (GetStencilReference ());
        }

          //��������� ��������� �������������

        RasterizerState* rasterizer_state = GetRasterizerState ();
      
        if (!rasterizer_state)
          rasterizer_state = default_rasterizer_state.get ();

        rasterizer_state->Bind ();

          //��������� �������� ������ � ���������
          
        size_t *context_cache = &GetContextDataTable (Stage_Output)[0],
               &current_viewport_hash = context_cache [OutputStageCache_ViewportHash],
               &current_scissor_hash  = context_cache [OutputStageCache_ScissorHash];
        
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
        exception.touch ("render::low_level::opengl::OutputStage::Bind");
        throw;
      }
    }
    
  private:
    typedef xtl::intrusive_ptr<FrameBufferHolder> FrameBufferHolderPtr;
    typedef xtl::trackable_ptr<FrameBufferHolder> FrameBufferHolderTrackablePtr;
    typedef xtl::com_ptr<View>                    ViewPtr;
    typedef stl::list<FrameBufferHolderPtr>       FrameBufferHolderList;
    typedef xtl::com_ptr<ISwapChain>              SwapChainPtr;
    typedef xtl::com_ptr<BlendState>              BlendStatePtr;
    typedef xtl::com_ptr<DepthStencilState>       DepthStencilStatePtr;
    typedef xtl::com_ptr<RasterizerState>         RasterizerStatePtr;

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
        exception.touch ("render::low_level::opengl::OutputStage::Impl::CreateFrameBufferHolder");
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
    BlendStatePtr                 default_blend_state;           //��������� ��������� ���������� ������ �� ���������
    BlendStatePtr                 null_blend_state;              //��������� ��������� ���������� ������ �����. ���������� ���������
    DepthStencilStatePtr          default_depth_stencil_state;   //��������� ��������� ������������� ��������� �� ���������
    DepthStencilStatePtr          null_depth_stencil_state;      //��������� ��������� ������������� ��������� �����. ���������� ���������
    RasterizerStatePtr            default_rasterizer_state;      //��������� ������������� �� ���������
};

/*
    ����������� / ����������
*/

OutputStage::OutputStage (ContextManager& context_manager, ISwapChain* swap_chain)
  : impl (new Impl (context_manager, swap_chain))
{
}

OutputStage::~OutputStage ()
{
}

/*
    �������� ������� ��������� ����������
*/

IStageState* OutputStage::CreateStageState ()
{
  return new OutputStageState (static_cast<OutputStageState*> (&*impl));
}

/*
    �������� �������
*/

ITexture* OutputStage::CreateTexture (const TextureDesc& desc)
{
  try
  {
    return impl->frame_buffer_manager.CreateRenderBuffer (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::OutputStage::CreateTexture");

    throw;
  }
}

ITexture* OutputStage::CreateRenderTargetTexture (ISwapChain* swap_chain, size_t buffer_index)
{
  try
  {
    return impl->frame_buffer_manager.CreateColorBuffer (swap_chain, buffer_index);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::OutputStage::CreateRenderTargetTexture");
    
    throw;
  }
}

ITexture* OutputStage::CreateDepthStencilTexture (ISwapChain* swap_chain)
{
  try
  {
    return impl->frame_buffer_manager.CreateDepthStencilBuffer (swap_chain);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::OutputStage::CreateDepthStencilTexture");
    
    throw;
  }
}

/*
    �������� �����������
*/

IView* OutputStage::CreateView (ITexture* texture, const ViewDesc& desc)
{
  try
  {
    return impl->CreateView (texture, desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::OutputStage::CreateView");

    throw;
  }
}

/*
    �������� ��������� ��������� ������
*/

IBlendState* OutputStage::CreateBlendState (const BlendDesc& desc)
{
  try
  {
    return new BlendState (impl->GetContextManager (), desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::OutputStage::CreateBlendState");

    throw;
  }
}

IDepthStencilState* OutputStage::CreateDepthStencilState (const DepthStencilDesc& desc)
{
  try
  {
    return new DepthStencilState (impl->GetContextManager (), desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::OutputStage::CreateDepthStencilState");
    
    throw;
  }
}

IRasterizerState* OutputStage::CreateRasterizerState (const RasterizerDesc& desc)
{
  try
  {
    return new RasterizerState (impl->GetContextManager (), desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::OutputStage::CreateRasterizerState");
    throw;
  }
}

/*
    ����� ������� �����������
*/

void OutputStage::SetRenderTargets (IView* render_target_view, IView* depth_stencil_view)
{
  try
  {
    return impl->SetRenderTargets (render_target_view, depth_stencil_view);          
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::OutputStage::SetRenderTargets");

    throw;
  }
}

IView* OutputStage::GetRenderTargetView () const
{
  return impl->GetRenderTargetView ();
}

IView* OutputStage::GetDepthStencilView () const
{
  return impl->GetDepthStencilView ();
}

/*
    ��������� ��������� ���������� ������
*/

void OutputStage::SetBlendState (IBlendState* state)
{
  BlendState* blend_state = cast_object<BlendState> (*impl, state, "render::low_level::opengl::OutputStage::SetBlendState", "state");
      
  impl->SetBlendState (blend_state);
}

IBlendState* OutputStage::GetBlendState () const
{
  return impl->GetBlendState ();
}

/*
    ��������� ��������� ������������� ���������
*/

void OutputStage::SetDepthStencilState (IDepthStencilState* state)
{
  DepthStencilState* depth_stencil_state = cast_object<DepthStencilState> (*impl, state, "render::low_level::opengl::OutputStage::SetDepthStencilState", "state");

  impl->SetDepthStencilState (depth_stencil_state);
}

void OutputStage::SetStencilReference (size_t reference)
{
  impl->SetStencilReference (reference);
}

IDepthStencilState* OutputStage::GetDepthStencilState () const
{
  return impl->GetDepthStencilState ();
}

size_t OutputStage::GetStencilReference () const
{
  return impl->GetStencilReference ();
}

/*
    ��������� ��������� ������������
*/

void OutputStage::SetRasterizerState (IRasterizerState* state)
{
  RasterizerState* rasterizer_state = cast_object<RasterizerState> (*impl, state, "render::low_level::opengl::OutputStage::SetState", "state");

  impl->SetRasterizerState (rasterizer_state);
}

void OutputStage::SetViewport (const Viewport& viewport)
{
  impl->SetViewport (viewport);
}

void OutputStage::SetScissor (const Rect& scissor_rect)
{
  impl->SetScissor (scissor_rect);
}

IRasterizerState* OutputStage::GetRasterizerState () const
{
  return impl->GetRasterizerState ();
}

const Viewport& OutputStage::GetViewport () const
{
  return impl->GetViewport ();
}

const Rect& OutputStage::GetScissor () const
{
  return impl->GetScissor ();
}


/*
    ������� ������� ���������
*/

void OutputStage::ClearRenderTargetView (const Color4f& color)
{  
  impl->ClearViews (ClearFlag_RenderTarget, color, 0.0f, 0);
}

void OutputStage::ClearDepthStencilView (size_t clear_flags, float depth, unsigned char stencil)
{
  Color4f clear_color;

  impl->ClearViews (ClearFlag_Depth | ClearFlag_Stencil, clear_color, depth, stencil);
}

void OutputStage::ClearViews (size_t clear_flags, const Color4f& color, float depth, unsigned char stencil)
{
  impl->ClearViews (clear_flags, color, depth, stencil);
}

/*
    ���������� �� ��������� ������� ������� ��������� / ���������� ������� ������� ���������
*/

void OutputStage::UpdateRenderTargets ()
{
  try
  {
    impl->UpdateRenderTargets ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::OutputStage::UpdateRenderTargets");
    throw;
  }
}

/*
    ��������� ��������� ������ � �������� OpenGL
*/

void OutputStage::Bind ()
{
  impl->Bind ();
}
