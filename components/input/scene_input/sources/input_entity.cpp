#include "shared.h"

using namespace scene_graph;
using namespace input;

/*
    ����������� / ����������
*/

InputEntity::InputEntity (const scene_graph::InputZoneModel& in_zone)
  : zone (in_zone)
{
}

InputEntity::~InputEntity ()
{
}

