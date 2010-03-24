#include "shared.h"

using namespace render;
using namespace render::render2d;
using namespace scene_graph;

/*
    ����������� / ����������
*/

Renderable::Renderable (scene_graph::Entity* entity)
  : on_update_connection (entity->RegisterEventHandler (NodeEvent_AfterUpdate, xtl::bind (&Renderable::UpdateNotify, this)))
  , need_update (true)
  , video_position (0)
{
}

/*
    ���������� �� ���������� �������
*/

void Renderable::UpdateNotify ()
{
  need_update = true;
}

/*
    ���������� ��� ���������� ����� �������
*/

void Renderable::SetVideoPosition (float position)
{
  video_position = position;
}

float Renderable::VideoPosition () const
{
  return video_position;
}

/*
    ���������
*/

void Renderable::Draw (IFrame& frame)
{
  if (need_update)
  {
    Update ();
    
    need_update = false;
  }

  for (PrerequisiteList::iterator iter=prerender.begin (), end=prerender.end (); iter!=end; ++iter)
    (*iter)->Update ();

  DrawCore (frame);
}

/*
    ���������� ��������, ��������� �������������
*/

void Renderable::AddPrerender (const PrerequisitePtr& object)
{
  if (!object)
    return;
    
  for (PrerequisiteList::iterator iter=prerender.begin (), end=prerender.end (); iter!=end; ++iter)  
    if (*iter == object)
      return;

  prerender.push_back (object);
}
