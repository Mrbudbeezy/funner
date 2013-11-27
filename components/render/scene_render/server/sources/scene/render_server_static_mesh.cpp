#include "shared.h"

using namespace render::scene;
using namespace render::scene::server;

/*
    �������� ���������� ������������ ������
*/

struct StaticMesh::Impl
{
  stl::string      mesh_name; //��� ����
  manager::Entity& entity;    //��������

/// �����������
  Impl (manager::Entity& in_entity) : entity (in_entity) {}
};

/*
    ������������ / ���������� / ������������
*/

StaticMesh::StaticMesh (RenderManager& render_manager)
  : VisualModel (render_manager)
  , impl (new Impl (Entity ()))
{
}

StaticMesh::~StaticMesh ()
{
}

/*
    ��� ����
*/

void StaticMesh::SetMeshName (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    impl->mesh_name = name;

    impl->entity.SetPrimitive (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::StaticMesh::SetMeshName");
    throw;
  }
}

const char* StaticMesh::MeshName () const
{
  return impl->mesh_name.c_str ();
}
