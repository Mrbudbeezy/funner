#include "shared.h"

using namespace render::scene;
using namespace render::scene::client;

/*
    �������� ���������� ��������
*/

struct StaticMesh::Impl
{
  size_t mesh_name_hash; //��� ����� ����

  Impl () : mesh_name_hash () {}
};

/*
    ����������� / ����������
*/

StaticMesh::StaticMesh (scene_graph::StaticMesh& entity, SceneManager& manager, interchange::NodeType node_type)
  : VisualModel (entity, manager, node_type)
  , impl (new Impl)
{
}

StaticMesh::~StaticMesh ()
{
}

/*
    ���������� �������������
*/

void StaticMesh::UpdateCore (client::Context& context)
{
  try
  {
    VisualModel::UpdateCore (context);

    scene_graph::StaticMesh& mesh = SourceNode ();

      //������������� ����� ����

    size_t mesh_name_hash = mesh.MeshNameHash ();

    if (mesh_name_hash != impl->mesh_name_hash)
    {
      context.SetStaticMeshName (Id (), mesh.MeshName ());

      impl->mesh_name_hash = mesh_name_hash;
    }    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::StaticMesh::UpdateCore");
    throw;
  }
}
