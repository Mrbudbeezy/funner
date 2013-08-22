#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

/*
    �������� ���������� ������� ����������
*/

struct ServerImpl::Impl
{
  server::WindowManager window_manager; //�������� ����
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

/*
    ���������
*/

WindowManager& ServerImpl::WindowManager ()
{
  return impl->window_manager;
}
