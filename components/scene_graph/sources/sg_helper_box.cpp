#include "shared.h"

using namespace scene_graph::helpers;
using namespace bound_volumes;

/*
    ���������
*/

Box::Box ()
{
    //�� 㬮�砭�� ᮧ������ ��ࠫ���������� � ��ࠬ� �����筮� �����
    
  SetBoundBox (aaboxf (-1.0f, 1.0f));
}

/*
    �������� ()
*/

Box::Pointer Box::Create ()
{
  return Pointer (new Box, false);
}

/*
    ��⮤, ��뢠��� �� ���饭�� ������� ��ꥪ�
*/

void Box::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Helper::AcceptCore (visitor);
}
