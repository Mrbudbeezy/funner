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
      //�����������
    OutputStageState (OutputStageState* in_main_state=0) : main_state (in_main_state), stencil_reference (0) {}

      //��������� �������� ��������� ��������� ���������� ������
    void SetBlendState (BlendState* state)
    {
      if (state == blend_state)
        return;

      blend_state = state;
    }

      //��������� �������� ��������� ��������� ���������� ������
    BlendState* GetBlendState () const { return blend_state.get (); }
    
      //��������� �������� ��������� ��������� ������������� ���������
    void SetDepthStencilState (DepthStencilState* state)
    {
      if (state == depth_stencil_state)
        return;

      depth_stencil_state = state;            
    }

      //��������� �������� ��������� ��������� ������������� ���������
    DepthStencilState* GetDepthStencilState () const { return depth_stencil_state.get (); }
    
      //��������� �������� ������ ���������
    void SetStencilReference (size_t reference)
    {
      stencil_reference = reference;
    }

      //��������� �������� ������ ���������
    size_t GetStencilReference () const { return stencil_reference; }

      //��������� ������� ����������� ���������
    void SetRenderTargetView (View* in_render_target_view)
    {
      if (render_target_view == in_render_target_view)
        return;

      render_target_view = in_render_target_view;
    }
    
    void SetDepthStencilView (View* in_depth_stencil_view)
    {
      if (depth_stencil_view == in_depth_stencil_view)
        return;

      depth_stencil_view = in_depth_stencil_view;
    }    

      //��������� �������� ����������� ������ �����
    View* GetRenderTargetView () const { return render_target_view.get (); }

      //��������� �������� ����������� ������ ������������� ���������
    View* GetDepthStencilView () const { return depth_stencil_view.get (); }    
    
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
    }    

  private:
    OutputStageState (const OutputStageState&); //no impl        

  private:    
    typedef xtl::trackable_ptr<View>              ViewPtr;
    typedef xtl::trackable_ptr<BlendState>        BlendStatePtr;
    typedef xtl::trackable_ptr<DepthStencilState> DepthStencilStatePtr;
    typedef xtl::trackable_ptr<OutputStageState>  OutputStageStatePtr;

  private:
    OutputStageStatePtr  main_state;          //�������� ��������� ������
    BlendStatePtr        blend_state;         //������� ��������� ��������� ���������� ������
    DepthStencilStatePtr depth_stencil_state; //������� ��������� ��������� ������������� ���������
    size_t               stencil_reference;   //������� �������� ���������
    ViewPtr              render_target_view;  //������� ����������� ������ �����
    ViewPtr              depth_stencil_view;  //������� ����������� ������ ������������� ���������
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

struct OutputStage::Impl: public ContextObject, public FrameBufferManagerHolder
{
  public:
    Impl (ContextManager& context_manager, ISwapChain* swap_chain) :
      ContextObject (context_manager),
      FrameBufferManagerHolder (context_manager, swap_chain),
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
      
        //��������� ��������� �� ���������

      SetBlendState (&*default_blend_state);
      SetDepthStencilState (&*default_depth_stencil_state);
    }    
    
      //��������� ��������� ������
    OutputStageState& GetState () { return state; }
    
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

      state.SetRenderTargetView (render_target_view);
      state.SetDepthStencilView (depth_stencil_view);

        //��������� ����� ������������� ���������� ������� ������� ���������

      need_update_render_targets = true;
    }

      //��������� �������� ��������� ��������� ���������� ������
    void SetBlendState (IBlendState* in_blend_state)
    {
      BlendState* blend_state = cast_object<BlendState> (*this, in_blend_state, "render::low_level::opengl::OutputStage::SetBlendState", "blend_state");
      
      state.SetBlendState (blend_state);
    }
      
      //��������� �������� ��������� ��������� ���������� ������
    IBlendState* GetBlendState () { return state.GetBlendState (); }  
    
      //��������� �������� ��������� ��������� ������������� ���������
    void SetDepthStencilState (IDepthStencilState* in_depth_stencil_state)
    {
      DepthStencilState* depth_stencil_state = cast_object<DepthStencilState> (*this, in_depth_stencil_state,
        "render::low_level::opengl::OutputStage::SetDepthStencilState", "depth_stencil_state");

      state.SetDepthStencilState (depth_stencil_state);
    }
    
      //��������� �������� ��������� ��������� ������������� ���������
    IDepthStencilState* GetDepthStencilState () { return state.GetDepthStencilState (); }
     
      //���������� ������� ������� ����� � �������-��������
    void ClearViews (size_t clear_flags, const Color4f& color, float depth, unsigned char stencil)
    {
      try
      {
          //��������� ��������� ���������

        MakeContextCurrent ();      
        
          //��������� �������� ��� ����������
          
        ContextDataTable& context_cache = GetContextDataTable (Stage_Output);
        
        GLbitfield mask = 0;
        
        if (clear_flags & ClearFlag_RenderTarget)
        {
          size_t clear_color_hash = crc32 (&color, sizeof color);
          
          if (context_cache [OutputStageCache_ClearColorHash] != clear_color_hash)
          {
            glClearColor (color.red, color.green, color.blue, color.alpha);
            
            context_cache [OutputStageCache_ClearColorHash] = clear_color_hash;
          }
          
          mask |= GL_COLOR_BUFFER_BIT;
        }

        if (clear_flags & ClearFlag_Depth)
        {
          size_t clear_depth_hash = crc32 (&depth, sizeof depth);
          
          if (context_cache [OutputStageCache_ClearDepthHash] != clear_depth_hash)
          {
            glClearDepth (depth);
            
            context_cache [OutputStageCache_ClearDepthHash] = clear_depth_hash;
          }

          mask |= GL_DEPTH_BUFFER_BIT;        
        }

        if (clear_flags & ClearFlag_Stencil)
        {
          if (context_cache [OutputStageCache_ClearStencilValue] != stencil)
          {
            glClearStencil (stencil);
            
            context_cache [OutputStageCache_ClearStencilValue] = stencil;
          }

          mask |= GL_STENCIL_BUFFER_BIT;        
        }      

        glClear     (mask);
        CheckErrors ("glClear");
      }
      catch (common::Exception& exception)
      {
        exception.Touch ("render::low_level::opengl::OutputStage::ClearViews");

        throw;
      }
    }
    
      //���������� �� ��������� ������� ������� ���������
    void InvalidateRenderTargets (const Rect& update_rect)
    {
      GetCurrentFrameBuffer ().InvalidateRenderTargets (update_rect);
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
        BindRenderTargets ();

        BlendState*        blend_state         = state.GetBlendState ();
        DepthStencilState* depth_stencil_state = state.GetDepthStencilState ();

        if (blend_state && frame_buffer_manager.IsActiveColorBuffer ())
        {
          blend_state->Bind ();      
        }
        else
        {
          null_blend_state->Bind ();
        }

        if (depth_stencil_state && frame_buffer_manager.IsActiveDepthStencilBuffer ())
        {
          depth_stencil_state->Bind (state.GetStencilReference ());
        }
        else
        {
          null_depth_stencil_state->Bind (state.GetStencilReference ());
        }
      }
      catch (common::Exception& exception)
      {
        exception.Touch ("render::low_level::opengl::OutputStage::Bind");
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
            Raise<ArgumentException> ("", "Render-target view has wrong bind flags %s", get_name ((BindFlag)render_target_view->GetBindFlags ()));
            
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
              RaiseNotSupported ("", "Unsupported render-target view texture format=%s", get_name (desc.format));
              break;
            default:
              RaiseInvalidArgument ("", "texture_desc.format", desc.format);
              break;
          }
        }

        if (depth_stencil_view)
        {
          if (!(depth_stencil_view->GetBindFlags () & BindFlag_DepthStencil))
            Raise<ArgumentException> ("", "Depth-stencil view has wrong bind flags %s", get_name ((BindFlag)depth_stencil_view->GetBindFlags ()));
            
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
              RaiseNotSupported ("", "Unsupported depth-stencil view texture format=%s", get_name (desc.format));
              break;
            case PixelFormat_D16:
            case PixelFormat_D24X8:
            case PixelFormat_D24S8:
            case PixelFormat_S8:
              break;
            default:
              RaiseInvalidArgument ("", "texture_desc.format", desc.format);
              break;
          }
        }

        return FrameBufferHolderPtr (new FrameBufferHolder (render_target_view, depth_stencil_view, frame_buffer_manager), false);
      }
      catch (common::Exception& exception)
      {
        exception.Touch ("render::low_level::opengl::OutputStage::Impl::CreateFrameBufferHolder");
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
      return *GetFrameBufferHolder (state.GetRenderTargetView (), state.GetDepthStencilView ())->frame_buffer;
    }    

      //��������� ������� ������� ������� ��������� � �������� OpenGL
    void BindRenderTargets ()
    {
      if (need_update_render_targets)
        UpdateRenderTargets ();

      FrameBufferHolder* frame_buffer_holder = GetFrameBufferHolder (state.GetRenderTargetView (), state.GetDepthStencilView ());

      updatable_frame_buffer_holder = frame_buffer_holder;

      frame_buffer_holder->frame_buffer->Bind ();      
    }

  private:
    OutputStageState              state;                         //��������� ������
    FrameBufferHolderList         frame_buffers;                 //������ �����
    bool                          need_update_render_targets;    //����, ����������� �� ������������� ���������� ������� ������� ���������
    FrameBufferHolderTrackablePtr updatable_frame_buffer_holder; //����������� ������ �����
    ViewPtr                       default_render_target_view;    //����������� ������ ����� �� ���������
    ViewPtr                       default_depth_stencil_view;    //����������� ������ �������-�������� �� ���������
    BlendStatePtr                 default_blend_state;           //��������� ��������� ���������� ������ �� ���������
    BlendStatePtr                 null_blend_state;              //��������� ��������� ���������� ������ �����. ���������� ���������
    DepthStencilStatePtr          default_depth_stencil_state;   //��������� ��������� ������������� ��������� �� ���������
    DepthStencilStatePtr          null_depth_stencil_state;      //��������� ��������� ������������� ��������� �����. ���������� ���������
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
  return new OutputStageState (&impl->GetState ());
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
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::OutputStage::CreateTexture");

    throw;
  }
}

ITexture* OutputStage::CreateRenderTargetTexture (ISwapChain* swap_chain, size_t buffer_index)
{
  try
  {
    return impl->frame_buffer_manager.CreateColorBuffer (swap_chain, buffer_index);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::OutputStage::CreateRenderTargetTexture");
    
    throw;
  }
}

ITexture* OutputStage::CreateDepthStencilTexture (ISwapChain* swap_chain)
{
  try
  {
    return impl->frame_buffer_manager.CreateDepthStencilBuffer (swap_chain);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::OutputStage::CreateDepthStencilTexture");
    
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
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::OutputStage::CreateView");

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
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::OutputStage::CreateBlendState");

    throw;
  }
}

IDepthStencilState* OutputStage::CreateDepthStencilState (const DepthStencilDesc& desc)
{
  try
  {
    return new DepthStencilState (impl->GetContextManager (), desc);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::OutputStage::CreateDepthStencilState");
    
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
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::OutputStage::SetRenderTargets");

    throw;
  }
}

IView* OutputStage::GetRenderTargetView () const
{
  return impl->GetState ().GetRenderTargetView ();
}

IView* OutputStage::GetDepthStencilView () const
{
  return impl->GetState ().GetDepthStencilView ();
}

/*
    ��������� ��������� ���������� ������
*/

void OutputStage::SetBlendState (IBlendState* state)
{
  impl->SetBlendState (state);
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
  impl->SetDepthStencilState (state);
}

void OutputStage::SetStencilReference (size_t reference)
{
  impl->GetState ().SetStencilReference (reference);
}

IDepthStencilState* OutputStage::GetDepthStencilState () const
{
  return impl->GetDepthStencilState ();
}

size_t OutputStage::GetStencilReference () const
{
  return impl->GetState ().GetStencilReference ();
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

void OutputStage::InvalidateRenderTargets (const Rect& update_rect)
{
  try
  {    
    impl->InvalidateRenderTargets (update_rect);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::OutputStage::InvalidateRenderTargets");
    throw;
  }
}

void OutputStage::UpdateRenderTargets ()
{
  try
  {
    impl->UpdateRenderTargets ();
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::OutputStage::UpdateRenderTargets");
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
