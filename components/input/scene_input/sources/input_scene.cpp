#include "shared.h"

using namespace scene_graph;
using namespace input;

/*
    ��������� / ��������
*/

InputScene::InputScene (scene_graph::Scene& in_scene)
  : scene (in_scene)
{
}

InputScene::~InputScene ()
{
  entities.clear ();
}

/*
    ���� ���ﭨ� ����⨩
*/

void InputScene::ResetTouchState ()
{
  entities.clear ();
}

/*
    ����祭�� ��ꥪ� �����
*/

InputEntityPtr InputScene::GetEntity (const scene_graph::InputZoneModel& zone)
{
  try
  {
    EntityMap::iterator iter = entities.find (&zone);
    
    if (iter != entities.end ())
      return iter->second.entity;

    EntityDesc& desc = entities [&zone];
    
    try
    {
      desc.entity                = InputEntityPtr (new InputEntity (zone), false);
      desc.on_destroy_connection = zone.RegisterEventHandler (NodeEvent_AfterDestroy, xtl::bind (&InputScene::OnInputZoneDestroyed, this, &zone));
      
      return desc.entity;
    }
    catch (...)
    {
      entities.erase (&zone);
      throw;
    }      
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::InputScene::GetEntity");
    throw;
  }
}

/*
    �����饭�� �� 㤠����� ���� �����
*/

void InputScene::OnInputZoneDestroyed (const scene_graph::InputZoneModel* zone)
{
  entities.erase (zone);
}
