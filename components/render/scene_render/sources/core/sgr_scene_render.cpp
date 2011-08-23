#include "shared.h"

using namespace render;

/*
    �������� ���������� ������� �����
*/

struct SceneRender::Impl: public xtl::reference_counter, public scene_graph::IScreenListener, public scene_graph::IViewportListener
{
  render::RenderManager* manager; //�������� ���������� ��������� � �������� �����
  scene_graph::Screen*   screen;  //�����
  
///�����������
  Impl ()
    : manager (0)
    , screen (0)
  {
  }
  
///��������� ������� ���� ������ ��������
  void OnScreenChangeBackground (bool state, const math::vec4f& new_color)
  {
  }

///������������ ������� ������
  void OnScreenAttachViewport (Viewport& viewport)
  {
  }

///����������� ������� ������
  void OnScreenDetachViewport (Viewport& viewport)
  {
  }

///����� ������
  void OnScreenDestroy ()
  {
  }
  
///������� ������ �������
  void OnViewportChangeZOrder (int)
  {
  }
};

/*
    ����������� / ����������
*/

SceneRender::SceneRender ()
  : impl (new Impl)
{
}

SceneRender::SceneRender (const SceneRender& render)
  : impl (render.impl)
{
  addref (impl);
}

SceneRender::~SceneRender ()
{
  release (impl);
}

SceneRender& SceneRender::operator = (const SceneRender& render)
{
  SceneRender (render).Swap (*this);
  
  return *this;
}

/*
    ��������� / ����� ������� ����������
*/

void SceneRender::SetRenderManager (render::RenderManager* manager)
{
  throw xtl::make_not_implemented_exception ("render::SceneRender::SetRenderManager");
}

render::RenderManager* SceneRender::RenderManager () const
{
  return impl->manager;
}

/*
    �������� ����� ���� ���������� � ��������
*/

bool SceneRender::IsBindedToRenderer () const
{
  return impl->manager && impl->screen;
}

/*
    ����� (�������� �������� - weak-ref)
*/

void SceneRender::SetScreen (scene_graph::Screen* screen)
{
  throw xtl::make_not_implemented_exception ("render::SceneRender::SetScreen");
}

scene_graph::Screen* SceneRender::Screen () const
{
  return impl->screen;
}

/*
    ����������� ������� ������� ���������
*/

void SceneRender::SetRenderTarget (const char* name, const render::RenderTarget& target)
{
  throw xtl::make_not_implemented_exception ("render::SceneRender::SetRenderTarget");
}

void SceneRender::RemoveRenderTarget (const char* name)
{
  throw xtl::make_not_implemented_exception ("render::SceneRender::RemoveRenderTarget");
}

void SceneRender::RemoveAllRenderTargets ()
{
  throw xtl::make_not_implemented_exception ("render::SceneRender::RemoveAllRenderTargets");
}

/*
    ��������� ������� ������� ���������
*/

bool SceneRender::HasRenderTarget (const char* name) const
{
  throw xtl::make_not_implemented_exception ("render::SceneRender::HasRenderTarget");
}

RenderTarget SceneRender::RenderTarget (const char* name) const
{
  throw xtl::make_not_implemented_exception ("render::SceneRender::RenderTarget");
}

/*
    ����������
*/

void SceneRender::Update ()
{
  throw xtl::make_not_implemented_exception ("render::SceneRender::Update");
}

/*
    �����
*/

void SceneRender::Swap (SceneRender& render)
{
  stl::swap (impl, render.impl);
}

namespace render
{

void swap (SceneRender& render1, SceneRender& render2)
{
  render1.Swap (render2);
}

}
