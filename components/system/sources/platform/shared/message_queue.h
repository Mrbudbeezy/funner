#ifndef SYSLIB_MESSAGE_QUEUE_INTERNAL_HEADER
#define SYSLIB_MESSAGE_QUEUE_INTERNAL_HEADER

#include <stl/hash_set>
#include <stl/queue>

#include <xtl/exception.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/signal.h>

#include <common/singleton.h>

#include <syslib/condition.h>
#include <syslib/mutex.h>
#include <syslib/application.h>

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum MessageQueueEvent
{
  MessageQueueEvent_OnEmpty,    //������� ����� ������
  MessageQueueEvent_OnNonEmpty, //������� ��������� ���� ������
  
  MessageQueueEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class MessageQueue
{
  public:
    class Handler
    {
      public:
        Handler ();
        Handler (const Handler&);

        Handler& operator = (const Handler&) { return *this; }
        
        size_t Id () const { return id; }

      private:
        size_t id;
    };
      
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    class Message: public xtl::reference_counter
    {
      friend class MessageQueue;
      public:
        Message () : id () {}
      
        virtual ~Message () {}      
      
        virtual void Dispatch () = 0;        

      private:
        size_t id;
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
    void RegisterHandler   (const Handler&);
    void UnregisterHandler (const Handler&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void PushMessage      (const Handler&, const MessagePtr& message);
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
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ���������� ������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void ()> EventHandler;

    xtl::connection RegisterEventHandler (MessageQueueEvent event, const EventHandler&);
    
  private:
    MessageQueue (const MessageQueue&); //no impl
    MessageQueue& operator = (const MessageQueue&); //no impl
    
  private:
    typedef stl::queue<MessagePtr> MessageQueueImpl;
    typedef stl::hash_set<size_t>  HandlerSet;
    typedef xtl::signal<void ()>   EventSignal;

  private:
    Mutex            mutex;
    Condition        condition;
    MessageQueueImpl messages;
    HandlerSet       handlers;
    EventSignal      signals [MessageQueueEvent_Num];
    bool             was_empty_message_pushed;          //this flag is set to true if empty message was pushed to queue in order to tell WaitMessage method to return
};

typedef common::Singleton<MessageQueue> MessageQueueSingleton;

IApplicationDelegate* create_message_queue_application_delegate ();

}

#endif
