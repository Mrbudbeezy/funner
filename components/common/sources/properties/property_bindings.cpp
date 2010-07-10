#if 0

#include "shared.h"

using namespace common;

namespace
{

/*
    ���������
*/

const size_t DEFAULT_BINDINGS_ARRAY_RESERVE_SIZE = 8; //������������� ����� ����������

/*
    ����������
*/

const size_t UNASSIGNED_PROPERTY_INDEX = ~0u;

struct Binding: public xtl::reference_counter
{
  stl::auto_ptr<IPropertySetter> setter; //������� ��������� ��������
  stl::auto_ptr<IPropertyGetter> getter; //������� ��������� ��������
  stl::string                    name;   //��� ��������
  size_t                         index;  //������ ��������

  Binding (const char* in_name)
    : name (in_name)
    , index (UNASSIGNED_PROPERTY_INDEX)
  {
  }
};

typedef xtl::intrusive_ptr<Binding> BindingPtr;
typedef stl::vector<BindingPtr>     BindingArray;

}

/*
    �������� ���������� ���������� �������
*/

struct PropertyBindings::Impl: public xtl::reference_counter
{
  PropertyLayout layout;   //������������ �������
  BindingArray   bindings; //����������  
  
///�����������
  Impl ()
  {
    bindings.reserve (DEFAULT_BINDINGS_ARRAY_RESERVE_SIZE);
  }
};

/*
    ������������ / ���������� / ������������
*/

PropertyBindings::PropertyBindings ()
  : impl (new Impl)
{
}

PropertyBindings::PropertyBindings (const PropertyBindings& in_impl)
  : impl (in_impl)
{
  addref (impl);
}

PropertyBindings::~PropertyBindings ()
{
  release (impl);
}

PropertyBindings& PropertyBindings::operator = (const PropertyBindings& bindings)
{
  PropertyBindings (bindings).Swap (*this);

  return *this;
}

/*
    ���������� ����������
*/

size_t PropertyBindings::Size () const
{
}

/*
    ���������� �������
*/

void PropertyBindings::BindProperty (const char* name, IPropertyGetter* getter)
{
}

void PropertyBindings::BindProperty (const char* name, IPropertySetter* setter)
{
}

void PropertyBindings::UnbindProperty (const char* name)
{
}

void PropertyBindings::Clear ()
{
}

/*
    ���������� ����������
*/

void PropertyBindings::Update ()
{
}

void PropertyBindings::Commit () const
{
}

/*
    �����
*/

void PropertyBindings::Swap (PropertyBindings& bindings)
{
  stl::swap (impl, bindings.impl);
}

namespace common
{

void swap (PropertyBindings& bindings1, PropertyBindings& bindings2)
{
  bindings1.Swap (bindings2);
}

}

#endif