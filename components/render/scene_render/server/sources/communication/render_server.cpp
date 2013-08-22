#include "shared.h"

using namespace render::scene::server;

/*
    �������� ���������� ������� ���������� �����
*/

struct Server::Impl
{
  ServerImpl               server;              //������ ����������
  ConnectionAcceptor       acceptor;            //������, ����������� �������� �����������
  ServerLoopbackConnection loopback_connection; //���������� ��� �������������� � ��������
  ClientWindowManager      window_manager;      //�������� ����

/// �����������
  Impl (const char* name, ServerThreadingModel threading_model)
    : acceptor (name, server, threading_model)
    , loopback_connection (name)
    , window_manager (loopback_connection)
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
    e.touch ("render::scene::server::Server::Server");
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
  try
  {
    impl->window_manager.AttachWindow (name, window, init_string);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Server::AttachWindow");
    throw;
  }
}

void Server::DetachWindow (const char* name)
{
  try
  {
    impl->window_manager.DetachWindow (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Server::DetachWindow");
    throw;
  }
}

void Server::DetachAllWindows ()
{
  try
  {
    impl->window_manager.DetachAllWindows ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Server::DetachAllWindows");
    throw;
  }
}
