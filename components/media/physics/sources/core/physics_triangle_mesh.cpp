#include "shared.h"

using namespace media::physics::shapes;

/*
   ���������� �����
*/

typedef xtl::uninitialized_storage<math::vec3f>     VerticesArray;
typedef xtl::uninitialized_storage<TriangleIndices> TrianglesArray;

struct TriangleMesh::Impl : public xtl::reference_counter
{
  stl::string    name;      //��� �����
  bool           is_convex; //�������� �� �������� ��������������
  VerticesArray  vertices;  //�������
  TrianglesArray triangles; //������� �������������

  Impl ()
    : is_convex (true)
    {}
};

/*
   ������������ / ���������� / ������������
*/

TriangleMesh::TriangleMesh ()
  : impl (new Impl)
{
}

TriangleMesh::TriangleMesh (const TriangleMesh& source)
  : impl (source.impl)
{
  addref (impl);
}

TriangleMesh::TriangleMesh (Impl* in_impl)
  : impl (in_impl)
{
}

TriangleMesh::~TriangleMesh ()
{
  release (impl);
}

TriangleMesh& TriangleMesh::operator = (const TriangleMesh& source)
{
  TriangleMesh (source).Swap (*this);

  return *this;
}
    
/*
   �������� �����
*/

TriangleMesh TriangleMesh::Clone () const
{
  return TriangleMesh (new Impl (*impl));
}

/*
   �������������
*/

size_t TriangleMesh::Id () const
{
  return (size_t)impl;
}

/*
   ��� �����
*/

const char* TriangleMesh::Name () const
{
  return impl->name.c_str ();
}

void TriangleMesh::Rename (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::physics::TriangleMesh::Rename", "name");

  impl->name = name;
}

/*
   �������� �� �������� ��������������
*/

bool TriangleMesh::IsConvex () const
{
  return impl->is_convex;
}

void TriangleMesh::SetConvex (bool convex)
{
  impl->is_convex = convex;
}

/*
   ������ � ���������
*/

unsigned int TriangleMesh::VerticesCount () const
{
  return (unsigned int)impl->vertices.size ();
}

void TriangleMesh::SetVerticesCount (unsigned int count)
{
  impl->vertices.resize (count);
}

const math::vec3f* TriangleMesh::Vertices () const
{
  return const_cast<TriangleMesh&> (*this).Vertices ();
}

math::vec3f* TriangleMesh::Vertices ()
{
  return impl->vertices.data ();
}

/*
   ������ � ���������
*/

unsigned int TriangleMesh::TrianglesCount () const
{
  return (unsigned int)impl->triangles.size ();
}

void TriangleMesh::SetTrianglesCount (unsigned int count)
{
  impl->triangles.resize (count);
}

const TriangleIndices* TriangleMesh::Triangles () const
{
  return const_cast<TriangleMesh&> (*this).Triangles ();
}

TriangleIndices* TriangleMesh::Triangles ()
{
  return impl->triangles.data ();
}

/*
   �����
*/

void TriangleMesh::Swap (TriangleMesh& source)
{
  stl::swap (impl, source.impl);
}

namespace media
{

namespace physics
{

/*
   �����
*/

void swap (TriangleMesh& mesh1, TriangleMesh& mesh2)
{
  mesh1.Swap (mesh2);
}

}

}
