#include "shared.h"

using namespace media::collada;
using namespace media;
using namespace common;

/*
    ���������� ��������
*/

struct Image::Impl: public xtl::reference_counter
{
  stl::string id;   //������������� ��������
  stl::string path; //���� � ����� ��������
};

/*
    ������������ / ���������� / ������������
*/

Image::Image ()
  : impl (new Impl)
    {}

Image::Image (const Image& source)
  : impl (source.impl)
{
  addref (impl);
}

Image::~Image ()
{
  release (impl);
}

Image& Image::operator = (const Image& source)
{
  if (this == &source)
    return *this;

  release (impl);
  impl = source.impl;
  addref (impl);
  
  return *this;
}

/*
    �������� �����
*/

Image Image::Clone () const
{
  Image return_value;

  return_value.SetId (Id ());
  return_value.SetPath (Path ());

  return return_value;
}

/*
    ������������� ��������
*/

const char* Image::Id () const
{
  return impl->id.c_str ();
}

void Image::SetId (const char* id)
{
  if (!id)
    throw xtl::make_null_argument_exception ("media::collada::Image::SetId", "id");
    
  impl->id = id;
}

/*
    ���� � ��������
*/

const char* Image::Path () const
{
  return impl->path.c_str ();
}

void Image::SetPath (const char* path)
{
  if (!path)
    throw xtl::make_null_argument_exception ("media::collada::Image::SetPath", "path");

  impl->path = path;
}
