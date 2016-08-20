#include <xtl/common_exceptions.h>

#include <shared/message_queue.h>

using namespace syslib;

static int message_queue_id = 1;

/*
    MessageQueue::Handler
*/

MessageQueue::Handler::Handler ()  
{
  id = xtl::atomic_increment (message_queue_id);
}

MessageQueue::Handler::Handler (const Handler&)
{
  id = xtl::atomic_increment (message_queue_id);
}

/*
    ����������� / ����������
*/

MessageQueue::MessageQueue ()
  : was_empty_message_pushed (false)
{
}

MessageQueue::~MessageQueue ()
{
  try
  {
    mutex.Lock ();
    
    if (!messages.empty ()) //�������� �����, � ������ empty ������ ��� ��� ������
      signals [MessageQueueEvent_OnEmpty]();
  }
  catch (...)
  {
  }
}

/*
    ����������� ��������� ������������ ������������
*/

void MessageQueue::RegisterHandler (const Handler& handler)
{
  try
  {
    Lock lock (mutex);
    
    handlers.insert (handler.Id ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MessageQueue::RegisterHandler");
    throw;
  }
}

void MessageQueue::UnregisterHandler (const Handler& handler)
{
  try
  {
    Lock lock (mutex);
    
    handlers.erase (handler.Id ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MessageQueue::UnregisterHandler");
    throw;
  }
}

/*
    ��������� ��������� � �������
*/

void MessageQueue::PushMessage (const Handler& handler, const MessagePtr& message)
{
  try
  {
    Lock lock (mutex);
    
    message->id = handler.Id ();
    
    bool is_empty = messages.empty ();
    
    messages.push (message);
    
    if (is_empty)
    {
      try
      {
        signals [MessageQueueEvent_OnNonEmpty]();
      }
      catch (...)
      {
      }      
      
      condition.NotifyAll ();
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MessageQueue::PushMessage");
    throw;
  }
}

//������������ ��� ����������� ���� ��������� ���������
void MessageQueue::PushEmptyMessage ()
{
  try
  {
    Lock lock (mutex);
    
    if (messages.empty ())
    {
      was_empty_message_pushed = true;

      try
      {
        signals [MessageQueueEvent_OnNonEmpty]();
      }
      catch (...)
      {
      }
      
      condition.NotifyAll ();
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MessageQueue::PushEmptyMessage");
    throw;
  }
}

/*
    ��������������� ������� ���������� ��������� ��� �����������
*/

void MessageQueue::DispatchFirstMessage ()
{
  try
  {
    MessagePtr message;
    
    {    
      Lock lock (mutex);

      while (messages.empty ())
        condition.Wait (mutex);

      message = messages.front ();
      
      messages.pop ();
      
      if (messages.empty ())
      {
        try
        {
          signals [MessageQueueEvent_OnEmpty]();
        }
        catch (...)
        {
        }        
      }
      
      if (!handlers.count (message->id))
        return; //���������� ��������� �� ���������������
    }
    
    if (message)
      message->Dispatch ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MessageQueue::DispatchFirstMessage");
    throw;
  }
}

/*
    �������� ���������� ��������� � ������� / �������� ���������
*/

bool MessageQueue::IsEmpty () const
{
  try
  {
    Lock lock (const_cast<Mutex&> (mutex));
    
    return messages.empty ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MessageQueue::IsEmpty");
    throw;
  }
}

void MessageQueue::WaitMessage ()
{
  try
  {
    Lock lock (mutex);

    while (messages.empty () && !was_empty_message_pushed)
      condition.Wait (mutex);

    was_empty_message_pushed = false;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MessageQueue::IsEmpty");
    throw;
  }
}

/*
    ����������� ���������� ������� ������� ���������
*/

xtl::connection MessageQueue::RegisterEventHandler (MessageQueueEvent event, const EventHandler& handler)
{
  switch (event)
  {
    case MessageQueueEvent_OnEmpty:
    case MessageQueueEvent_OnNonEmpty:
      break;
    default:
      throw xtl::make_argument_exception ("syslib::MessageQueue::RegisterEventHandler", "event", event);
  }
  
  return signals [event].connect (handler);
}
