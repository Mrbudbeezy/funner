#ifndef MEDIALIB_GEOMETRY_SHARED_HEADER
#define MEDIALIB_GEOMETRY_SHARED_HEADER

#include <common/exception.h>
#include <media/geometry/mesh.h>

namespace medialib
{

namespace geometry
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class MeshBuffer
{
  public:
    MeshBuffer  ();
    MeshBuffer  (size_t size);
    MeshBuffer  (const MeshBuffer&);
    ~MeshBuffer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������ / ����� ������ (���������� ��������� ������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size     () const { return finish - start; }
    size_t Capacity () const { return end_of_storage - start; }
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void* Data () { return start; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ / �������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Resize  (size_t new_size);
    void Reserve (size_t new_size);
    
  private:
    MeshBuffer& operator = (const MeshBuffer&); //no impl
  
  private:      
    char *start, *finish, *end_of_storage;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct InstanceResource
{
  size_t          ref_count;          //������� ������
  BufferCloneMode default_clone_mode; //����� ��������������� �� ���������
  
  InstanceResource () : ref_count (1), default_clone_mode (BufferCloneMode_Copy) {}
  InstanceResource (const InstanceResource& resource) : ref_count (1), default_clone_mode (resource.default_clone_mode) {}

  private: InstanceResource& operator = (const InstanceResource&); //no impl
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> T* clone_resource (T* resource, BufferCloneMode mode, const char* source)
{
  T* new_resource = 0;

  switch (mode)
  {
    case BufferCloneMode_Copy:
      new_resource = new T (*resource);
      break;
    case BufferCloneMode_Instance:
      new_resource = resource;
      resource->ref_count++;
      break;
    case BufferCloneMode_Default:
      new_resource = clone_resource (resource, resource->default_clone_mode, source);
      break;
    default:
      common::RaiseInvalidArgument (source, "mode", mode);
      break;
  }

  new_resource->default_clone_mode = mode;
  
  return new_resource;
}

template <class T> T* clone_resource (T* resource, const char* source)
{
  return clone_resource (source, resource, resource->default_clone_mode);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> void release_resource (T* resource)
{
  if (!--resource->ref_count)
    delete resource;
}

}

}

#endif
