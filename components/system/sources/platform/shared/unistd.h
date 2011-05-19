#ifndef SYSLIB_UNISTD_PLATFORM_HEADER
#define SYSLIB_UNISTD_PLATFORM_HEADER

#include <shared/default_window_manager.h>
#include <shared/default_library_manager.h>
#include <shared/default_timer_manager.h>
#include <shared/default_application_manager.h>
#include <shared/pthread_manager.h>

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� POSIX
///////////////////////////////////////////////////////////////////////////////////////////////////
class UnistdLibraryManager: public DefaultLibraryManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static dll_t LoadLibrary   (const wchar_t* name);
    static void  UnloadLibrary (dll_t);
    static void* GetSymbol     (dll_t, const char* symbol_name);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� POSIX
///////////////////////////////////////////////////////////////////////////////////////////////////
class UnistdApplicationManager: public DefaultApplicationManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ���������� ���� �� miliseconds ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void Sleep (size_t miliseconds);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static IApplicationDelegate* CreateDefaultApplicationDelegate ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class UnistdPlatform
 : public DefaultWindowManager
 , public DefaultTimerManager 
 , public PThreadManager 
 , public UnistdLibraryManager
 , public UnistdApplicationManager
{
};

}

#endif
