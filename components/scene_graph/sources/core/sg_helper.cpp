#include "shared.h"

using namespace scene_graph::helpers;

/*
    �����, ���������� ��� ��������� ������� �������
*/

void Helper::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    VisualModel::AcceptCore (visitor);    
}
