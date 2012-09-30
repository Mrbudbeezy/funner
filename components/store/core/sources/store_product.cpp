#include "shared.h"

using namespace store;

///���������� ��������
struct Product::Impl : public xtl::reference_counter
{
  stl::string           description;             //��������
  stl::string           id;                      //�������������
  common::PropertyMap   properties;              //������ ��������
  const void*           handle;                  //�������������� ����������
  ReleaseHandleFunction handle_release_function; //�������, ���������� ��� ������������ �����������

  Impl ()
    : handle (0)
    {}

  ~Impl ()
  {
    if (handle_release_function)
      handle_release_function (handle);
  }
};

/*
   ����������� / ���������� / �����������
*/

Product::Product ()
  : impl (new Impl)
  {}

Product::Product (const Product& source)
  : impl (source.impl)
{
  addref (impl);
}

Product::~Product ()
{
  release (impl);
}

Product& Product::operator = (const Product& source)
{
  Product (source).Swap (*this);

  return *this;
}

/*
   ��������
*/

const char* Product::Description () const
{
  return impl->description.c_str ();
}

void Product::SetDescription (const char* description)
{
  if (!description)
    throw xtl::make_null_argument_exception ("store::Product::SetDescription", "description");

  impl->description = description;
}

/*
   �������������
*/

const char* Product::Id () const
{
  return impl->id.c_str ();
}

void Product::SetId (const char* id) const
{
  if (!id)
    throw xtl::make_null_argument_exception ("store::Product::SetId", "id");

  impl->id = id;
}

/*
   ������ ��������
*/

const common::PropertyMap& Product::Properties () const
{
  return impl->properties;
}

common::PropertyMap& Product::Properties ()
{
  return impl->properties;
}

void Product::SetProperties (const common::PropertyMap& properties)
{
  impl->properties = properties;
}

/*
   �������������� �����������
*/

const void* Product::Handle () const
{
  return impl->handle;
}

void Product::SetHandle (const void* handle, const ReleaseHandleFunction& release_function)
{
  if (impl->handle_release_function)
    impl->handle_release_function (impl->handle);

  impl->handle = handle;
  impl->handle_release_function = release_function;
}

/*
   �����
*/

void Product::Swap (Product& source)
{
  stl::swap (source.impl, impl);
}

namespace store
{

/*
   �����
*/

void swap (Product& product1, Product& product2)
{
  product1.Swap (product2);
}

}
