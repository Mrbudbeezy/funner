#ifndef MEDIA_PHYSICS_PHYSICS_LIBRARY_HEADER
#define MEDIA_PHYSICS_PHYSICS_LIBRARY_HEADER

#include <common/serializer_manager.h>

#include <media/physics/collection.h>
#include <media/physics/material.h>
#include <media/physics/rigid_body.h>
#include <media/physics/scene.h>
#include <media/physics/shape.h>
#include <media/physics/triangle_mesh.h>

namespace media
{

namespace physics
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class PhysicsLibrary
{
  public:
    typedef PhysicsLibraryCollection<Material>               MaterialCollection;
    typedef PhysicsLibraryCollection<RigidBody>              RigidBodyCollection;
    typedef PhysicsLibraryCollection<Scene>                  SceneCollection;
    typedef PhysicsLibraryCollection<Shape>                  ShapeCollection;
    typedef PhysicsLibraryCollection<shapes::TriangleMesh>   TriangleMeshCollection;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    PhysicsLibrary  ();
    PhysicsLibrary  (const PhysicsLibrary&);
    PhysicsLibrary  (const char* file_name);
    ~PhysicsLibrary ();

    PhysicsLibrary& operator = (const PhysicsLibrary&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name   () const;
    void        Rename (const char*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const MaterialCollection&     Materials      () const;
          MaterialCollection&     Materials      ();
    const RigidBodyCollection&    RigidBodies    () const;
          RigidBodyCollection&    RigidBodies    ();
    const SceneCollection&        Scenes         () const;
          SceneCollection&        Scenes         ();
    const ShapeCollection&        Shapes         () const;
          ShapeCollection&        Shapes         ();
    const TriangleMeshCollection& TriangleMeshes () const;
          TriangleMeshCollection& TriangleMeshes ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    struct SaveOptions
    {
      bool        separate_meshes_file;   //��������� �� ���� � ��������� �����
      const char* meshes_file_name;       //���� separate_meshes_file == true - ��� ����� ��� ���������� �����

      SaveOptions ();
    };

    void Load (const char* file_name);
    void Save (const char* file_name, const SaveOptions& options) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (PhysicsLibrary&);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (PhysicsLibrary&, PhysicsLibrary&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::ResourceSerializerManager
<
  void (const char* file_name, PhysicsLibrary& library),
  void (const char* file_name, const PhysicsLibrary::SaveOptions& options, const PhysicsLibrary& library)
> PhysicsLibraryManager;

}

}

#endif
