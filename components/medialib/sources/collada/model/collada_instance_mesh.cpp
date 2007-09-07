#include <media/collada/scene.h>

using namespace media::collada;

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

InstanceMesh::InstanceMesh (media::collada::Mesh& mesh)
  : impl (new Impl (mesh))
  {}
  
InstanceMesh::~InstanceMesh ()
{
  delete impl;
}

/*
    ���
*/

media::collada::Mesh& InstanceMesh::Mesh ()
{
  return impl->mesh;
}

const media::collada::Mesh& InstanceMesh::Mesh () const
{
  return impl->mesh;
}

/*
    ������������� ���������
*/

media::collada::MaterialBinds& InstanceMesh::MaterialBinds ()
{
  return impl->binds;
}

const media::collada::MaterialBinds& InstanceMesh::MaterialBinds () const
{
  return impl->binds;
}
