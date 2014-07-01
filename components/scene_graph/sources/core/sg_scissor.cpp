#include "shared.h"

using namespace scene_graph;

/*
    ����������� / ����������
*/

Scissor::Scissor ()
{
  SetBoundBox (bound_volumes::axis_aligned_box <float> (-0.5f, -0.5f, 0, 0.5f, 0.5f, 0));
}

Scissor::~Scissor ()
{
}

/*
    �������� 
*/

Scissor::Pointer Scissor::Create ()
{
  return Pointer (new Scissor, false);
}

/*
    �����, ���������� ��� ��������� �������
*/

void Scissor::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
