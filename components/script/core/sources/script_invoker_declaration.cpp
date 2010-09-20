#include "shared.h"

using namespace script;

/*
    ���������� ����������� �����
*/

struct InvokerDeclaration::Impl: public xtl::reference_counter
{
};

/*
    ������������ / ���������� / ������������
*/

InvokerDeclaration::InvokerDeclaration (const Invoker& invoker)
{
  throw xtl::make_not_implemented_exception ("script::InvokerDeclaration::InvokerDeclaration");
}

InvokerDeclaration::InvokerDeclaration (const InvokerDeclaration& decl)
  : impl (decl.impl)
{
  addref (impl);
}

InvokerDeclaration::~InvokerDeclaration ()
{
  release (impl);
}

InvokerDeclaration& InvokerDeclaration::operator = (const InvokerDeclaration& decl)
{
  InvokerDeclaration (decl).Swap (*this);
  
  return *this;
}

/*
    ��� ������������� ��������
*/

const xtl::type_info& InvokerDeclaration::ResultType () const
{
  throw xtl::make_not_implemented_exception ("script::InvokerDeclaration::ResultType");
}

/*
    ���������� ����������
*/

size_t InvokerDeclaration::ParametersCount () const
{
  throw xtl::make_not_implemented_exception ("script::InvokerDeclaration::ParametersCount");
}

/*
    ��������� ���������
*/

const xtl::type_info& InvokerDeclaration::ParameterType (size_t index) const
{
  throw xtl::make_not_implemented_exception ("script::InvokerDeclaration::ParameterType");
}

/*
    ���������� ����������
*/

size_t InvokerDeclaration::OverloadsCount () const
{
  throw xtl::make_not_implemented_exception ("script::InvokerDeclaration::OverloadsCount");
}

/*
    ��������� ����������
*/

InvokerDeclaration InvokerDeclaration::Overload (size_t index) const
{
  throw xtl::make_not_implemented_exception ("script::InvokerDeclaration::Overload");
}

/*
    �����
*/

void InvokerDeclaration::Swap (InvokerDeclaration& decl)
{
  stl::swap (impl, decl.impl);
}

namespace script
{

void swap (InvokerDeclaration& decl1, InvokerDeclaration& decl2)
{
  decl1.Swap (decl2);
}

}
