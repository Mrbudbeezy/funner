#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

/*
    �������� ���������� ������� ����������
*/

struct ServerImpl::Impl
{
};

/*
    ����������� / ����������
*/

ServerImpl::ServerImpl ()
{
  try
  {
    impl.reset (new Impl);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ServerImpl::ServerImpl");
    throw;
  }
}

ServerImpl::~ServerImpl ()
{
}
