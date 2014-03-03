#include "shared.h"

using namespace render::scene;
using namespace render::scene::client;

/*
    �������� ���������� ��������
*/

namespace
{

struct ShaderPropertiesSync
{
  PropertyMapSynchronizer synchronizer;
  size_t                  cached_id;

  ShaderPropertiesSync ()
    : cached_id ()
  {
  }

  bool Sync (const common::PropertyMap* properties, SceneManager& manager)
  {
    size_t id = properties ? properties->Id () : 0;

    if (id == cached_id)
      return false;

    ClientImpl& client = manager.Client ();

    if (properties)
    {
      synchronizer = client.CreateSynchronizer (*properties);
    }
    else
    {
      synchronizer = PropertyMapSynchronizer ();
    }

    client.Synchronize ();

    cached_id = id;

    return true;
  }
};

}

struct VisualModel::Impl
{
  scene_graph::Scissor* scissor;                   //����������� ������� ���������
  ShaderPropertiesSync  dynamic_shader_properties; //������������ �������� �������
  ShaderPropertiesSync  static_shader_properties;  //����������� �������� �������

  Impl () : scissor () {}
};

/*
    ����������� / ����������
*/

VisualModel::VisualModel (scene_graph::VisualModel& entity, SceneManager& manager, interchange::NodeType node_type)
  : Entity (entity, manager, node_type)
  , impl (new Impl)
{
}

VisualModel::~VisualModel ()
{
}

/*
    ���������� �������������
*/

void VisualModel::UpdateCore (client::Context& context)
{
  try
  {
    Entity::UpdateCore (context);

    scene_graph::VisualModel& model = SourceNode ();

      //������������� ������� ���������

    scene_graph::Scissor* scissor = model.Scissor ();

    if (scissor != impl->scissor)
    {
      if (scissor)
      {
        NodePtr scissor_node = Scenes ().FindNode (*scissor);

        context.SetVisualModelScissor (Id (), scissor_node ? scissor_node->Id () : 0u);
      }
      else context.SetVisualModelScissor (Id (), 0u);

      impl->scissor = scissor;
    }    

      //������������� ������� �������

    if (impl->dynamic_shader_properties.Sync (model.DynamicShaderProperties (), Scenes ()))
      context.SetVisualModelDynamicShaderProperties (Id (), impl->dynamic_shader_properties.cached_id);

    if (impl->static_shader_properties.Sync (model.StaticShaderProperties (), Scenes ()))
      context.SetVisualModelStaticShaderProperties (Id (), impl->static_shader_properties.cached_id);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::VisualModel::UpdateCore");
    throw;
  }
}
