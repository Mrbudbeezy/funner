#include "shared.h"

using namespace render::render2d;
using namespace scene_graph;

/*
    �����������
*/

RenderableSprite::RenderableSprite (scene_graph::Sprite* in_sprite, IRenderer& renderer)
  : Renderable (in_sprite),
    sprite (in_sprite),
    primitive (renderer.CreatePrimitive (), false)
{
  RenderableSprite::UpdateNotify ();
}

/*
    ���������� �� ���������� ���������� scene_graph::Sprite
*/

void RenderableSprite::UpdateNotify ()
{
}

/*
    ���������
*/

void RenderableSprite::Draw (IFrame& frame)
{
  printf ("Draw sprite name='%s' material='%s' frame=%u\n", sprite->Name (), sprite->Material (), sprite->Frame ());
}
