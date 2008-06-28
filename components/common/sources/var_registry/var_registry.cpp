#include "shared.h"

using namespace common;
using namespace xtl;

/*
   ������������ / �����������
*/

VarRegistry::VarRegistry ()
  : impl (VarRegistryImpl::GetNullBranch ())
  {}

VarRegistry::VarRegistry (const char* branch_name)
{
  if (!branch_name)
    throw make_null_argument_exception ("common::VarRegistry::VarRegistry", "branch_name");

  impl = VarRegistryImpl::CreateBranch (branch_name);
}

VarRegistry::VarRegistry (const VarRegistry& source)
  : impl (source.impl)
{
  addref (impl);
}

VarRegistry::~VarRegistry ()
{
  release (impl);
}
    
/*
   �����������
*/

VarRegistry& VarRegistry::operator = (const VarRegistry& source)
{
  VarRegistry (source).Swap (*this);
  
  return *this;
}

/*
   ���������/��������� ������
*/

const char* VarRegistry::GetValue (const char* var_name) const
{
  if (!var_name)
    throw make_null_argument_exception ("common::VarRegistry::GetValue", "var_name");

  return impl->GetValue (var_name);
}

void VarRegistry::SetValue (const char* var_name, const char* value)
{
  static const char* METHOD_NAME = "common::VarRegistry::SetValue";

  if (!var_name)
    throw make_null_argument_exception (METHOD_NAME, "var_name");

  if (!value)
    throw make_null_argument_exception (METHOD_NAME, "value");

  impl->SetValue (var_name, value);
}

/*
   �������� ������� ����������
*/

bool VarRegistry::HasVariable (const char* var_name) const
{
  return var_name && impl->HasVariable (var_name);
}

/*
   ��������� �����
*/

const char* VarRegistry::BranchName () const
{
  return impl->BranchName ();
}           

/*
   �������� ���������� �� ������
*/

bool VarRegistry::IsOpened () const
{
    //!!!����� �������� ���������� � GetNullBranch!!!
  return impl != VarRegistryImpl::GetNullBranch ();
}

/*
   �����������/���������� �������
*/

void VarRegistry::Open (const char* branch_name)
{
  VarRegistry (branch_name).Swap (*this);
}

void VarRegistry::Close ()
{
  VarRegistry ().Swap (*this);
}

/*
   ����� ���� ���������� / ���������� �� �����
*/

void VarRegistry::EnumerateVars (const EnumHandler& handler)
{
  impl->EnumerateVars (handler);
}

void VarRegistry::EnumerateVars (const char* var_name_mask, const EnumHandler& handler)
{
  if (!var_name_mask)
    throw make_null_argument_exception ("common::VarRegistry::EnumerateVars", "var_name_mask");

  impl->EnumerateVars (var_name_mask, handler);
}

/*
   �������� �� ����������/���������/�������� ����������
*/

xtl::connection VarRegistry::RegisterEventHandler (const char* var_name_mask, VarRegistryEvent event, const EventHandler& handler) const
{
  if (!var_name_mask)
    throw make_null_argument_exception ("common::VarRegistry::RegisterEventHandler", "var_name_mask");    

  return impl->RegisterEventHandler (var_name_mask, event, handler);
}

/*
   �����
*/

void VarRegistry::Swap (VarRegistry& source)
{
  stl::swap (impl, source.impl);
}

namespace common
{

/*
   �����
*/

void swap (VarRegistry& source1, VarRegistry& source2)
{
  source1.Swap (source2);
}

}
