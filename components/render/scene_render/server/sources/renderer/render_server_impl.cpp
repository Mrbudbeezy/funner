#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

/*
    �������� ���������� ������� ����������
*/

struct ServerImpl::Impl
{
  stl::string             server_name;      //��� �������
  server::RenderManager   render_manager;   //�������� ����������
  server::WindowManager   window_manager;   //�������� ����
  server::ViewportManager viewport_manager; //�������� �������� ������
  server::ScreenManager   screen_manager;   //�������� �������

/// �����������
  Impl (const char* name)
    : server_name (name)
    , render_manager (name)
    , window_manager (render_manager)
    , viewport_manager ()
    , screen_manager (window_manager, render_manager, viewport_manager)
  {
  }
};

/*
    ����������� / ����������
*/

ServerImpl::ServerImpl (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    impl.reset (new Impl (name));
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
    ����� ����������� ����������������
*/

common::Log& ServerImpl::Log ()
{
  return impl->render_manager.Log ();
}

/*
    ���������
*/

render::manager::RenderManager& ServerImpl::RenderManager ()
{
  return impl->render_manager.Manager ();
}

WindowManager& ServerImpl::WindowManager ()
{
  return impl->window_manager;
}

ScreenManager& ServerImpl::ScreenManager ()
{
  return impl->screen_manager;
}
