#include "shared.h"

using namespace render;
using namespace render::render2d;

/*
    ����������� / ����������
*/

SceneRender2d::SceneRender2d ()
{
}

SceneRender2d::~SceneRender2d ()
{
}

/*
    ��������� �����
*/

void SceneRender2d::Render (IRenderContext& render_context)
{
  throw xtl::make_not_implemented_exception ("render::render2d::SceneRender2d::Render");
}

/*
    ������� ������
*/

void SceneRender2d::AddRef ()
{
  addref (this);
}

void SceneRender2d::Release ()
{
  release (this);
}
