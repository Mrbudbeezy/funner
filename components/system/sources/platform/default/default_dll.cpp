#include <platform/platform.h>
#include <common/exception.h>

using namespace syslib;

/*
    ��������� ����������: ������ � ������������� ������������ ���������� ��� ��������� �� ���������
*/

namespace
{

void Raise (const char* method_name)
{
  common::RaiseNotSupported (method_name, "No dynamic libraries support for default platform");
}

}

/*
    �������� ������������ ���������
*/

Platform::dll_t Platform::LoadLibrary (const wchar_t* name)
{
  Raise ("syslib::DefaultPlatform::LoadLibrary");

  return 0;
}

void Platform::UnloadLibrary (dll_t)
{
  Raise ("syslib::DefaultPlatform::UnloadLibrary");
}

void* Platform::GetSymbol (dll_t, const char*)
{
  Raise ("syslib::DefaultPlatform::GetSymbol");
  return 0;
}
