#ifndef ENGINE_SHARED_HEADER
#define ENGINE_SHARED_HEADER

#include <stl/algorithm>
#include <stl/auto_ptr.h>
#include <stl/hash_map>
#include <stl/list>
#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/ref.h>
#include <xtl/reference_counter.h>
#include <xtl/signal.h>

#include <common/component.h>
#include <common/file.h>
#include <common/log.h>
#include <common/parser.h>
#include <common/property_map.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <engine/attachments.h>
#include <engine/config.h>
#include <engine/subsystem_manager.h>

namespace engine
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class StartupManagerImpl
{
  public:
    typedef engine::StartupManager::StartupHandler StartupHandler;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    StartupManagerImpl ();
    ~StartupManagerImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������/�������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterStartupHandler       (const char* node_name, const StartupHandler& startup_handler);
    void UnregisterStartupHandler     (const char* node_name);
    void UnregisterAllStartupHandlers ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Start (const common::ParseNode& node, const char* wc_mask, SubsystemManager& manager);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

/*
   �������� ��������� �������
*/

typedef common::Singleton<StartupManagerImpl> StartupManagerSingleton;

}

#endif
