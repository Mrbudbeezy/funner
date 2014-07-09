#include "shared.h"

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

/*
    �������� ���������� ������������ ������
*/

typedef xtl::intrusive_ptr<Scissor> ScissorPtr;

struct VisualModel::Impl
{
  ScissorPtr      scissor;              //������� ���������
  manager::Entity entity;               //��������
  bool            need_update_world_tm; //���� ������������� ���������� ������� �������

/// �����������
  Impl (manager::Entity& in_entity)
    : entity (in_entity)
    , need_update_world_tm (true)
  {
  }
};

/*
    ������������ / ���������� / ������������
*/

VisualModel::VisualModel (RenderManager& render_manager)
{
  try
  {
    manager::Entity entity = render_manager.Manager ().CreateEntity ();

    entity.SetUserData (this);

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
  entity.SetUserData (this);
}

VisualModel::~VisualModel ()
{
}

/*
    ������� ���������
*/

void VisualModel::SetScissor (render::scene::server::Scissor* node)
{
  impl->scissor = node;

  if (node)
    impl->entity.SetWorldScissor (node->Area ());
}

Scissor* VisualModel::Scissor () const
{
  return impl->scissor.get ();
}

/*
    �������� �������
*/

void VisualModel::SetDynamicShaderProperties (const common::PropertyMap& properties)
{
  impl->entity.SetProperties (properties);
}

void VisualModel::SetStaticShaderProperties (const common::PropertyMap& properties)
{
  impl->entity.SetShaderOptions (properties);
}

const common::PropertyMap& VisualModel::DynamicShaderProperties () const
{
  return impl->entity.Properties ();
}

const common::PropertyMap& VisualModel::StaticShaderProperties () const
{
  return impl->entity.ShaderOptions ();
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
    ���������� �� ���������� ������� �������
*/

void VisualModel::OnWorldMatrixUpdated ()
{
  impl->need_update_world_tm = true;
}

/*
    ���������
*/

void VisualModel::Draw (RenderingContext& context, void* user_data)
{
  try
  {    
    DrawCore (context, user_data);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::VisualModel::Draw(node='%s')", Name ());
    throw;
  }
}

void VisualModel::DrawCore (RenderingContext& context, void* user_data)
{
  if (impl->scissor)
  {
    bool visible = impl->scissor->IsVisible ();

    if (impl->entity.ScissorState () != visible)
      impl->entity.SetScissorState (visible);
  }

  if (impl->need_update_world_tm)
  {
    impl->entity.SetWorldMatrix (WorldMatrix ());

    impl->need_update_world_tm = false;
  }

  context.Frame ().AddEntity (impl->entity, user_data);
}
