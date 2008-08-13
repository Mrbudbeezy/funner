#include "scene_render_shared.h"

using namespace render;

namespace
{

/*
    ���������
*/

const size_t VIEW_ARRAY_RESERVE  = 8;   //������������� ���������� �������� ���������
const size_t DEFAULT_AREA_WIDTH  = 100; //������ �������� ������������ �� ���������
const size_t DEFAULT_AREA_HEIGHT = 100; //������ �������� ������������ �� ���������

/*
    ������ ���� ����������    
*/

class NullRenderTarget: public IRenderTargetImpl
{
  public:
///����� ��������������� �������
    const char* ColorAttachment () { return ""; }
    const char* DepthStencilAttachment () { return ""; }

///������� �����
    render::Screen* Screen () { return 0; }

///����� �������� ������
    void SetScreen (render::Screen*)
    {
      Raise ("render::NullRenderTarget::SetScreen");
    }
    
///������� ���������� ���� ������
    const Rect& ScreenArea () { return screen_area; }
    
///��������� ����������� ���� ������
    void SetScreenArea (const Rect&)
    {
      Raise ("render::NullRenderTarget::SetScreenArea");
    }
    
///������� ���������� ���� ������
    const Rect& RenderableArea () { return renderable_area; }
    
///��������� ���������� ���� ������
    void SetRenderableArea (const Rect&)
    {
      Raise ("render::NullRenderTarget::SetRenderableArea");
    }

///�����������
    void Draw ()
    {
      Raise ("render::NullRenderTarget::Draw");
    }
    
///������ �����������
    void CaptureImage (media::Image&)
    {
      Raise ("render::NullRenderTarget::CaptureImage");
    }

///��������� ����������
    static NullRenderTarget* Instance ()
    {
      static char              buffer [sizeof NullRenderTarget];
      static NullRenderTarget* target = new (buffer) NullRenderTarget;

      return target;
    }
    
///����� ��������
    void FlushResources () {}

  private:
    NullRenderTarget () {}
  
    void Raise (const char* source)
    {
      throw xtl::format_operation_exception (source, "Invalid operation on null render-target");      
    }
    
  private:
    Rect screen_area;
    Rect renderable_area;
};

}

/*
    �������� ����������
*/

namespace render
{

struct DrawContext
{
  RenderTargetManager&  render_target_manager;      //�������� ����� ����������
  mid_level::IRenderer& renderer;                   //������� ������������
  DrawContext*          parent;                     //������������ ��������
  size_t                child_start_frame_position; //��������� ������ ������ ��� ��������� ����������  
  size_t                parent_frame_offset;        //���������� ��������� ������ ������������� ���������
  size_t                depth_available;            //���������� ��������� ������� ����������� ����������  
  size_t                transaction_id;             //������������� ���������� ����������

  DrawContext (RenderTargetManager& manager) :
    render_target_manager (manager),
    renderer (manager.Renderer ()),
    parent (manager.DrawContext ()),
    child_start_frame_position (parent ? parent->child_start_frame_position : 0),
    parent_frame_offset (renderer.GetFramePosition () - child_start_frame_position),
    depth_available (parent ? parent->depth_available - 1 : manager.MaxDrawDepth ())    
  {
      //��������� ������ ���������� ����������
      
    static size_t next_transaction_id = 0;

    transaction_id = next_transaction_id++;
    
    if (!transaction_id)
      transaction_id = next_transaction_id++; //��������� ����

      //��������� ������ ������ ������

    renderer.SetFramePosition (child_start_frame_position);
    
      //��������� ������ �������� ��������� ����������
      
    manager.SetDrawContext (this);
  }

  ~DrawContext ()
  {
    try
    {
        //��������� ������������� ��������� ����������

      render_target_manager.SetDrawContext (parent);

      if (parent)
      {
          //��������� ������������� ������ ������ ��������� ����������                    

        parent->child_start_frame_position = renderer.GetFramePosition ();

          //��������� ������������� �������� ������        

        renderer.SetFramePosition (parent->child_start_frame_position + parent_frame_offset);
      }
      else
      {
          //���� �������� �������� �������� - ������ ��� ����������� �����

        render_target_manager.DrawFrames ();
      }
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }
};

}

namespace
{

/*
    �������� ���������� ���� ����������
*/

class RenderTargetImpl: private IScreenListener, private RenderView::IRenderTargetAPI, public IRenderTargetImpl
{
  public:
///�����������
    RenderTargetImpl (RenderTargetManager& in_manager, const char* in_color_attachment_name, const char* in_depth_stencil_attachment_name) :
      manager (&in_manager),
      color_attachment_name (in_color_attachment_name),
      depth_stencil_attachment_name (in_depth_stencil_attachment_name),
      screen (0),
      screen_area (0, 0, DEFAULT_AREA_WIDTH, DEFAULT_AREA_HEIGHT),
      last_update_transaction_id (0),
      need_update_attachments (true),
      need_update_background (true),
      need_update_areas (true),
      need_reorder (true),
      dirty_renderable_area (false)
    {
        //�������������� ������ ��� �������� �������� ������

      views.reserve (VIEW_ARRAY_RESERVE);      
      
        //����������� ���� � ���������

      manager->Register (this);
    }

///����������
    ~RenderTargetImpl ()
    {
        //������ ����������� � ���������

      manager->Unregister (this);

        //����� �������� ������

      SetScreen (0);
    }

///����� ��������������� �������
    const char* ColorAttachment () { return color_attachment_name.c_str (); }
    const char* DepthStencilAttachment () { return depth_stencil_attachment_name.c_str (); }
    
///������� �����
    render::Screen* Screen () { return screen; }

///����� �������� ������
    void SetScreen (render::Screen* in_screen)
    {
        //������ ����������� ���������

      if (screen)
      {
        screen->DetachListener (this);
        views.clear ();
      }
        
        //��������� �������� ������

      screen = in_screen;

        //����������� ���������

      if (screen)
      {
        for (size_t i=0, count=screen->ViewportsCount (); i<count; i++)
          OnAttachViewport (screen->Viewport (i));

        screen->AttachListener (this);
      }
    }
    
///������� ���������� ���� ������
    const Rect& ScreenArea () { return screen_area; }
    
///��������� ����������� ���� ������
    void SetScreenArea (const Rect& rect)
    {
      if (screen_area.left == rect.left && screen_area.top == rect.top && screen_area.width == rect.width && screen_area.height == rect.height)
        return;      

      screen_area = rect;

      need_update_areas = true;
    }
    
///������� ���������� ���� ������
    const Rect& RenderableArea ()
    {
      if (!dirty_renderable_area && need_update_attachments)
        UpdateAttachments ();

      return renderable_area;
    }
    
///��������� ���������� ���� ������
    void SetRenderableArea (const Rect& rect)
    {
      if (renderable_area.left == rect.left && renderable_area.top == rect.top && renderable_area.width == rect.width && renderable_area.height == rect.height)
        return;      

      renderable_area = rect;

      need_update_areas     = true;
      dirty_renderable_area = true;
    }    
    
///�����������
    void Draw ()
    {
      static const char* METHOD_NAME = "render::RenderTarget::Impl::Draw";
      
      if (!screen)
        return;
        
      DrawContext* parent_draw_context = manager->DrawContext ();
        
      if (parent_draw_context)
      {
        if (last_update_transaction_id == parent_draw_context->transaction_id)
          return; //����������� ���������� ���������� �������� �����

        if (!parent_draw_context->depth_available)
          return; //����������� �� ������� ����������� ����������
      }

      try
      {      
        DrawContext draw_context (*manager);
        
          //������������ �������� ������
        
        if (need_reorder)
          SortViews ();
          
          //���������� ������ �������� ������

        if (need_update_areas)
          UpdateAreas ();
         
          //������� ���� ����������
        
        ClearRenderTarget ();
        
          //��������� �������� ������

        for (ViewArray::iterator iter=views.begin (), end=views.end (); iter!=end; ++iter)
        {
          Viewport& viewport = (*iter)->Viewport ();        
          
          try
          {
            (*iter)->Draw ();
          }
          catch (std::exception& exception)
          {
            LogPrintf ("%s\n    at draw viewport='%s', render_path='%s'", exception.what (), viewport.Name (), viewport.RenderPath ());
          }
          catch (...)
          {
            LogPrintf ("Unknown exception\n    at draw viewport='%s', render_path='%s'", viewport.Name (), viewport.RenderPath ());
          }
        }
      }
      catch (std::exception& exception)
      {
        LogPrintf ("%s\n    at RenderTargetImpl::Draw", exception.what ());
      }
      catch (...)
      {
        LogPrintf ("Unknown exception\n    at RenderTargetImpl::Draw");
      }

        //���������� ������ ���������� ���������� ��� ����������� ���������� ����������

      last_update_transaction_id = parent_draw_context ? parent_draw_context->transaction_id : 0;
    }    

///������ �����������
    void CaptureImage (media::Image& image)
    {      
      mid_level::IRenderTarget *render_target = 0, *depth_stencil_target = 0;
      
      RenderTargetImpl::GetRenderTargets (render_target, depth_stencil_target);
      
      if (!render_target)
        throw xtl::format_operation_exception ("render::RenderTargetImpl::CaptureImage", "Null render-target buffer");
        
      render_target->CaptureImage (image);
    }

  private:
///����������������
    void LogVPrintf (const char* format, va_list list)
    {
      if (!format)
        return;
        
      try
      {
        manager->LogMessage (common::vformat (format, list).c_str ());
      }
      catch (...)
      {
        //���������� ���� ����������
      }
    }

    void LogPrintf (const char* format, ...)
    {
      va_list list;
      
      va_start   (list, format);  
      LogVPrintf (format, list);
    }

///������� ���� ����������
    void ClearRenderTarget ()
    {
      try
      {
        if (!need_update_background && !clear_frame)
          return;

        mid_level::IRenderer& renderer = manager->Renderer ();
        
        if (need_update_background)
        {
          if (!screen->BackgroundState ())
          {
              //�������� ���������� �����

            clear_frame            = 0;
            need_update_background = false;

            return;
          }
          
          if (!clear_frame)
          {
              //�������� ���������� �����

            clear_frame = ClearFramePtr (renderer.CreateClearFrame (), false);
            
            mid_level::IRenderTarget *render_target = 0, *depth_stencil_target = 0;

            RenderTargetImpl::GetRenderTargets (render_target, depth_stencil_target);
            clear_frame->SetRenderTargets      (render_target, depth_stencil_target);
            clear_frame->SetFlags              (render::mid_level::ClearFlag_All);
          }

            //���������� ����� �������

          clear_frame->SetColor (screen->BackgroundColor ());

          need_update_background = false;
        }

          //�������          

        renderer.AddFrame (clear_frame.get ());
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::RenderTarget::Impl::ClearRenderTarget");
        throw;
      }
    }
  
///���������� ������ �������� ������
    void UpdateAreas ()
    {
      for (ViewArray::iterator iter=views.begin (), end=views.end (); iter!=end; ++iter)
        (*iter)->UpdateAreaNotify ();

      need_update_areas = false;
    }
    
///���������� ������� �������
    void UpdateAttachments ()
    {
      try
      {
        AttachmentPtr new_color_attachment (manager->GetAttachment (color_attachment_name.c_str ())),
                      new_depth_stencil_attachment (manager->GetAttachment (depth_stencil_attachment_name.c_str ()));
                      
          //������������� ��������� ��������������� ������� ����������
          
        mid_level::IRenderTarget* render_target = new_color_attachment ? new_color_attachment.get () : depth_stencil_attachment.get ();
        
        if (!dirty_renderable_area)
        {
          if (render_target)
          {
            renderable_area.left   = 0;
            renderable_area.top    = 0;
            renderable_area.width  = render_target->GetWidth ();
            renderable_area.height = render_target->GetHeight ();
          }                      
          else
          {
            renderable_area = Rect ();
          }
        }

        color_attachment         = new_color_attachment;
        depth_stencil_attachment = new_depth_stencil_attachment;                

        need_update_attachments = false;
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::RenderTargetImpl::UpdateAttachments");
        throw;
      }
    }

///���������� �������� ������
    void SortViews ()
    {
      struct ViewComparator
      {
        bool operator () (const ViewPtr& vp1, const ViewPtr& vp2) const { return vp1->Viewport ().ZOrder () < vp2->Viewport ().ZOrder (); }
      };

      stl::sort (views.begin (), views.end (), ViewComparator ());
      
      need_reorder = false;    
    }  
    
///��������� ������� ����������
    render::mid_level::IRenderer& GetRenderer () { return manager->Renderer (); }
    
///��������� ������ ������� ����������
    const Rect& GetRenderableArea  () { return renderable_area; }
    const Rect& GetScreenArea () { return screen_area; }
    
///��������� ���� ����������
    ICustomSceneRender& GetRenderPath (const char* name) { return manager->GetRenderPath (name); }
    
///��������� ��������������� ������� ����������
    void GetRenderTargets (mid_level::IRenderTarget*& render_target, mid_level::IRenderTarget*& depth_stencil_target)
    {
      if (need_update_attachments)
        UpdateAttachments ();      

      render_target        = color_attachment.get ();
      depth_stencil_target = depth_stencil_attachment.get ();
    }

///���������� �� ���������� ������� ���������� �������� ������    
    void UpdateOrderNotify ()
    {
      need_reorder = true;
    }
  
///���������� �� ��������� ����
    void OnChangeBackground (bool, const math::vec4f&)
    {
      need_update_background = true;
    }

///���������� � ���������� ������� ������
    void OnAttachViewport (Viewport& viewport)
    {
      views.push_back (ViewPtr (new RenderView (viewport, *this), false));
    }

///���������� �� �������� ������� ������
    void OnDetachViewport (Viewport& viewport)
    {
      size_t id = viewport.Id ();

      for (ViewArray::iterator iter=views.begin (); iter!=views.end ();)
        if ((*iter)->Viewport ().Id () == id) views.erase (iter);
        else                                  ++iter;
    }

///���������� �� �������� ������
    void OnDestroy ()
    {
      SetScreen (0);
    }

///����� ��������
    void FlushResources ()
    {
      for (ViewArray::iterator iter=views.begin (), end=views.end (); iter!=end; ++iter)
        (*iter)->FlushResources ();
        
      clear_frame              = 0;
      color_attachment         = 0;
      depth_stencil_attachment = 0;
      need_update_attachments  = true;
      need_update_background   = true;
    }

///������� ����� ������ (�� ���������, ����������� ��������� ����� �������� ��� �������� Impl ��� �������� ������)
    void AddRef () {}
    void Release () {}
    
  private:
    typedef xtl::intrusive_ptr<RenderTargetManager>      ManagerPtr;
    typedef xtl::com_ptr<render::mid_level::IClearFrame> ClearFramePtr;
    typedef xtl::intrusive_ptr<RenderView>               ViewPtr;
    typedef stl::vector<ViewPtr>                         ViewArray;
    typedef xtl::com_ptr<mid_level::IRenderTarget>       AttachmentPtr;

  private:
    ManagerPtr       manager;                       //�������� ����� ����������
    stl::string      color_attachment_name;         //��� ���������������� ������ �����
    stl::string      depth_stencil_attachment_name; //��� ���������������� ������ ������������� ���������
    AttachmentPtr    color_attachment;              //��������������� ����� �����
    AttachmentPtr    depth_stencil_attachment;      //��������������� ����� ������������� ���������
    render::Screen*  screen;                        //�����
    Rect             screen_area;                   //���������� ���� ������
    Rect             renderable_area;               //���������� ���� ������
    ViewArray        views;                         //������ �������� ����������
    ClearFramePtr    clear_frame;                   //��������� ����
    size_t           last_update_transaction_id;    //������������� ��������� ���������� ����������
    bool             need_update_attachments;       //���������� �������� ��������������� ������
    bool             need_update_background;        //���������� �������� ��������� ����
    bool             need_update_areas;             //���������� �������� ������� �������� ������
    bool             need_reorder;                  //���������� ��������������� ������� ������
    bool             dirty_renderable_area;         //����, ������������ ���������� ������������� ��������������� ���������� ������� ����������
};

}

/*
    ������������ / ���������� / ������������
*/

RenderTarget::RenderTarget ()
  : impl (NullRenderTarget::Instance ())
{
  addref (impl);
}

RenderTarget::RenderTarget (IRenderTargetImpl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

RenderTarget::RenderTarget (RenderTargetManager& manager, const char* color_attachment_name, const char* depth_stencil_attachment_name)
{
  static const char* METHOD_NAME = "render::RenderTarget";

  if (!color_attachment_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "color_attachment_name");
    
  if (!depth_stencil_attachment_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "depth_stencil_attachment_name");

  impl = new RenderTargetImpl (manager, color_attachment_name, depth_stencil_attachment_name);
}

RenderTarget::RenderTarget (const RenderTarget& target)
  : impl (target.impl)
{
  addref (impl);
}

RenderTarget::~RenderTarget ()
{
  release (impl);
}

RenderTarget& RenderTarget::operator = (const RenderTarget& target)
{
  RenderTarget (target).Swap (*this);
  
  return *this;
}

/*
    ������������� ���� ����������
*/

size_t RenderTarget::Id () const
{
  return reinterpret_cast<size_t> (get_pointer (impl));
}

/*
    ����� ��������������� �������
*/

const char* RenderTarget::Attachment (RenderTargetAttachment buffer_id) const
{  
  switch (buffer_id)
  {
    case RenderTargetAttachment_Color:        return impl->ColorAttachment ();
    case RenderTargetAttachment_DepthStencil: return impl->DepthStencilAttachment ();
    default:
      throw xtl::make_argument_exception ("render::RenderTarget::Attachment", "buffer_id");
  }
}

/*
    ������� ��������� ������������ ������� ������
*/

void RenderTarget::SetScreenArea (const Rect& rect)
{
  try
  {
    impl->SetScreenArea (rect);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderTarget::SetScreenArea");
    throw;
  }
}

void RenderTarget::SetScreenArea (int left, int top, size_t width, size_t height)
{
  SetScreenArea (Rect (left, top, width, height));
}

const Rect& RenderTarget::ScreenArea () const
{
  return impl->ScreenArea ();
}

/*
    ���������� ������� ��������������� ������� (��������, � ������� �����������)
*/

void RenderTarget::SetRenderableArea (const Rect& rect)
{
  try
  {
    impl->SetRenderableArea (rect);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderTarget::SetRenderableArea");
    throw;
  }
}

void RenderTarget::SetRenderableArea (int left, int top, size_t width, size_t height)
{
  SetRenderableArea (Rect (left, top, width, height));
}

const Rect& RenderTarget::RenderableArea () const
{
  return impl->RenderableArea ();
}

/*
    ����� (�������� �������� - weak-ref)
*/

void RenderTarget::SetScreen (render::Screen* screen)
{
  try
  {
    impl->SetScreen (screen);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderTarget::SetScreen");
    throw;
  }
}

Screen* RenderTarget::Screen ()
{
  return impl->Screen ();
}

const Screen* RenderTarget::Screen () const
{
  return impl->Screen ();
}

/*
    ����������
*/

void RenderTarget::Update ()
{
  try
  {
    impl->Draw ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderTarget::Update");
    throw;
  }  
}

/*
    ������ ����������� (screen-shot)
*/

void RenderTarget::CaptureImage (media::Image& image)
{
  try
  {
    impl->CaptureImage (image);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::SceneRender::CaptureImage(media::Image&)");
    throw;
  }
}

void RenderTarget::CaptureImage (const char* image_name)
{
  try
  {
    media::Image image;
    
    CaptureImage (image);
    
    image.Save (image_name);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::SceneRender::CaptureImage(const char*)");
    throw;
  }
}

/*
    �����
*/

void RenderTarget::Swap (RenderTarget& target)
{
  stl::swap (impl, target.impl);
}

namespace render
{

void swap (RenderTarget& target1, RenderTarget& target2)
{
  target1.Swap (target2);
}

}
