#include "shared.h"

using namespace render;
using namespace render::obsolete::render2d;
using namespace scene_graph;

namespace
{

const float MINIMAL_NODE_SCALE = 0.00000001f;

bool has_zero_component_or_nan (const math::vec3f& v, float eps)
{
  for (size_t i=0; i < 3; i++)
    if (v [i] != v [i] || fabs (v [i]) <= eps)
      return true;

  return false;
}

bool has_nan (const math::vec3f& v)
{
  for (size_t i=0; i < 3; i++)
    if (v [i] != v [i])
      return true;

  return false;
}

/*
    ���������� �������� �����
*/

struct RenderViewVisitor: public xtl::visitor<void, SpriteModel, TextLine, HeightMap, PageCurl>
{
  typedef xtl::intrusive_ptr<Render> RenderPtr;

  FramePtr  frame;  //���� ���������
  RenderPtr render; //������ 

  RenderViewVisitor (const FramePtr& in_frame, const RenderPtr& in_render) : frame (in_frame), render (in_render) {}

  void visit (SpriteModel& model)
  {
    if (has_zero_component_or_nan (model.WorldScale (), MINIMAL_NODE_SCALE) || has_nan (model.WorldPosition ()))
      return;

    Renderable* renderable = render->GetRenderable (&model);
    
    if (renderable)
      renderable->Draw (*frame);
  }

  void visit (HeightMap& height_map)
  {
    if (has_zero_component_or_nan (height_map.WorldScale (), MINIMAL_NODE_SCALE) || has_nan (height_map.WorldPosition ()))
      return;

    Renderable* renderable = render->GetRenderable (&height_map);
    
    if (renderable)
      renderable->Draw (*frame);
  }
  
  void visit (TextLine& text_line)
  {
    if (has_zero_component_or_nan (text_line.WorldScale (), MINIMAL_NODE_SCALE) || has_nan (text_line.WorldPosition ()))
      return;

    Renderable* renderable = render->GetRenderable (&text_line);
    
    if (renderable)
      renderable->Draw (*frame);
  }

  void visit (PageCurl& page_curl)
  {
    if (has_zero_component_or_nan (page_curl.WorldScale (), MINIMAL_NODE_SCALE) || has_nan (page_curl.WorldPosition ()))
      return;

    Renderable* renderable = render->GetRenderable (&page_curl);

    if (renderable)
      renderable->Draw (*frame);
  }
};

}

/*
    ����������� / ����������
*/

RenderView::RenderView (Scene* in_scene, Render* in_render)
  : render (in_render),
    scene (in_scene),
    camera (0)
{
  try
  {
    if (!scene)
      throw xtl::make_null_argument_exception ("", "scene");
      
    if (!render)
      throw xtl::make_null_argument_exception ("", "render");
      
      //�������� �����
      
    IRenderer& renderer = *render->Renderer ();
    
    frame = FramePtr (renderer.CreateFrame (), false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::render2d::RenderView::RenderView");
    throw;
  }    
}

RenderView::~RenderView ()
{
}

/*
    ������� ������ ����������
*/

void RenderView::SetRenderTargets (IRenderTarget* render_target, IRenderTarget* depth_stencil_target)
{
  frame->SetRenderTargets (render_target, depth_stencil_target);
}

mid_level::IRenderTarget* RenderView::GetRenderTarget ()
{
  return frame->GetRenderTarget ();  
}

mid_level::IRenderTarget* RenderView::GetDepthStencilTarget ()
{
  return frame->GetDepthStencilTarget ();
}

/*
    ��������� ������� ������
*/

void RenderView::SetViewport (const render::obsolete::Rect& rect)
{
  try
  {
    render::mid_level::Viewport viewport;
    
    viewport.x      = rect.left;
    viewport.y      = rect.top;
    viewport.width  = rect.width;
    viewport.height = rect.height;
    
    frame->SetViewport (viewport);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::render2d::RenderView::SetViewport");
    throw;
  }
}

void RenderView::GetViewport (render::obsolete::Rect& out_rect)
{
  try
  {
    render::mid_level::Viewport viewport;
    
    frame->GetViewport (viewport);
    
    out_rect.left   = viewport.x;
    out_rect.top    = viewport.y;
    out_rect.width  = viewport.width;
    out_rect.height = viewport.height;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::render2d::RenderView::GetViewport");
    throw;
  }
}

/*
    ��������� ������
*/

void RenderView::SetCamera (scene_graph::Camera* in_camera)
{
  camera = in_camera;
}

scene_graph::Camera* RenderView::GetCamera ()
{
  return camera;
}

/*
    ��������� / ������ �������
*/

void RenderView::SetProperty (const char*, const char*)
{
}

void RenderView::GetProperty (const char*, size_t buffer_size, char* value_buffer)
{
  if (buffer_size && value_buffer)
    *value_buffer = 0;
}

/*
    ���������
*/

void RenderView::Draw ()
{
  if (!camera)
    return;    

    //������� �����

  frame->Clear ();

    //��������� ������ ���� � ��������    

  frame->SetProjection (camera->ProjectionMatrix ());
  frame->SetViewPoint  (camera->WorldPosition ());

    //����� �����

  RenderViewVisitor visitor (frame, render);

  scene->VisitEach (camera->Frustum (), visitor);

    //���������� ����� �� ���������
    
  render->Renderer ()->AddFrame (frame.get ());
}

/*
    ������� ������
*/

void RenderView::AddRef ()
{
  addref (this);
}

void RenderView::Release ()
{
  release (this);
}
