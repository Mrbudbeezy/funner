#include "shared.h"

using namespace media::adobe::xfl;

/*
    �������� ���������� �������
*/

struct Resource::Impl : public xtl::reference_counter
{
  stl::string name; //��� �������
  stl::string path; //���� � �����
};

/*
    ������������ / ���������� / ������������
*/

Resource::Resource ()
  : impl (new Impl)
  {}
  
Resource::Resource (const Resource& source)
  : impl (source.impl)
{
  addref (impl);
}

Resource::~Resource ()
{
  release (impl);
}

Resource& Resource::operator = (const Resource& source)
{
  Resource (source).Swap (*this);

  return *this;
}

/*
   ���
*/

const char* Resource::Name () const
{
  return impl->name.c_str ();
}

void Resource::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::adobe::xfl::Resource::SetName", "name");

  impl->name = name;
}

/*
   ���� � �������
*/

const char* Resource::Path () const
{
  return impl->path.c_str ();
}

void Resource::SetPath (const char* path)
{
  if (!path)
    throw xtl::make_null_argument_exception ("media::adobe::xfl::Resource::SetPath", "path");

  impl->path = path;
}

/*
   �����
*/

void Resource::Swap (Resource& resource)
{
  stl::swap (impl, resource.impl);
}

namespace media
{

namespace adobe
{

namespace xfl
{

/*
   �����
*/

void swap (Resource& resource1, Resource& resource2)
{
  resource1.Swap (resource2);
}

}

}

}
