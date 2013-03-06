#ifndef NETWORK_WRAPPERS_SHARED_HEADER
#define NETWORK_WRAPPERS_SHARED_HEADER

#include <stl/auto_ptr.h>
#include <stl/deque>
#include <stl/list>
#include <stl/string>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/signal.h>

#include <common/component.h>
#include <common/file.h>
#include <common/lockable.h>
#include <common/log.h>
#include <common/singleton.h>
#include <common/strlib.h>
#include <common/time.h>

#include <syslib/condition.h>
#include <syslib/mutex.h>
#include <syslib/thread.h>

#include <network/inet_address.h>
#include <network/socket.h>
#include <network/socket_address.h>
#include <network/tcp_client.h>
#include <network/url.h>
#include <network/url_connection.h>

#include <platform/platform.h>

namespace network
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� URL ��⮪��
///////////////////////////////////////////////////////////////////////////////////////////////////
class UrlStreamManagerImpl
{
  public:
    typedef UrlStreamManager::StreamCreator StreamCreator;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� / ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    UrlStreamManagerImpl  ();
    ~UrlStreamManagerImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��ࠡ��稪�� URL ����ᮢ
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterStreamCreator       (const char* url_regexp, const StreamCreator& creator);
    void UnregisterStreamCreator     (const char* url_regexp);
    void UnregisterAllStreamCreators ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��⮪�
///////////////////////////////////////////////////////////////////////////////////////////////////
    IUrlStream* CreateStream (const char* url, const char* params, IUrlStream::IListener& stream);
  
  private:
    UrlStreamManagerImpl (const UrlStreamManagerImpl&); //no impl
    UrlStreamManagerImpl& operator = (const UrlStreamManagerImpl&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

typedef common::Singleton<UrlStreamManagerImpl> UrlStreamManagerSingleton;

}

#endif
