#include <platform/platform.h>

using namespace syslib;

namespace
{

//��������� ����������
void raise (const char* source)
{
  throw xtl::format_not_supported_exception (method_name, "No threads support for default platform");
}

}

/*
    �������� / �������� ����
*/

Platform::thread_t Platform::CreateThread (IThreadCallback*)
{
  raise ("syslib::DefaultPlatform::CreateThread");
}

void Platform::DeleteThread (thread_t)
{
  raise ("syslib::DefaultPlatform::DeleteThread");
}

/*
    ������ ����
*/

void Platform::CancelThread (thread_t)
{
  raise ("syslib::DefaultPlatform::CancelThread");
}

/*
    �������� ���������� ����
*/

void Platform::JoinThread (thread_t)
{
  raise ("syslib::DefaultPlatform::JoinThread");
}

/*
    ������ � ���������� ������� ����
*/

Platform::tls_t Platform::CreateTls (IThreadCleanupCallback* cleanup)
{
  raise ("syslib::DefaultPlatform::CreateTls");
}

void Platform::DeleteTls (tls_t tls)
{
  raise ("syslib::DefaultPlatform::CreateTls");
}

void Platform::SetTls (tls_t tls, void* data)
{
  raise ("syslib::DefaultPlatform::SetTls");
}

void* Platform::GetTls (tls_t tls)
{
  raise ("syslib::DefaultPlatform::GetTls");
}
