#include "shared.h"

using namespace syslib;

/*
    ������� �᪫�祭��: ࠡ�� � ��������筮��� ���������� ��� ������� �� 㬮�砭��
*/

namespace
{

void raise (const char* method_name)
{
  throw xtl::format_not_supported_exception (method_name, "No thread local storage support for this platform");
}

}

tls_t DefaultThreadManager::CreateTls (IThreadCleanupCallback* cleanup)
{
  raise ("syslib::DefaultThreadManager::CreateTls");
  return 0;
}

void DefaultThreadManager::DestroyTls (tls_t tls)
{
  raise ("syslib::DefaultThreadManager::DestroyTls");
}

void DefaultThreadManager::SetTls (tls_t tls, void* data)
{
  raise ("syslib::DefaultThreadManager::SetTls");
}

void* DefaultThreadManager::GetTls (tls_t tls)
{
  raise ("syslib::DefaultThreadManager::GetTls");
  return 0;
}
