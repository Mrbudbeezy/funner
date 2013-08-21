#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

namespace
{

/// �������� ���������
class ContextImpl: public Context
{
  public:
    ContextImpl (ConnectionState& state) : Context (state) {}

    using Context::SetCounterparty;
    using Context::ProcessCommands;
};

}

/*
    �������� ���������� ����������
*/

typedef xtl::com_ptr<interchange::IConnection> ConnectionPtr;

struct Connection::Impl: public xtl::trackable
{
  xtl::trackable_ptr<ServerImpl> server;               //������ �� ������
  ConnectionState                state;                //��������� ����������
  ContextImpl                    context;              //��������
  ConnectionPtr                  response_connection;  //�������� ���������� � ��������

/// �����������
  Impl (ServerImpl& in_server, const char* init_string)
    : server (&in_server)
    , context (state)
  {
    common::PropertyMap properties = common::parse_init_string (init_string);

    stl::string initiator = properties.GetString ("initiator");

    response_connection = ConnectionPtr (interchange::ConnectionManager::CreateConnection (initiator.c_str (), "initiator=render::scene::server::Connection"), false);

    context.SetCounterparty (response_connection.get ());
  } 
};

/*
    ����������� / ����������
*/

Connection::Connection (ServerImpl& server, const char* init_string)
{
  try
  {
      //must be thread neutral

    if (!init_string)
      init_string = "";

    impl = new Impl (server, init_string);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Connection::Connection");
    throw;
  }
}

Connection::~Connection ()
{
}

/*
    ������ �����
*/

ServerImpl& Connection::Server ()
{
  return *impl->server;
}

/*
    ��������
*/

server::Context& Connection::Context ()
{
  return impl->context;
}

/*
    ��������� ����������
*/

ConnectionState& Connection::State ()
{
  return impl->state;
}

/*
    ��������� �������� ������ ������
*/

void Connection::ProcessCommands (const interchange::CommandBuffer& commands)
{
  try
  {
    if (!impl->server)
      throw xtl::format_operation_exception ("", "Server is lost");

    impl->context.ProcessCommands (commands);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::server::Connection::ProcessCommands");
    throw;
  }
}

/*
    ��������� ������� ���������� �� ��������
*/

xtl::trackable& Connection::GetTrackable ()
{
  return *impl;
}

/*
    ������� ������
*/

void Connection::AddRef  ()
{
  addref (this);
}

void Connection::Release ()
{
  release (this);
}
