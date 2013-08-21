#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

/*
    �������� ���������� acceptor
*/

struct ConnectionAcceptor::Impl
{
  stl::string name; //��� �������

/// �����������
  Impl (const char* in_name)
    : name (in_name)
  {
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
      throw xtl::make_not_implemented_exception (__FUNCTION__);
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

ConnectionAcceptor::ConnectionAcceptor (const char* name)
{
  try
  {
    impl.reset (new Impl (name));
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

