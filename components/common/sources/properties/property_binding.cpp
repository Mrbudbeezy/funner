#include "shared.h"

using namespace common;

/*
    �������� ���������� ���������� ��������
*/

struct PropertyBinding::Impl: public xtl::reference_counter
{
  stl::auto_ptr<detail::IPropertySetter> setter; //������� ���������
  stl::auto_ptr<detail::IPropertyGetter> getter; //������� ������
};

/*
    ������������ / ���������� / ������������
*/

PropertyBinding::PropertyBinding ()
  : impl (new Impl)
{
}

PropertyBinding::PropertyBinding (const PropertyBinding& binding)
  : impl (binding.impl)
{
  addref (impl);
}

PropertyBinding::~PropertyBinding ()
{
  release (impl);
}

PropertyBinding& PropertyBinding::operator = (const PropertyBinding& binding)
{
  PropertyBinding (binding).Swap (*this);

  return *this;
}

/*
    ����� setter / getter ��� ��������
*/

void PropertyBinding::ResetSetter ()
{
  impl->setter.reset ();
}

void PropertyBinding::ResetGetter ()
{
  impl->getter.reset ();
}

/*
    ��������� setter / getter ��� ��������
*/

void PropertyBinding::SetSetterCore (detail::IPropertySetter* setter)
{
  impl->setter.reset (setter);
}

void PropertyBinding::SetGetterCore (detail::IPropertyGetter* getter)
{
  impl->getter.reset (getter);
}

detail::IPropertyGetter* PropertyBinding::GetterCore () const
{
  return impl->getter.get ();
}

detail::IPropertySetter* PropertyBinding::SetterCore () const
{
  return impl->setter.get ();
}

/*
    �������� ������� setter/getter
*/

bool PropertyBinding::HasSetter () const
{
  return impl->setter;
}

bool PropertyBinding::HasGetter () const
{
  return impl->getter;
}

/*
    ������������� � ������ �������
*/

void PropertyBinding::CopyFromPropertyMap (const PropertyMap& map, size_t property_index)
{
  if (!impl->setter)
    return;
    
  try
  {    
    impl->setter->CopyFrom (map, property_index);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyBinding::CopyFromPropertyMap");
    throw;
  }
}

void PropertyBinding::CopyToPropertyMap (PropertyMap& map, size_t property_index) const
{
  if (!impl->getter)
    return;
    
  try
  {    
    impl->getter->CopyTo (map, property_index);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyBinding::CopyToPropertyMap");
    throw;
  }
}

/*
    �����
*/

void PropertyBinding::Swap (PropertyBinding& binding)
{
  stl::swap (impl, binding.impl);
}

namespace common
{

void swap (PropertyBinding& binding1, PropertyBinding& binding2)
{
  binding1.Swap (binding2);
}

}
