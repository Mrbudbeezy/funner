#include "shared.h"

using namespace render::scene::client;

/*
    �������� ���������� ������� �����
*/

struct Client::Impl: public xtl::reference_counter
{
  stl::auto_ptr<Connection> connection; //������ �� ����������
};

/*
    ����������� / ����������
*/

Client::Client (const char* connection_name, const char* init_string, size_t logon_timeout_ms)
{
  try
  {
    if (!connection_name)
      throw xtl::make_null_argument_exception ("", "connection_name");

    if (!init_string)
      init_string = "";

    impl = new Impl;

    impl->connection.reset (new Connection (connection_name, init_string, logon_timeout_ms));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Client::Client");
    throw;
  }
}

Client::Client (const Client& render)
  : impl (render.impl)
{
  addref (impl);
}

Client::~Client ()
{
  release (impl);
}

Client& Client::operator = (const Client& render)
{
  Client (render).Swap (*this);

  return *this;
}

/*
    �������� ������� ����������
*/

const char* Client::Description () const
{
  return impl->connection->Description ();
}

/*
    �������� ���� ����������
*/

RenderTarget Client::CreateRenderTarget (const char* target_name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ������������ ������� ����������� ����������
*/

void Client::SetMaxDrawDepth (size_t level)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

size_t Client::MaxDrawDepth () const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ������ � ���������
*/

void Client::LoadResource (const char* file_name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Client::UnloadResource (const char* file_name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    �����
*/

void Client::Swap (Client& render)
{
  stl::swap (impl, render.impl);
}

namespace render
{

namespace scene
{

void swap (Client& render1, Client& render2)
{
  render1.Swap (render2);
}

}

}
