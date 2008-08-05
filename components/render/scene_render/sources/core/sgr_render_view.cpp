#include "scene_render_shared.h"

using namespace render;

/*
    ����������� / ����������
*/

RenderView::RenderView (const render::Viewport& vp, IRenderTargetAPI& in_render_target_api)
  : viewport (vp),
    render_target_api (in_render_target_api),
    current_scene (0),
    current_camera (0),
    need_update_view (true),
    need_update_area (true),
    need_update_camera (true),
    need_update_path (true),
    need_update_clear_frame (true)
{
  viewport.AttachListener (this);
  
  render_target_api.UpdateOrderNotify ();
}

RenderView::~RenderView ()
{
  viewport.DetachListener (this);
}    

/*
    ���������
*/

void RenderView::Draw ()
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

  if (viewport.HasBackground ())
  {
    render_target_api.GetRenderer ().AddFrame (clear_frame.get ());
  }

    //������������

  render_view->Draw ();
}

/*
    ���������� ���������� ������� ����������
*/

void RenderView::UpdateRenderView ()
{
  try
  {
      //����������� �����

    scene_graph::Camera* camera = viewport.Camera ();
    scene_graph::Scene*  scene  = camera ? camera->Scene () : 0;
    
    if (camera && current_camera != camera)
    {
        //�������� �� ��������� ����� � ������

      on_camera_scene_change = camera->RegisterEventHandler (scene_graph::NodeEvent_AfterSceneChange,
        xtl::bind (&RenderView::OnChangeScene, this));

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
        //��������� ��������� ����� ����������
        
      RenderPathManager* render_path_manager = render_target_api.GetRenderPathManager ();

        //��� ���������� ������, ����� ��� ��������� ����� ���������� ���������� ������� ������� ����������

      if (!camera || !scene || !render_path_manager)
        return;            

        //�������� ������� ����������

      ICustomSceneRender& render = render_path_manager->GetRenderPath (viewport.RenderPath ());

      RenderViewPtr new_render_view (render.CreateRenderView (scene), false);

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
      const Rect &renderable_area = render_target_api.GetRenderableArea (),
                 &screen_area     = render_target_api.GetScreenArea (),
                 &viewport_area   = viewport.Area ();

      float x_scale  = float (renderable_area.width) / screen_area.width,
            y_scale  = float (renderable_area.height) / screen_area.height;
            
      Rect result (int (renderable_area.left + (viewport_area.left - screen_area.left) * x_scale),
                   int (renderable_area.top + (viewport_area.top - screen_area.top) * y_scale),
                   size_t (viewport_area.width * x_scale),
                   size_t (viewport_area.height * y_scale));

      render_view->SetViewport (result);
      
      need_update_area = false;
    }

    need_update_view = false;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderView::UpdateRenderView");

    throw;
  }
}

/*
    ���������� ���������� �����
*/

void RenderView::UpdateClearFrame ()
{  
    //����� ���������� �����

  if (!viewport.HasBackground () || !render_view)
  {
    if (clear_frame)
      clear_frame = 0;

    return;
  }
  
    //�������� ���������� ����� (��� �������������)
    
  if (!clear_frame)
  {
    render::mid_level::IRenderer& renderer = render_target_api.GetRenderer ();        
    
    clear_frame = ClearFramePtr (renderer.CreateClearFrame (), false);

    clear_frame->SetRenderTargets (renderer.GetColorBuffer (), renderer.GetDepthStencilBuffer ()); ////��������!!!
    clear_frame->SetFlags         (render::mid_level::ClearFlag_All | render::mid_level::ClearFlag_ViewportOnly);    
  }
  
      //��������� ��������� ������� ������

  Rect src_viewport_rect;

  render_view->GetViewport (src_viewport_rect);

  render::mid_level::Viewport dst_viewport_rect;

  dst_viewport_rect.x      = src_viewport_rect.left;
  dst_viewport_rect.y      = src_viewport_rect.top;
  dst_viewport_rect.width  = src_viewport_rect.width;
  dst_viewport_rect.height = src_viewport_rect.height;

  clear_frame->SetViewport (dst_viewport_rect);

    //���������� ���� �������

  clear_frame->SetColor (viewport.BackgroundColor ());

    //������ ����� ������������� ���������� ���������� �����

  need_update_clear_frame = false;
}

/*
    ����� ��������
*/

void RenderView::FlushResources ()
{
  need_update_view        = true;
  need_update_area        = true;
  need_update_path        = true;
  need_update_clear_frame = true;

  render_view = 0;
  clear_frame = 0;
}

/*
    ����������� �������
*/

//���������� �� ��������� ������� ������
void RenderView::OnChangeCamera (scene_graph::Camera*)
{
  need_update_view   = true;
  need_update_camera = true;

  on_camera_scene_change.disconnect ();
}

//���������� � ������������� ��������� ������ ������� ������ (�������� �� RenderTarget)
void RenderView::UpdateAreaNotify ()
{
  need_update_view        = true;
  need_update_area        = true;
  need_update_clear_frame = true;
}

//���������� � ������������� ��������� ������ ������� ������ (�������� �� Viewport)
void RenderView::OnChangeArea (const Rect&)
{
  UpdateAreaNotify ();
}

//���������� � ������������� ��������� ���� ����������
void RenderView::OnChangeRenderPath (const char*)
{
  need_update_view = true;
  need_update_path = true;
}

//���������� �� ��������� ������� ���������� �������� ������
void RenderView::OnChangeZOrder (int)
{
  render_target_api.UpdateOrderNotify ();
}

//���������� � ����� �����
void RenderView::OnChangeScene ()
{
  need_update_view = true;
}

//���������� �� ��������� �������� ����������
void RenderView::OnChangeProperty (const char* name, const char* value)
{
  if (!render_view)
    return;

  render_view->SetProperty (name, value);
}

void RenderView::OnChangeBackground (bool, const math::vec4f&)
{
  need_update_clear_frame = true;
}

/*
    ������� ������ (�� ���������, ����������� ��������� ����� �������� ��� �������� RenderView ��� ������� RenderTarget)
*/

void RenderView::AddRef ()
{
}

void RenderView::Release ()
{
}
