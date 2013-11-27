#include "shared.h"

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

/*
    �������� ���������� ������������ ������
*/

struct VisualModel::Impl
{
  NodePtr         scissor; //������� ���������
  manager::Entity entity;  //��������

/// �����������
  Impl (manager::Entity& in_entity) : entity (in_entity) {}
};

/*
    ������������ / ���������� / ������������
*/

VisualModel::VisualModel (RenderManager& render_manager)
{
  try
  {
    manager::Entity entity = render_manager.Manager ().CreateEntity ();

    impl.reset (new Impl (entity));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::VisualModel::VisualModel");
    throw;
  }
}

VisualModel::VisualModel (manager::Entity& entity)
  : impl (new Impl (entity))
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

/*
    ������������ ��������
*/

manager::Entity& VisualModel::Entity ()
{
  return impl->entity;
}

const manager::Entity& VisualModel::Entity () const
{
  return impl->entity;
}

/*
    �����
*/

void VisualModel::VisitCore (ISceneVisitor& visitor)
{
  visitor.Visit (*this);
}

/*
    ���������
*/

void VisualModel::Draw (RenderingContext& context)
{
  try
  {
    DrawCore (context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::VisualModel::Draw(node='%s')", Name ());
    throw;
  }
}

void VisualModel::DrawCore (RenderingContext& context)
{
  //TODO: set scissor
}
