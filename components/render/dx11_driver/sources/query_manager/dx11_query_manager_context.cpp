#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
===================================================================================================
    QueryManagerContextState
===================================================================================================
*/

/*
    �������� ���������� ��������� ��������� ��������� ��������
*/

struct QueryManagerContextState::Impl: public DeviceObject
{
  bool is_dirty; //���� "���������"

/// �����������
  Impl (const DeviceManager& manager)
    : DeviceObject (manager)
    , is_dirty (true)
  {
  }

/// ����������
  virtual ~Impl ()
  {
  }
};

/*
    ����������� / ����������
*/

QueryManagerContextState::QueryManagerContextState (const DeviceManager& manager)
  : impl (new Impl (manager))
{
}

QueryManagerContextState::QueryManagerContextState (Impl* in_impl)
  : impl (in_impl)
{
}

QueryManagerContextState::~QueryManagerContextState ()
{
}

/*
    ����������
*/

QueryManagerContextState::Impl& QueryManagerContextState::GetImpl () const
{
  return *impl;
}

/*
    ���������� ����������� ���������
*/

void QueryManagerContextState::SetPredication (IPredicate* predicate, bool predicate_value)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::QueryManagerContextState::SetPredication");
    throw;
  }
}

IPredicate* QueryManagerContextState::GetPredicate ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

bool QueryManagerContextState::GetPredicateValue ()
{  
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ����������� ���������
*/

void QueryManagerContextState::CopyTo (const StateBlockMask& mask, QueryManagerContextState& dst_state) const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
===================================================================================================
    QueryManagerContextState
===================================================================================================
*/

/*
    �������� ���������� ��������� ��������� ��������
*/

struct QueryManagerContext::Impl: public QueryManagerContextState::Impl
{
  DxContextPtr context; //�������� ���������

/// �����������
  Impl (const DeviceManager& manager, const DxContextPtr& in_context)
    : QueryManagerContextState::Impl (manager)
    , context (in_context)
  {
    if (!context)
      throw xtl::make_null_argument_exception ("render::low_level::dx11::QueryManagerContext::Impl::Impl", "context");
  }
};

/*
    ����������� / ����������
*/

QueryManagerContext::QueryManagerContext (const DeviceManager& manager, const DxContextPtr& context)
  : QueryManagerContextState (new Impl (manager, context))
{
}

QueryManagerContext::~QueryManagerContext ()
{
}

/*
    ����������
*/

QueryManagerContext::Impl& QueryManagerContext::GetImpl () const
{
  return static_cast<Impl&> (QueryManagerContextState::GetImpl ());
}

/*
    �������� ������ �������
*/

void QueryManagerContext::Begin (IQuery* async)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::QueryManagerContext::Begin");
    throw;
  }
}

void QueryManagerContext::End (IQuery* async)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::QueryManagerContext::End");
    throw;
  }
}

/*
    ��������� ��������� ������ � ��������
*/

void QueryManagerContext::Bind ()
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::QueryManagerContext::Bind");
    throw;
  }
}
