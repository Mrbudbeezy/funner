#ifndef SOCIAL_CORE_SHARED_HEADER
#define SOCIAL_CORE_SHARED_HEADER

#include <stl/hash_map>
#include <stl/string>
#include <stl/vector>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/reference_counter.h>

#include <common/component.h>
#include <common/property_map.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <social/session.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SessionManagerImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SessionManagerImpl  ();
    ~SessionManagerImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterSession       (const char* id, const char* session_name_mask, const social::SessionManager::CreateSessionHandler& handler);
    void UnregisterSession     (const char* id);
    void UnregisterAllSessions ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsSessionRegistered (const char* session_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    social::ISessionManager* CreateSession (const char* session_name, const common::PropertyMap& config);

  private:
    SessionManagerImpl (const SessionManagerImpl&);               //no impl
    SessionManagerImpl& operator = (const SessionManagerImpl&);   //no impl

  private:
    struct Impl;
    Impl* impl;
};

typedef common::Singleton<SessionManagerImpl> SessionManagerSingleton;

#endif
