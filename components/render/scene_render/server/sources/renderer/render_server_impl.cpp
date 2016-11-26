#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

/*
    Описание реализации сервера рендеринга
*/

struct ServerImpl::Impl
{
  stl::string             server_name;      //имя сервера
  server::RenderManager   render_manager;   //менеджер рендеринга
  server::WindowManager   window_manager;   //менеджер окон
  server::ViewportManager viewport_manager; //менеджер областей вывода
  server::ScreenManager   screen_manager;   //менеджер экранов
  server::SceneManager    scene_manager;    //менеджер сцены
  server::ResourceManager resource_manager; //менеджер ресурсов
  server::MaterialManager material_manager; //менеджер материалов

/// Конструктор
  Impl (const char* name)
    : server_name (name)
    , render_manager (name)
    , window_manager (render_manager)
    , viewport_manager ()
    , screen_manager (window_manager, render_manager, viewport_manager)
    , scene_manager (render_manager)
    , resource_manager (render_manager)
    , material_manager (render_manager)
  {
  }
};

/*
    Конструктор / деструктор
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
    Поток отладочного протоколирования
*/

common::Log& ServerImpl::Log ()
{
  return impl->render_manager.Log ();
}

/*
    Менеджеры
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

SceneManager& ServerImpl::SceneManager ()
{
  return impl->scene_manager;
}

ResourceManager& ServerImpl::ResourceManager ()
{
  return impl->resource_manager;
}

MaterialManager& ServerImpl::MaterialManager ()
{
  return impl->material_manager;
}
