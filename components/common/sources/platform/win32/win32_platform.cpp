#include "shared.h"

using namespace common;

/*
    ��������� �������� ������
*/

ICustomFileSystem* Win32Platform::GetFileSystem ()
{
  return &*Singleton<Win32FileSystem>::Instance ();
}
