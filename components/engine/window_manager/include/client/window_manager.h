#ifndef CLIENT_WINDOW_MANAGER_HEADER
#define CLIENT_WINDOW_MANAGER_HEADER

#include <common/var_registry.h>

#include <syslib/window.h>

#include <client/engine.h>

namespace client
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class WindowManager
{
  public:
    typedef xtl::function<void (syslib::Window& window, common::VarRegistry& var_registry, Engine& engine)> StartupHandler;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterStartupHandler       (const char* name, const StartupHandler&, size_t order = 1); //��� ���� order, ��� ������ �����������
    static void UnregisterStartupHandler     (const char* name);
    static void UnregisterAllStartupHandlers ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void InitWindow  (syslib::Window& window, common::VarRegistry& var_registry, Engine& engine);
    static void InitWindows (common::VarRegistry& var_registry, Engine& engine);
};

}

#endif
