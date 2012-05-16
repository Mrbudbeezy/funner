#include "../shared.h"

using namespace syslib;
using namespace common;

/*
    �������� ������������ ���������
*/

dll_t WindowsLibraryManager::LoadLibrary (const wchar_t* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    HMODULE library = LoadLibraryW (name);

    if (!library)
      raise_error (common::format ("::LoadLibraryW('%S')", name).c_str ());

    return (dll_t)library;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsLibraryManager::LoadLibrary");
    throw;
  }

  return 0;
}

void WindowsLibraryManager::UnloadLibrary (dll_t handle)
{
  try
  {
    HMODULE library = (HMODULE)handle;

    if (!library)
      throw xtl::make_null_argument_exception ("", "library");

    FreeLibrary (library);

    check_errors ("::UnloadLibrary");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsLibraryManager::UnloadLibrary");
    throw;
  }
}

void* WindowsLibraryManager::GetSymbol (dll_t handle, const char* symbol_name)
{
  static const char* METHOD_NAME = "syslib::WindowsLibraryManager::GetSymbol";

  HMODULE library = (HMODULE)handle;

  if (!library)
    throw xtl::make_null_argument_exception (METHOD_NAME, "library");

  if (!symbol_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "symbol_name");

#ifdef WINCE
  void* address = (void*)GetProcAddressW (library, to_wstring_from_utf8 (symbol_name).c_str ());
#else
  void* address = (void*)GetProcAddress (library, symbol_name);
#endif

    //���������� ��� ������

  SetLastError (0);

  return address;
}
