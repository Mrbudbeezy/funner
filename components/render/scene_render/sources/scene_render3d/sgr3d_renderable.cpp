#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

/*
    �����������
*/

Renderable::Renderable (Scene& scene, scene_graph::Entity& entity)
  : Node (scene, entity)
{
}

/*
    ���������� �����
*/

void Renderable::UpdateFrame (Frame& frame)
{
  Update ();
  
  UpdateFrameCore (frame);
}

/*
    �����
*/

void Renderable::VisitCore (IVisitor& visitor)
{
  visitor.Visit (*this);
}
