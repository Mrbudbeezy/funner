#include "shared.h"

using namespace syslib;

/*
    ��������� ����������: ������ � ������ ���������� ��� ��������� �� ���������
*/

namespace
{

void raise (const char* method_name)
{
  throw xtl::format_not_supported_exception (method_name, "No cookie support for this platform");
}

}

/*
   �������� ������ cookie
*/

void DefaultCookieManager::SetAcceptCookie (bool policy)
{
  raise ("syslib::DefaultCookieManager::SetAcceptCookie");
}

bool DefaultCookieManager::AcceptCookie ()
{
  raise ("syslib::DefaultCookieManager::AcceptCookie");
  return false;
}

/*
   �������� cookie
*/

void DefaultCookieManager::DeleteCookies (const char* uri)
{
  raise ("syslib::DefaultCookieManager::DeleteCookies");
}

void DefaultCookieManager::DeleteAllCookies ()
{
  raise ("syslib::DefaultCookieManager::DeleteAllCookies");
}
