#include <ctime>

#include "shared.h"

using namespace syslib;
using namespace syslib::x11;

/*
    �������� �������
*/

struct Platform::timer_handle
{
  size_t       next_raise_time;        //����� ���������� ������������
  size_t       period_in_milliseconds; //������ ������������
  TimerHandler handler;                //����������
  void*        user_data;              //���������������� ������
  bool         need_delete;            //������ ������� ��������
  
  timer_handle (size_t in_period, TimerHandler in_handler, void* in_user_data)
    : next_raise_time (common::milliseconds () + in_period)
    , period_in_milliseconds (in_period)
    , handler (in_handler)
    , user_data (in_user_data)
    , need_delete (false)
  {
  }
};

namespace
{

/*
    ���������
*/

const char* LOG_NAME = "system.x11"; //��� ������ ����������������

/*
    �������� ��������
*/

class TimerManager
{
  public:
///�����������
    TimerManager ()
      : message_queue (*MessageQueueSingleton::Instance ())
      , log (LOG_NAME)
      , loop (true)
      , kill_lock (false)      
      , thread (xtl::bind (&TimerManager::TimersRoutine, this))
    {
      static bool initialized = false;
      
      if (!initialized)
      {
        initialized = true;
      }
      else
      {
        CloseTimersThread ();
        
        throw xtl::format_operation_exception ("syslib::TimerManager::TimerManager", "TimerManager can't be constructed more than one time");
      }
    }
    
///����������
    ~TimerManager ()
    {
      try
      {
        CloseTimersThread ();
        
        UnregisterAllTimers ();
      }
      catch (...)
      {
        //���������� ���� ����������
      }
    }
  
///�������� �������
    Platform::timer_t CreateTimer (size_t period_in_milliseconds, Platform::TimerHandler handler, void* user_data)
    {
      try
      {
        stl::auto_ptr<Platform::timer_handle> timer;
        
        {
          Lock lock (mutex);

          timer = new Platform::timer_handle (period_in_milliseconds, handler, user_data); 
          
          message_queue.RegisterHandler (timer.get ());
          
          try
          {
            timers.push_back (timer.get ());
          }
          catch (...)
          {
            message_queue.UnregisterHandler (timer.get ());
            throw;
          }
        }
        
        condition.NotifyOne ();
        
        return timer.release ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::TimerManager::CreateTimer");
        throw;
      }
    }
    
///�������� �������
    void KillTimer (Platform::timer_t timer)
    {
      try
      {
        if (!timer)
          throw xtl::make_null_argument_exception ("", "timer");
          
        Lock lock (mutex);
        
        if (kill_lock)
        {
          timer->need_delete = true;
          return;
        }
        
        message_queue.UnregisterHandler (timer);
        
        timers.erase (stl::remove (timers.begin (), timers.end (), timer), timers.end ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::TimerManager::KillTimer");
        throw;
      }
    }

  private:
///���������� ���� ��������� ��������
    void CloseTimersThread ()
    {
      {
        Lock lock (mutex);
      
        loop = false;
      
        condition.NotifyOne ();
      }
      
      thread.Join ();      
    }
    
///�������� ���� ��������
    void UnregisterAllTimers ()
    {
      Lock lock (mutex);
      
      for (TimerList::iterator iter=timers.begin (), end=timers.end (); iter!=end; ++iter)
        message_queue.UnregisterHandler (*iter);
    }
    
///��������� �������
    struct Message: public MessageQueue::Message
    {
      Platform::timer_t timer;
      
      Message (Platform::timer_t in_timer) : timer (in_timer) {}
      
      void Dispatch ()
      {
        if (timer->need_delete)
          return;
        
        timer->handler (timer->user_data);
      }
    };    
  
///���� ��������� ��������
    int TimersRoutine ()
    {
      while (loop)
      {
        Lock lock (mutex);
        
          //�������� ������������ ��������
        
        for (;;)
        {
          size_t time      = common::milliseconds ();
          bool   raised    = false;
          size_t wait_time = ~0;
          
          for (TimerList::iterator iter=timers.begin (), end=timers.end (); iter!=end; ++iter)
          {
            if ((*iter)->next_raise_time <= time)
            {
              raised = true;
              break;
            }
            
            size_t delay = (*iter)->next_raise_time - time;
            
            if (delay < wait_time)
              delay = wait_time;
          }
          
          if (raised)
            break;
          
          if (loop)
            condition.Wait (mutex, wait_time);
        }
        
          //����� ������������ ��������
          
        size_t time = common::milliseconds ();
        
        kill_lock = true;        
        
        for (TimerList::iterator iter=timers.begin (), end=timers.end (); iter!=end; ++iter)
        {
          Platform::timer_handle& timer = **iter;
          
          if (timer.next_raise_time > time)
            continue;
            
          try
          {
            message_queue.PushMessage (&timer, xtl::intrusive_ptr<Message> (new Message (&timer), false));
          }
          catch (std::exception& e)
          {
            log.Printf ("%s\n    at syslib::TimerManager::TimersRoutine", e.what ());
          }
          catch (...)
          {
            log.Printf ("Unknown exception\n    at syslib::TimerManager::TimersRoutine");            
          }
          
          timer.next_raise_time = time + timer.period_in_milliseconds;
        }
        
        kill_lock = false;
        
          //�������� ��������
          
        for (TimerList::iterator iter=timers.begin (), end=timers.end (); iter!=end;)
        {
          if ((*iter)->need_delete) KillTimer (*iter);
          else                      ++iter;
        }
      }
      
      return 0;
    }
  
  private:
    typedef stl::vector<Platform::timer_t> TimerList;

  private:
    MessageQueue& message_queue;   //������� ��������
    common::Log   log;             //����� ����������������
    bool          loop;            //�������� ��������� � ��������� ��������� ��������
    bool          kill_lock;       //������� ������������� �� ��������    
    Mutex         mutex;           //������� ������ � ���������� ��������
    Condition     condition;       //������� ��������� ������ �������
    Thread        thread;          //���� ��������� ��������
    TimerList     timers;          //������ ������������������ ��������
};

typedef common::Singleton<TimerManager> TimerManagerSingleton;

}

/*
    �������� / ����������� �������
*/

Platform::timer_t Platform::CreateTimer (size_t period_in_milliseconds, TimerHandler handler, void* user_data)
{
  try
  {
    TimerManager& manager = *TimerManagerSingleton::Instance ();

    return manager.CreateTimer (period_in_milliseconds, handler, user_data);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::CreateTimer");
    throw;
  }
}

void Platform::KillTimer (timer_t handle)
{
  try
  {
    TimerManager& manager = *TimerManagerSingleton::Instance ();
    
    manager.KillTimer (handle);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::KillTimer");
    throw;
  }
}
