#include <media/collada/geometry.h>

#include "collection.h"

using namespace medialib::collada;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

/*
    ���������� Mesh
*/

namespace
{

//�������������� Surface
class ConstructableSurface: public Surface
{
  public:
    ConstructableSurface (medialib::collada::PrimitiveType type, size_t verts_count, size_t indices_count)
      : Surface (type, verts_count, indices_count) {}
};

//������ ������������
class SurfaceListImpl: public Collection<Surface, ConstructableSurface, true>
{
  public:
    SurfaceListImpl (Entity& owner) : Collection<Surface, ConstructableSurface, true> (owner) {}
  
    Surface& Create (PrimitiveType type, size_t verts_count, size_t indices_count)
    {
      ConstructableSurface* surface = new ConstructableSurface (type, verts_count, indices_count);
      
      try
      {
        InsertCore (*surface);

        return *surface;
      }
      catch (...)
      {
        delete surface;
        throw;
      }
    }
};

}

//���������� Mesh
struct Mesh::Impl
{
  SurfaceListImpl surfaces; //����������� ����  
  
  Impl (Entity& owner) : surfaces (owner) {}
};

/*
    �����������
*/

Mesh::Mesh (ModelImpl* owner, const char* id)
  : Entity (owner, id), impl (new Impl (*this))
  {}

/*
    ������ ������������
*/

Mesh::SurfaceList& Mesh::Surfaces ()
{
  return impl->surfaces;
}

const Mesh::SurfaceList& Mesh::Surfaces () const
{
  return impl->surfaces;
}
