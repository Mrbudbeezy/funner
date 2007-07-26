#include <media/collada/scene.h>

using namespace medialib::collada;

/*
    �������� ���������� InstanceMesh
*/

class ConstructableMaterialBinds: public MaterialBinds
{
  public:
    ConstructableMaterialBinds (Entity& entity) : MaterialBinds (entity) {}
    ~ConstructableMaterialBinds () {}
};

struct InstanceMesh::Impl
{
  collada::Mesh&             mesh;  //���
  ConstructableMaterialBinds binds; //������������� ���������
  
  Impl (collada::Mesh& in_mesh) : mesh (in_mesh), binds (in_mesh) {}
};

/*
    ����������� / ����������
*/

InstanceMesh::InstanceMesh (medialib::collada::Mesh& mesh)
  : impl (new Impl (mesh))
  {}
  
InstanceMesh::~InstanceMesh ()
{
  delete impl;
}

/*
    ���
*/

medialib::collada::Mesh& InstanceMesh::Mesh ()
{
  return impl->mesh;
}

const medialib::collada::Mesh& InstanceMesh::Mesh () const
{
  return impl->mesh;
}

/*
    ������������� ���������
*/

medialib::collada::MaterialBinds& InstanceMesh::MaterialBinds ()
{
  return impl->binds;
}

const medialib::collada::MaterialBinds& InstanceMesh::MaterialBinds () const
{
  return impl->binds;
}
