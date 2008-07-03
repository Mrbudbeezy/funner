#include "shared.h"

using namespace render;
using namespace render::render2d;
using namespace scene_graph;

namespace
{

/*
    ���������� �������� �����
*/

struct RenderViewVisitor: public xtl::visitor<void, Sprite>
{
  typedef xtl::intrusive_ptr<Render> RenderPtr;

  FramePtr  frame;  //���� ���������
  RenderPtr render; //������ 

  RenderViewVisitor (const FramePtr& in_frame, const RenderPtr& in_render) : frame (in_frame), render (in_render) {}

  void visit (Sprite& sprite)
  {
    Renderable* renderable = render->GetRenderable (&sprite);
    
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

    frame->SetClearBuffers  (true, true);
    frame->SetRenderTargets (renderer.GetColorBuffer (), renderer.GetDepthStencilBuffer ());

      //��������� ������� ������ �� ���������
      
    SetViewport (0, 0, 100.0f, 100.0f);
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
    ��������� ������� ������
*/

void RenderView::SetViewport (float left, float top, float width, float height)
{
  try
  {
    viewport [0] = left;
    viewport [1] = top;
    viewport [2] = width;
    viewport [3] = height;

    mid_level::IRenderTarget& screen = *render->Renderer ()->GetColorBuffer ();

    size_t screen_width  = screen.GetWidth (),
           screen_height = screen.GetHeight ();
           
    mid_level::Viewport viewport_rect;
    
    viewport_rect.x      = int (left * screen_width);
    viewport_rect.y      = int (top * screen_height);
    viewport_rect.width  = size_t (width * screen_width);
    viewport_rect.height = size_t (height * screen_height);

    frame->SetViewport (viewport_rect);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::render2d::RenderView::SetViewport");
    throw;
  }
}

void RenderView::GetViewport (float out_viewport [4])
{
  for (size_t i=0; i<4; i++)
    out_viewport [i] = viewport [i];
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

    //��������� ����� �������

  math::vec4f clear_color;

  render->GetBackgroundColor (clear_color);

  frame->SetClearColor (clear_color);    
  
    //��������� ������ ���� � ��������
    
  frame->SetProjection (camera->ProjectionMatrix ());
  frame->SetView       (invert (camera->WorldTM ()));

    //����� �����

  RenderViewVisitor visitor (frame, render);

  scene->VisitEach (visitor);
  
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
