#include <shared/message_queue.h>

using namespace syslib;

/*
    ����������� / ����������
*/

MessageQueue::MessageQueue ()
{
}

MessageQueue::~MessageQueue ()
{
  try
  {
    mutex.Lock ();
  }
  catch (...)
  {
  }
}

/*
    ����������� ��������� ������������ ������������
*/

void MessageQueue::RegisterHandler (handler_t handler)
{
  try
  {
    Lock lock (mutex);
    
    handlers.insert (handler);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MessageQueue::RegisterHandler");
    throw;
  }
}

void MessageQueue::UnregisterHandler (handler_t handler)
{
  try
  {
    Lock lock (mutex);
    
    handlers.erase (handler);
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

void MessageQueue::PushMessage (handler_t handler, const MessagePtr& message)
{
  try
  {
    Lock lock (mutex);
    
    message->handler = handler;
    
    bool is_empty = messages.empty ();
    
    messages.push (message);
    
    if (is_empty)
      condition.NotifyAll ();
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
      condition.NotifyAll ();
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

      if (!handlers.count (message->handler))
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

    while (messages.empty ())
      condition.Wait (mutex);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MessageQueue::IsEmpty");
    throw;
  }
}
