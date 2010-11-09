#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#ifdef DestroyAll
#undef DestroyAll
#endif

#ifdef DisplayString
#undef DisplayString
#endif

#include <stl/hash_map>
#include <stl/hash_set>
#include <stl/queue>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/string.h>

#include <common/log.h>
#include <common/property_map.h>
#include <common/singleton.h>
#include <common/strlib.h>
#include <common/utf_converter.h>

#include <syslib/condition.h>
#include <syslib/mutex.h>
#include <syslib/thread.h>
#include <syslib/platform/x11.h>

#include <platform/platform.h>

namespace syslib
{

namespace x11
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class MessageQueue
{
  public:
    typedef void* handler_t;
      
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    class Message: public xtl::reference_counter
    {
      friend class MessageQueue;
      public:
        Message () : handler (0) {}
      
        virtual ~Message () {}      
      
        virtual void Dispatch () = 0;
        
      private:
        handler_t handler;
    };
    
    typedef xtl::intrusive_ptr<Message> MessagePtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    MessageQueue  ();
    ~MessageQueue ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ��������� ������������ ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterHandler   (handler_t);
    void UnregisterHandler (handler_t);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void PushMessage      (handler_t, const MessagePtr& message);
    void PushEmptyMessage (); //������������ ��� ����������� ���� ��������� ���������
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� ������� ���������� ��������� ��� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void DispatchFirstMessage ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ��������� � ������� / �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    bool IsEmpty     () const;
    void WaitMessage ();
    
  private:
    MessageQueue (const MessageQueue&); //no impl
    MessageQueue& operator = (const MessageQueue&); //no impl
    
  private:
    typedef stl::queue<MessagePtr>   MessageQueueImpl;
    typedef stl::hash_set<handler_t> HandlerSet;

  private:
    Mutex            mutex;
    Condition        condition;
    MessageQueueImpl messages;
    HandlerSet       handlers;
};

typedef common::Singleton<MessageQueue> MessageQueueSingleton;

}

}

