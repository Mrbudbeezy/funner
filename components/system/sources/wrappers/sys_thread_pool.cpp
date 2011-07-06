#include "shared.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

using namespace common;
using namespace syslib;

namespace
{

/*
    ���������
*/

const size_t FINISH_BACKGROUND_THREAD_TIME = 2 * 60 * 1000;        //����� ��������������� ���� ��� ���������� �������� �����
const size_t NEW_THREAD_WAIT_ACTIONS_COUNT = 5;                    //���������� ��������� �����, ��� ������� �������� ����� ����
const size_t BACKGROUND_MIN_PAUSE_TIME     = 50;                   //����������� ����� � ������ ������� ����
const size_t BACKGROUND_MAX_PAUSE_TIME     = 200;                  //������������ ����� � ������ ������� ����
const char*  LOG_NAME                      = "system.thread_pool"; //��� ������ ����������������

/*
    ��������������� ����������
*/

class ThreadPool;

void create_new_background_thread (ThreadPool*);

/*
    ��� �������
*/

class ThreadPool: public xtl::reference_counter, public Lockable
{
  public:    
///�����������
    ThreadPool ()
    {
      threads_count = 0;
    }

///����������
    ~ThreadPool ()
    {
    }
  
///��������� ���������� ������� � �������
    void ProcessNewEvent (ActionThread thread_type, Action& action)
    {
      if (thread_type != ActionThread_Background)
        return;
      
      common::Lock lock (*this);
      
      if (!threads_count || ActionQueue::ActionsCount (thread_type) > NEW_THREAD_WAIT_ACTIONS_COUNT)
      {
        create_new_background_thread (this);
      }
    }
    
///����������� ����� ����
    void IncThreadsCounter ()
    {
      threads_count++;
    }
    
    void DecThreadsCounter ()
    {
      threads_count--;
    }
    
  private:
    size_t threads_count;
};

typedef xtl::intrusive_ptr<ThreadPool>           ThreadPoolPtr;
typedef xtl::lock_ptr<ThreadPool, ThreadPoolPtr> ThreadPoolLockPtr;

/*
    ����� ��������� ������� �����
*/

class BackgroundThread: public xtl::reference_counter
{
  public:
///�����������
    BackgroundThread (const ThreadPoolPtr& in_pool)
      : log (LOG_NAME)
      , pool (in_pool)
    {
      Thread (xtl::bind (&BackgroundThread::Run, xtl::intrusive_ptr<BackgroundThread> (this)));
      
      ThreadPoolLockPtr (pool)->IncThreadsCounter ();
    }

///����������
    ~BackgroundThread ()
    {
      log.Printf ("Finish background worker thread %u", thread_id);
      
      ThreadPoolLockPtr (pool)->DecThreadsCounter ();      
    }

  private:
///������� ����
    int Run ()
    {
      thread_id = Platform::GetCurrentThreadId ();      

      log.Printf ("Start background worker thread %u", thread_id);      
      
      size_t last_perform = common::milliseconds ();
        
      for (;;)        
      {
        bool processed_actions = 0;
        
        for (size_t i=0, count=ActionQueue::ActionsCount (ActionThread_Current); i<count; i++)
        {
          try
          {          
            Action action = ActionQueue::PopAction (ActionThread_Current);
              
            if (!action.IsEmpty ())
            {
              action.Perform ();

              last_perform = common::milliseconds ();
              processed_actions++;
            }          
          }
          catch (std::exception& e)
          {
            log.Printf ("Exception in background thread %u: %s", thread_id, e.what ());
          }
          catch (...)
          {
            log.Printf ("Unknown exception in background thread %u", thread_id);
          }
        }

        try
        {
          Action action = ActionQueue::PopAction (ActionThread_Background);
            
          if (!action.IsEmpty ())
          {
            action.Perform ();

            last_perform = common::milliseconds ();
            processed_actions++;
          }
        }
        catch (...)
        {
          //���������� ���� ����������
        }        

        if (!processed_actions)
        {
            //������ ����              

          if (common::milliseconds () - last_perform > FINISH_BACKGROUND_THREAD_TIME)
          {
            ThreadPoolLockPtr lock (pool);

            if (!ActionQueue::ActionsCount (ActionThread_Background))
            {
              ActionQueue::CleanupCurrentThread ();
              return 0;
            }
          }

            //�������� ������

          Platform::Sleep (BACKGROUND_MIN_PAUSE_TIME + stl::random_number (BACKGROUND_MAX_PAUSE_TIME-BACKGROUND_MIN_PAUSE_TIME));
        }
      }      

      return 0;
    }    

  private:
    Log           log;
    ThreadPoolPtr pool;
    size_t        thread_id;
};

void create_new_background_thread (ThreadPool* pool)
{
  xtl::intrusive_ptr<BackgroundThread> (new BackgroundThread (pool), false);
}

/*
    ��������� ������� ���� �����
*/

class Component
{
  public:
    Component ()    
    {
      ThreadPoolPtr pool (new ThreadPool, false);

      ActionQueue::RegisterEventHandler (ActionQueueEvent_OnPushAction, xtl::bind (&ThreadPool::ProcessNewEvent, pool, _1, _2));
    }
};

}

extern "C"
{

ComponentRegistrator<Component> ThreadPool ("common.action_queue.thread_pool");

}
