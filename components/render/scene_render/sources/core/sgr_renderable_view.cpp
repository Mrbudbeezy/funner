#include "shared.h"

using namespace render;

/*
    �������� ���������� ������� ���������
*/

struct RenderableView::Impl: public scene_graph::IViewportListener, public scene_graph::IScreenListener
{
  RenderManager&         manager;  //�������� ����������
  scene_graph::Screen&   screen;   //�����
  scene_graph::Viewport& viewport; //������� ������
  ISceneRenderPtr        renderer; //������ �����  
  
///�����������
  Impl (RenderManager& in_manager, scene_graph::Screen& in_screen, scene_graph::Viewport& in_viewport)
    : manager (in_manager)
    , screen (in_screen)
    , viewport (in_viewport)
    , renderer (SceneRenderManagerSingleton::Instance ()->CreateRender (manager, viewport.Renderer ()))
  {    
  }

///������� ������ ���������
  void OnScreenChangeArea (const Rect& new_area)
  {
  }

///������� ������ ���������
  void OnViewportChangeArea (const Rect& new_area)
  {
  }

///�������� ������
  void OnViewportChangeCamera (scene_graph::Camera* new_camera)
  {
  }

///������� ������� �������� ������
  void OnViewportChangeZOrder (int new_z_order)
  {
  }

///�������� ���������� ������� ������
  void OnViewportChangeActive (bool new_state)
  {
  }
  
///�������� ��������� ������� ������� ������
  void OnViewportChangeBackground (bool new_state, const math::vec4f& new_color)
  {
  }

///������� ���� ����������
  void OnViewportChangeRenderer (const char* new_renderer)
  {
  }

///�������� �������� ����������
  void OnViewportChangeProperties (const common::PropertyMap& properties)
  {
  }
};

/*
    ����������� / ����������
*/

RenderableView::RenderableView (RenderManager& manager, scene_graph::Screen& screen, scene_graph::Viewport& viewport)
{
  try
  {
    impl = new Impl (manager, screen, viewport);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderableView::RenderableView");
    throw;
  }
}

RenderableView::~RenderableView ()
{
}

/*
    ������� ������ / �����
*/

const scene_graph::Viewport& RenderableView::Viewport ()
{
  return impl->viewport;
}

const scene_graph::Screen& RenderableView::Screen ()
{
  return impl->screen;
}

/*
    ����������� ������� ������� ���������
*/

void RenderableView::SetRenderTarget (const char* name, const RenderTarget& target)
{
  throw xtl::make_not_implemented_exception ("render::RenderableView::SetRenderTarget");
}

void RenderableView::RemoveRenderTarget (const char* name)
{
  throw xtl::make_not_implemented_exception ("render::RenderableView::RemoveRenderTarget");
}

/*
    ���������� �����
*/

void RenderableView::UpdateFrame (Frame* parent_frame)
{
  throw xtl::make_not_implemented_exception ("render::RenderableView::Update");
}
