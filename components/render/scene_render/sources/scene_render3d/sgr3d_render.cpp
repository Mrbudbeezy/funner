#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

/*
    �������� ���������� �������
*/

struct Render::Impl
{
  RenderManager& manager; //�������� ����������
  render::Frame  frame;   //����
  Log            log;     //����� ����������������
  
///�����������
  Impl (RenderManager& in_manager)
    : manager (in_manager)
    , frame (manager.CreateFrame ())
  {
    log.Printf ("SceneRender3D created");
  }
  
///����������
  ~Impl ()
  {
    log.Printf ("SceneRender3D destroyed");
  }
};

/*
    ����������� / ����������
*/

Render::Render (RenderManager& manager)
{
  try
  {
    impl = new Impl (manager);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::Render::Render");
    throw;
  }
}

Render::~Render ()
{
}

/*
    ����
*/

render::Frame& Render::Frame ()
{
  return impl->frame;
}

/*
    ���������� ������
*/

void Render::UpdateCamera (scene_graph::Camera* camera)
{
  throw xtl::make_not_implemented_exception ("render::scene_graph3d::Render::UpdateCamera");
}

/*
    ���������� ������� ����������
*/

void Render::UpdateProperties (const common::PropertyMap&)
{
  throw xtl::make_not_implemented_exception ("render::scene_graph3d::Render::UpdateProperties");
}

/*
    ���������� ����������� �����
*/

void Render::UpdateFrame ()
{
  throw xtl::make_not_implemented_exception ("render::scene_graph3d::Render::UpdateFrame");
}

/*
    ������� ������
*/

void Render::AddRef ()
{
  addref (this);
}

void Render::Release ()
{
  release (this);
}
