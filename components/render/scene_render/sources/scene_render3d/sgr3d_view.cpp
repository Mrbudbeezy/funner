#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

/*
    �������� ���������� �������
*/

struct View::Impl
{
  RenderPtr      render;  //������
  render::Frame  frame;   //����
  Log            log;     //����� ����������������

///�����������
  Impl (RenderManager& in_manager)
    : render (Render::GetRender (in_manager))
    , frame (in_manager.CreateFrame ())
  {
    log.Printf ("View created");
  }
  
///����������
  ~Impl ()
  {
    log.Printf ("View destroyed");
  }
};

/*
    ����������� / ����������
*/

View::View (RenderManager& manager)
{
  try
  {
    impl = new Impl (manager);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::View::View");
    throw;
  }
}

View::~View ()
{
}

/*
    ����
*/

render::Frame& View::Frame ()
{
  return impl->frame;
}

/*
    ���������� ������
*/

void View::UpdateCamera (scene_graph::Camera* camera)
{
  throw xtl::make_not_implemented_exception ("render:scene_graph3d::View::UpdateCamera");
}

/*
    ���������� ������� ����������
*/

void View::UpdateProperties (const common::PropertyMap&)
{
  throw xtl::make_not_implemented_exception ("render:scene_graph3d::View::UpdateProperties");
}

/*
    ���������� ����������� �����
*/

void View::UpdateFrame ()
{
  throw xtl::make_not_implemented_exception ("render:scene_graph3d::View::UpdateFrame");
}

/*
    ������� ������
*/

void View::AddRef ()
{
  addref (this);
}

void View::Release ()
{
  release (this);
}
