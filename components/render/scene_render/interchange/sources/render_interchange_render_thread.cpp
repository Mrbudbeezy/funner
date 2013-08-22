#include "shared.h"

using namespace render::scene::interchange;

namespace
{

const char*  LOG_NAME           = "render.scene.interchange";
const size_t MAX_POP_TIMEOUT_MS = 100;

}

typedef xtl::com_ptr<IConnection> ConnectionPtr;

struct RenderThread::Impl: public xtl::reference_counter
{
  stl::string       name;                   //��� ����
  common::Log       log;                    //����� ����������������
  stl::string       manager_name;           //��� ��������� ����������
  CommandQueue      command_queue;          //������� ������
  volatile bool     stop_request;           //���� ������� ��������� ����  
  syslib::Thread    thread;                 //����

/// �����������
  Impl (const char* in_name, size_t render_queue_size)
    : name (common::format ("%s.%s", LOG_NAME, in_name))
    , log (name.c_str ())
    , command_queue (render_queue_size)
    , stop_request ()
    , thread (name.c_str (), xtl::bind (&Impl::Run, this))
  {
  }

/// ����������
  ~Impl ()
  {
    try
    {
      stop_request = true;

      thread.Join ();
    }
    catch (...)
    {
    }
  }

///���������� ���������
  class ProxyConnection: public IConnection, public xtl::reference_counter, public xtl::trackable
  {
    public:
      /// �����������
      ProxyConnection (Impl* in_impl, IConnection* in_source_connection) : impl (in_impl), source_connection (in_source_connection) {}

      /// ��������� �������� ������ ������
      void ProcessCommands (const CommandBuffer& commands)
      {
        try
        {
          impl->command_queue.Push (CommandQueueItem (source_connection.get (), commands));
        }
        catch (xtl::exception& e)
        {
          e.touch ("render::scene::interchange::RenderThread::Impl::ProxyConnection::ProcessCommands");
          throw;
        }
      }

      /// ��������� ������� ���������� �� ��������
      xtl::trackable& GetTrackable () { return *this; }

      /// ������� ������
      void AddRef  () { addref (this); }
      void Release () { release (this); }

    private:
      xtl::intrusive_ptr<Impl> impl;
      ConnectionPtr            source_connection;
  };

/// ���� ����
  int Run ()
  {
    while (!stop_request)
    {
      try
      {
        CommandQueueItem command;

        if (!command_queue.Pop (command, MAX_POP_TIMEOUT_MS))
          continue; 

        command.connection->ProcessCommands (command.buffer);
      }
      catch (std::exception& e)
      {
        log.Printf ("%s\n    at render::scene::interchange::RenderThread::Impl::Run", e.what ());
      }
      catch (...)
      {
        log.Printf ("unknown exception\n    at render::scene::interchange::RenderThread::Impl::Run");
      }
    }

    return 0;
  }
};

/*
    ����������� / ���������� / ������������
*/

RenderThread::RenderThread (const char* name, size_t render_queue_size)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    impl = new Impl (name, render_queue_size);
  } 
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::interchange::RenderThread::RenderThread");
    throw;
  }
}

RenderThread::RenderThread (const RenderThread& thread)
  : impl (thread.impl)
{
  addref (impl);
}

RenderThread::~RenderThread ()
{
  release (impl);
}

RenderThread& RenderThread::operator = (const RenderThread& thread)
{
  RenderThread new_thread (thread);

  stl::swap (impl, new_thread.impl);

  return *this;
}

/*
    �������� ����������-����������
*/

IConnection* RenderThread::CreateConnection (IConnection* source_connection)
{
  try
  {
    if (!source_connection)
      throw xtl::make_null_argument_exception ("", "source_connection");

    return new Impl::ProxyConnection (impl, source_connection);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::interchange::RenderThread::CreateConnection");
    throw;
  }
}

/*
    �������� ������������ ��������
*/

namespace
{

typedef xtl::shared_ptr<syslib::Semaphore> SemaphorePtr;

class WaiterConnection: public IConnection, public xtl::reference_counter, public xtl::trackable
{
  public:
    /// �����������
    WaiterConnection (const SemaphorePtr& in_semaphore) : semaphore (in_semaphore) {}

    /// ����������
    ~WaiterConnection () 
    {
      try
      {
        semaphore->Post ();
      }
      catch (...)
      {
      }
    }

    /// ��������� �������� ������ ������
    void ProcessCommands (const CommandBuffer&) { }

    /// ��������� ������� ���������� �� ��������
    xtl::trackable& GetTrackable () { return *this; }

    /// ������� ������
    void AddRef  () { addref (this); }
    void Release () { release (this); }

  private:
    SemaphorePtr semaphore;
};

}

bool RenderThread::WaitQueuedCommands (size_t timeout_ms)
{
  try
  {
    SemaphorePtr semaphore (new syslib::Semaphore (0));

    ConnectionPtr connection (new WaiterConnection (semaphore), false);

    impl->command_queue.Push (CommandQueueItem (connection.get (), CommandBuffer ()));

    connection = ConnectionPtr ();

    if (timeout_ms == size_t (-1)) 
    {
      semaphore->Wait ();
      return true;
    }
    else return semaphore->TryWait (timeout_ms);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::interchange::RenderThread::WaitQueuedCommands");
    throw;
  }
}

void RenderThread::WaitQueuedCommands ()
{
  while (!WaitQueuedCommands (size_t (-1)));
}
