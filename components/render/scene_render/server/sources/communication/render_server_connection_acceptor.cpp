#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

/*
    ���������
*/

const size_t RENDER_THREAD_SIZE = 4; //������ ������� ������ ����������

/*
    �������� ���������� acceptor
*/

struct ConnectionAcceptor::Impl
{
  stl::string                              name;            //��� �������
  ServerThreadingModel                     threading_model; //������ ���������� ��������
  stl::auto_ptr<interchange::RenderThread> render_thread;   //���� ����������
  size_t                                   owner_thread_id; //�������������� ����-���������
  ServerImpl&                              server;          //������ ����������

/// �����������
  Impl (const char* in_name, ServerImpl& in_server, ServerThreadingModel model)
    : name (in_name)
    , threading_model (model)
    , owner_thread_id (syslib::Thread::GetCurrentThreadId ())
    , server (in_server)
  {
    switch (threading_model)
    {
      case ServerThreadingModel_SingleThreaded:
        break;
      case ServerThreadingModel_MultiThreaded:
        render_thread.reset (new interchange::RenderThread (name.c_str (), RENDER_THREAD_SIZE));
        break;
      default:
        throw xtl::format_operation_exception ("", "Invalid threading model %d", threading_model);
    }


    interchange::ConnectionManager::RegisterConnection (name.c_str (), name.c_str (), xtl::bind (&Impl::CreateConnection, this, _2));
  }

/// ����������
  ~Impl ()
  {
    try
    {
      interchange::ConnectionManager::UnregisterConnection (name.c_str ());
    }
    catch (...)
    {
    }
  }

/// �������� ����������
  interchange::IConnection* CreateConnection (const char* init_string)
  {
    try
    {
      switch (threading_model)
      {
        case ServerThreadingModel_SingleThreaded:
          if (syslib::Thread::GetCurrentThreadId () != owner_thread_id)
            throw xtl::format_operation_exception ("", "Can't create rendering connection from thread %u in single threaded model (use ServerThreadingModel_MultiThreaded)", syslib::Thread::GetCurrentThreadId ());

          return new Connection (server, init_string);
        case ServerThreadingModel_MultiThreaded:
        {
          xtl::com_ptr<Connection> connection (new Connection (server, init_string), false);

          return render_thread->CreateConnection (connection.get ());
        }
        default:
          throw xtl::format_operation_exception ("", "Invalid threading model %d", threading_model);
      }
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::ConnectionAcceptor::Impl::CreateConnection");
      throw;
    }
  }
};

/*
    ����������� / ����������
*/

ConnectionAcceptor::ConnectionAcceptor (const char* name, ServerImpl& server, ServerThreadingModel threading_model)
{
  try
  {
    impl.reset (new Impl (name, server, threading_model));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ConnectionAcceptor::ConnectionAcceptor");
    throw;
  }
}

ConnectionAcceptor::~ConnectionAcceptor ()
{
}
