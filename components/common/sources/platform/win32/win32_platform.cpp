#include "shared.h"

using namespace common;

/*
    ��������� �������� ������
*/

ICustomFileSystem* Win32Platform::GetFileSystem ()
{
#ifdef WINCE
  return &*Singleton<Win32FileSystem>::Instance ();
#else
  return DefaultPlatform::GetFileSystem ();
#endif
}
