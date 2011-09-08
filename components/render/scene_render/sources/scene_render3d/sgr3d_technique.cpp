#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

/*
    �������� ���������� ������� ����������
*/

struct Technique::Impl
{
};

/*
    ����������� / ����������
*/

Technique::Technique ()
  : impl (new Impl)
{
}

Technique::~Technique ()
{
}

/*
    ���������� ������
*/

void Technique::UpdateCamera (scene_graph::Camera* camera)
{
  throw xtl::make_not_implemented_exception ("render::scene_render3d::Technique::UpdateCamera");
}

/*
    ���������� ������� ����������
*/

void Technique::UpdateProperties (const common::PropertyMap&)
{
  throw xtl::make_not_implemented_exception ("render::scene_render3d::Technique::UpdateProperties");
}

/*
    ���������
*/

void Technique::UpdateFrame (Scene& scene, Frame& frame)
{
  throw xtl::make_not_implemented_exception ("render::scene_render3d::Technique::Draw");
}
