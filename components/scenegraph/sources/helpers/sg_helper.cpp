#include "shared.h"

using namespace scene_graph::helpers;

/*
    ��⮤, ��뢠��� �� ���饭�� ������� ��ꥪ�
*/

void Helper::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);    
}
