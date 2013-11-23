#include "shared.h"

using namespace render::scene;
using namespace render::scene::server;

/*
    �������� ���������� ������������ ������
*/

struct VisualModel::Impl
{
  NodePtr scissor; //������� ���������
};

/*
    ������������ / ���������� / ������������
*/

VisualModel::VisualModel ()
  : impl (new Impl)
{
}

VisualModel::~VisualModel ()
{
}

/*
    ������� ���������
*/

void VisualModel::SetScissor (Node* node)
{
  impl->scissor = node;
}

Node* VisualModel::Scissor () const
{
  return impl->scissor.get ();
}
