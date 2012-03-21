#include "shared.h"

using namespace render::obsolete;

/*
    �������� ���������� ����������� ������� ������
*/

class RenderableViewport::Impl: private IViewportListener, public xtl::reference_counter
{
  public:
///�����������
    Impl (const render::obsolete::Viewport& in_viewport, RenderTargetImpl& in_render_target, RenderManager& in_render_manager)
      : viewport (in_viewport),
        render_target (in_render_target),
        render_manager (in_render_manager),
        renderer (in_render_manager.Renderer ()),
        current_scene (0),
        current_camera (0),
        need_update_view (true),
        need_update_area (true),
        need_update_camera (true),
        need_update_path (true),
        need_update_clear_frame (true)
    {
        //�������� �� ������� ������� ������

      viewport.AttachListener (this);

        //���������� � ������������� �������������� �������� ������

      render_target.UpdateViewportsOrder ();
    }

///����������
    ~Impl ()
    {
        //������ ����������� ���������

      viewport.DetachListener (this);
    }
    
///��������� ������� ������
    render::obsolete::Viewport& Viewport () { return viewport; }
    
///���������� ������� ������
    void Update ()
    {
        //���������� ������� ����������

      if (need_update_view)
        UpdateRenderView ();
        
        //���������� ���������� �����
        
      if (need_update_clear_frame)
        UpdateClearFrame ();

      if (!render_view || !viewport.IsActive ())
        return;

        //������� �����

      if (viewport.BackgroundState ())
      {
        render_manager.Renderer ().AddFrame (clear_frame.get ());
      }

        //������������

      render_view->Draw ();
    }

///���������� � ������������� ��������� ������ ������� ������
    void UpdateArea ()
    {
      need_update_view        = true;
      need_update_area        = true;
      need_update_clear_frame = true;
    }

  private:
///���������� ���������� ������� ������
    void UpdateRenderView ()
    {
      try
      {
          //����������� ������������ ��������

        scene_graph::Camera* camera = viewport.Camera ();
        scene_graph::Scene*  scene  = camera ? camera->Scene () : 0;
        Screen*              screen = render_target.Screen ();
        
        if (!screen)
          throw xtl::format_operation_exception ("", "Null screen");

        if (camera && current_camera != camera)
        {
            //�������� �� ��������� ����� � ������

          on_camera_scene_change = camera->RegisterEventHandler (scene_graph::NodeEvent_AfterSceneChange,
            xtl::bind (&Impl::OnChangeScene, this));

          current_camera = camera;
        }      

        if (need_update_path || scene != current_scene) //������� ������������ ������� ������
        {
          render_view      = 0;
          current_scene    = 0;
          need_update_path = need_update_camera = need_update_view = need_update_area = false;
        }

            //������������ ������� ������

        if (!render_view)
        {
            //��� ���������� ������ ��� ����� ���������� ������� ������� ����������

          if (!camera || !scene)
            return;            

            //�������� ������� ����������

          ICustomSceneRender& render_path = render_manager.GetRenderPath (viewport.RenderPath ());

          RenderViewPtr new_render_view (render_path.CreateRenderView (scene), false);

            //��������� ������� ������� ����������

          new_render_view->SetRenderTargets (render_target.ColorAttachment (), render_target.DepthStencilAttachment ());

            //��������� ������� ������� ������ � ������� ����������

          for (Viewport::PropertyIterator prop_iter=viewport.CreatePropertyIterator (); prop_iter; ++prop_iter)
            new_render_view->SetProperty (prop_iter->Name (), prop_iter->Value ());        

            //����� ���������� ���������� ���� ���������� ������

          need_update_area = need_update_camera = true;
          need_update_path = false;
          render_view      = new_render_view;            
        }

          //���������� ������

        if (need_update_camera)
        {
          render_view->SetCamera (camera);

          current_scene = scene;

          need_update_camera = false;
        }

          //���������� ���������� ������� ������
          
        if (need_update_area)
        {
          const Rect &renderable_area = render_target.RenderableArea (),
                     &screen_area     = screen->Area (),
                     &viewport_area   = viewport.Area ();

          float x_scale  = screen_area.width ? float (renderable_area.width) / screen_area.width : 0.0f,
                y_scale  = screen_area.height ? float (renderable_area.height) / screen_area.height : 0.0f;
                
          Rect result (int (renderable_area.left + (viewport_area.left - screen_area.left) * x_scale),
                       int (renderable_area.top + (viewport_area.top - screen_area.top) * y_scale),
                       size_t (ceil (viewport_area.width * x_scale)),
                       size_t (ceil (viewport_area.height * y_scale)));
                       
          render_view->SetViewport (result, viewport.MinDepth (), viewport.MaxDepth ());

          need_update_area = false;
        }

        need_update_view = false;
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::RenderableViewport::UpdateRenderView");

        throw;
      }
    }
    
///���������� ���������� �����
    void UpdateClearFrame ()
    {
        //����� ���������� �����

      if (!viewport.BackgroundState () || !render_view)
      {
        if (clear_frame)
          clear_frame = 0;

        return;
      }

        //�������� ���������� ����� (��� �������������)

      if (!clear_frame)
      {
        clear_frame = ClearFramePtr (renderer.CreateClearFrame (), false);

        clear_frame->SetRenderTargets (render_target.ColorAttachment (), render_target.DepthStencilAttachment ());
        clear_frame->SetFlags         (render::mid_level::ClearFlag_All | render::mid_level::ClearFlag_ViewportOnly);
      }

          //��������� ��������� ������� ������

      Rect src_viewport_rect;
      
      float min_depth = 0.0f, max_depth = 1.0f;

      render_view->GetViewport (src_viewport_rect, min_depth, max_depth);

      render::mid_level::Viewport dst_viewport_rect;

      dst_viewport_rect.x         = src_viewport_rect.left;
      dst_viewport_rect.y         = src_viewport_rect.top;
      dst_viewport_rect.width     = src_viewport_rect.width;
      dst_viewport_rect.height    = src_viewport_rect.height;
      dst_viewport_rect.min_depth = min_depth;
      dst_viewport_rect.max_depth = max_depth;

      clear_frame->SetViewport (dst_viewport_rect);

        //���������� ���� �������

      clear_frame->SetColor (viewport.BackgroundColor ());

        //������ ����� ������������� ���������� ���������� �����

      need_update_clear_frame = false;
    }

///���������� �� ��������� ������� ������
    void OnChangeCamera (scene_graph::Camera*)
    {
      need_update_view   = true;
      need_update_camera = true;

      on_camera_scene_change.disconnect ();
    }

///���������� � ������������� ��������� ������ ������� ������
    void OnChangeArea (const Rect&, float, float)
    {
      UpdateArea ();
    }

///���������� � ������������� ��������� ���� ����������
    void OnChangeRenderPath (const char*)
    {
      need_update_view = true;
      need_update_path = true;
    }

///���������� �� ��������� ������� ���������� �������� ������
    void OnChangeZOrder (int)
    {
      render_target.UpdateViewportsOrder ();
    }

///���������� � ����� �����
    void OnChangeScene ()
    {
      need_update_view = true;
    }

///���������� �� ��������� �������� ����������
    void OnChangeProperty (const char* name, const char* value)
    {
      if (!render_view)
        return;

      render_view->SetProperty (name, value);
    }

///���������� �� ��������� ����
    void OnChangeBackground (bool, const math::vec4f&)
    {
      need_update_clear_frame = true;
    }

  private:
    typedef xtl::com_ptr<IRenderView>                    RenderViewPtr;
    typedef xtl::com_ptr<render::mid_level::IClearFrame> ClearFramePtr;
    typedef xtl::com_ptr<render::mid_level::IRenderer>   RendererPtr;

  private:
    render::obsolete::Viewport      viewport;                //������� ������
    RenderTargetImpl&     render_target;           //���� ����������
    RenderManager&        render_manager;          //�������� ����������
    mid_level::IRenderer& renderer;                //������� ����������
    RenderViewPtr         render_view;             //������� ���������� �����
    scene_graph::Scene*   current_scene;           //������� �����
    scene_graph::Camera*  current_camera;          //������� ������
    ClearFramePtr         clear_frame;             //���� �������
    bool                  need_update_view;        //���������� �������� ��������� ������� ������
    bool                  need_update_area;        //���������� �������� ���������� ������� ������
    bool                  need_update_camera;      //���������� �������� ������
    bool                  need_update_path;        //���������� �������� ���� ����������
    bool                  need_update_clear_frame; //���������� �������� ��������� ����
    xtl::auto_connection  on_camera_scene_change;  //���������� � �������� ���������� �� ��������� ����� � ������
};

/*
    ������������ / ���������� / ������������
*/

RenderableViewport::RenderableViewport (const render::obsolete::Viewport& viewport, RenderTargetImpl& render_target, RenderManager& render_manager)
{
  try
  {
    impl = new Impl (viewport, render_target, render_manager); 
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderableViewport::RenderableViewport");
    throw;
  }
}

RenderableViewport::RenderableViewport (const RenderableViewport& view)
  : impl (view.impl)
{
  addref (impl);
}

RenderableViewport::~RenderableViewport ()
{
  release (impl);
}

RenderableViewport& RenderableViewport::operator = (const RenderableViewport& view)
{
  RenderableViewport new_view (view);

  stl::swap (impl, new_view.impl);

  return *this;
}

/*
    ��������� ������� ������
*/

render::obsolete::Viewport& RenderableViewport::Viewport ()
{
  return impl->Viewport ();
}

/*
    ���������� ������ ������
*/

void RenderableViewport::Update ()
{
  impl->Update ();
}

/*
    ���������� � ������������� ��������� ������ ������� ������
*/

void RenderableViewport::UpdateArea ()
{
  impl->UpdateArea ();
}
