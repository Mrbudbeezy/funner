#include "shared.h"

using namespace render::scene::server;

/*
    �������� ���������� ������� ���������� �����
*/

struct Server::Impl
{
  ServerImpl                     server;              //������ ����������
  ConnectionAcceptor             acceptor;            //������, ����������� �������� �����������
  OutputServerLoopbackConnection loopback_connection; //���������� ��� �������������� � ��������

/// �����������
  Impl (const char* name, ServerThreadingModel threading_model)
    : acceptor (name, server, threading_model)
    , loopback_connection (name)
  {
  }
};

/*
    ����������� / ����������
*/

Server::Server (const char* name, ServerThreadingModel threading_model)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    switch (threading_model)
    {
      case ServerThreadingModel_SingleThreaded:
      case ServerThreadingModel_MultiThreaded:
        break;
      default:
        throw xtl::make_argument_exception ("", "threading_model", threading_model);
    }

    impl.reset (new Impl (name, threading_model));    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Server");
    throw;
  }
}

Server::~Server ()
{
}

/*
    ������������� ����
*/

void Server::AttachWindow (const char* name, syslib::Window& window, const char* init_string)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Server::DetachWindow (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Server::DetachAllWindows ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
