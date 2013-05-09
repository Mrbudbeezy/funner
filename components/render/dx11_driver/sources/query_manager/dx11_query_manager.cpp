#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    �������� ���������� ��������� ��������
*/

struct QueryManager::Impl: public DeviceObject
{
/// �����������
  Impl (const DeviceManager& manager)
    : DeviceObject (manager)
  {
  }
};

/*
    ����������� / ����������
*/

QueryManager::QueryManager (const DeviceManager& manager)
  : impl (new Impl (manager))
{
}

QueryManager::~QueryManager ()
{
}

/*
    �������� ����������
*/

IPredicate* QueryManager::CreatePredicate ()
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::QueryManager::CreatePredicate");
    throw;
  }
}

IQuery* QueryManager::CreateQuery (QueryType type)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::QueryManager::CreateQuery");
    throw;
  }
}
