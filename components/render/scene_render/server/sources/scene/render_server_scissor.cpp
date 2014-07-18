#include "shared.h"

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

/*
    �������� ���������� ������� ���������
*/

struct Scissor::Impl
{
  manager::BoxArea area; //������� ���������
};

/*
    ����������� / ����������
*/

Scissor::Scissor ()
  : impl (new Impl)
{
}

Scissor::~Scissor ()
{
}

/*
    �������
*/

const manager::BoxArea& Scissor::Area ()
{
  return impl->area;
}

/*
    �����
*/

void Scissor::VisitCore (ISceneVisitor& visitor)
{
  visitor.Visit (*this);
}

/*
    ���������� �� ���������� ��������������� ����
*/

void Scissor::OnUpdateBounds ()
{
  const bound_volumes::aaboxf& box = BoundBox ();

  impl->area.SetBox (box.minimum (), box.maximum ());
}
