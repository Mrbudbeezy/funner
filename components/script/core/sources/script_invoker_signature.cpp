#include "shared.h"

using namespace script;

namespace
{

/*
    ���������
*/

const size_t DEFAULT_PARAMETERS_COUNT = 4;   //���������� ���������� �� ���������
const size_t TO_STRING_RESERVE_SIZE   = 256; //���������� ��������, ������������� � ������ �����

}

/*
    ���������� ����������� �����
*/

typedef stl::vector<const xtl::type_info*> InfoArray;

struct InvokerSignature::Impl: public xtl::reference_counter
{
  const xtl::type_info*      result_type;        //��� ����������
  InfoArray                  parameter_types;    //���� ����������  
  stl::auto_ptr<stl::string> string;             //������ � ������� ���������
  bool                       need_update_string; //����� �� ��������� ������ � ������� ���������
  
  Impl ()
    : result_type (&get_type (xtl::get_type<void> ()))
    , need_update_string (true)
  {
    parameter_types.reserve (DEFAULT_PARAMETERS_COUNT);
  }
};

/*
    ������������ / ���������� / ������������
*/

InvokerSignature::InvokerSignature ()
  : impl (new Impl)
{
}

InvokerSignature::InvokerSignature (const InvokerSignature& signature)
  : impl (signature.impl)
{
  addref (impl);
}

InvokerSignature::~InvokerSignature ()
{
  release (impl);
}

InvokerSignature& InvokerSignature::operator = (const InvokerSignature& signature)
{
  InvokerSignature (signature).Swap (*this);
  
  return *this;
}

/*
    ��� ������������� ��������
*/

const xtl::type_info& InvokerSignature::ResultType () const
{
  return *impl->result_type;
}

void InvokerSignature::SetResultType (const xtl::type_info& type)
{
  impl->result_type        = &type;
  impl->need_update_string = true;
}

/*
    ���������� ����������
*/

size_t InvokerSignature::ParametersCount () const
{
  return impl->parameter_types.size ();
}

/*
    ���� ����������
*/

const xtl::type_info& InvokerSignature::ParameterType (size_t index) const
{
  if (index >= impl->parameter_types.size ())
    throw xtl::make_range_exception ("script::InvokerSignature::ParameterType", "index", index, impl->parameter_types.size ());
    
  return *impl->parameter_types [index];
}

void InvokerSignature::SetParameterType (size_t index, const xtl::type_info& type)
{
  if (index >= impl->parameter_types.size ())
    throw xtl::make_range_exception ("script::InvokerSignature::ParameterType", "index", index, impl->parameter_types.size ());
    
  impl->need_update_string = true;
    
  impl->parameter_types [index] = &type;
}

/*
    ���������� / �������� ����� ����������
*/

size_t InvokerSignature::AddParameterType (const xtl::type_info& type)
{
  impl->parameter_types.push_back (&type);
 
  impl->need_update_string = true;
  
  return impl->parameter_types.size () - 1;
}

void InvokerSignature::RemoveParameterType (size_t index)
{
  if (index >= impl->parameter_types.size ())
    return;
    
  impl->need_update_string = true;

  impl->parameter_types.erase (impl->parameter_types.begin () + index);
}

void InvokerSignature::RemoveAllParameterTypes ()
{
  impl->parameter_types.clear ();

  impl->need_update_string = true;
}

/*
    ���������� � ������
*/

namespace
{

stl::string get_type_string (const xtl::type_info& t)
{
  if (t.is_const ())
    return stl::string ("const ") + get_type_string (t.remove_const ());
    
  if (t.is_volatile ())
    return stl::string ("volatile ") + get_type_string (t.remove_volatile ());

  if (t.is_pointer ())
    return get_type_string (t.remove_pointer ()) + stl::string ("*");

  if (t.is_reference ())
    return get_type_string (t.remove_reference ()) + stl::string ("&");
    
  if (&t == &xtl::get_type<unsigned char> ())  return "uchar";
  if (&t == &xtl::get_type<char> ())           return "char";
  if (&t == &xtl::get_type<unsigned short> ()) return "ushort";
  if (&t == &xtl::get_type<signed short> ())   return "short";
  if (&t == &xtl::get_type<unsigned int> ())   return "uint";
  if (&t == &xtl::get_type<signed int> ())     return "int";
  if (&t == &xtl::get_type<unsigned long> ())  return "ulong";
  if (&t == &xtl::get_type<signed long> ())    return "long";
  if (&t == &xtl::get_type<float> ())          return "float";  
  if (&t == &xtl::get_type<double> ())         return "double";
  if (&t == &xtl::get_type<long double> ())    return "long double";
  if (&t == &xtl::get_type<wchar_t> ())        return "wchar_t";
  
  return t.std_type ().name ();
}

}

const char* InvokerSignature::ToString () const
{
  if (!impl->need_update_string)
    return impl->string->c_str ();
    
  stl::string result;
  
  result.reserve (TO_STRING_RESERVE_SIZE);
  
  result += get_type_string (ResultType ());
  result += " (";
  
  for (InfoArray::const_iterator i=impl->parameter_types.begin (), end=impl->parameter_types.end (); i!=end; ++i)
  {
    if (i != impl->parameter_types.begin ())
      result += ", ";
      
    result += get_type_string (**i);
  }
  
  result += ")";
  
  if (!impl->string) impl->string  = new stl::string (result);
  else               *impl->string = result;

  impl->need_update_string = false;
  
  return impl->string->c_str ();
}

/*
    ���������
*/

bool InvokerSignature::operator == (const InvokerSignature& signature) const
{
  return impl->result_type == signature.impl->result_type && impl->parameter_types == signature.impl->parameter_types;
}

bool InvokerSignature::operator != (const InvokerSignature& signature) const
{
  return !(*this == signature);
}

/*
    �����
*/

void InvokerSignature::Swap (InvokerSignature& signature)
{
  stl::swap (impl, signature.impl);
}

namespace script
{

void swap (InvokerSignature& signature1, InvokerSignature& signature2)
{
  signature1.Swap (signature2);
}

}
