#ifndef MEDIALIB_PHYSICS_TRIANGLE_MESH_HEADER
#define MEDIALIB_PHYSICS_TRIANGLE_MESH_HEADER

#include <math/vector.h>

namespace media
{

namespace physics
{

namespace shapes
{

typedef size_t TriangleIndices [3];

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���� � ���� ��������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
class TriangleMesh
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������, ����������, ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    TriangleMesh  ();
    TriangleMesh  (const TriangleMesh&);
    ~TriangleMesh ();

    TriangleMesh& operator = (const TriangleMesh&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    TriangleMesh Clone () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Id () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name   () const;
    void        Rename (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� �������� ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsConvex  () const;
    void SetConvex (bool convex);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t VerticesCount    () const;
    void   SetVerticesCount (size_t count);

    const math::vec3f* Vertices () const;
          math::vec3f* Vertices ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t TrianglesCount    () const;
    void   SetTrianglesCount (size_t count);

    const TriangleIndices* Triangles () const;
          TriangleIndices* Triangles ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (TriangleMesh&);
    
  private:
    struct Impl;
    
    TriangleMesh (Impl*);

  private:
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (TriangleMesh&, TriangleMesh&);

}

}

}

#endif