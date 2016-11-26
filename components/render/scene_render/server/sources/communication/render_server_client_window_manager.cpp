#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;
using namespace render;

namespace
{

/*
    Константы
*/

const size_t INIT_STRING_RESERVE_SIZE = 256; //резервируемый объем строки инициализации

/*
    Окно
*/

typedef xtl::com_ptr<manager::INativeWindow> NativeWindowPtr;

struct ClientWindow: public xtl::reference_counter, public manager::INativeWindowListener
{
  NativeWindowPtr           native_window; //окно
  size_t                    id;            //идентификатор окна
  stl::string               name;          //имя окна
  ServerLoopbackConnection& connection;    //соединение

/// Конструктор
  ClientWindow (syslib::Window& window, size_t in_id, const char* in_name, const char* init_string, ServerLoopbackConnection& in_connection)
    : id (in_id)
    , name (in_name)
    , connection (in_connection)
  {
    try
    {
      native_window = NativeWindowPtr (manager::make_native_window (window), false);

      if (!native_window)
        throw xtl::format_operation_exception ("", "render::manager::make_native_window returns null window");

      native_window->AttachListener (this);

      manager::Rect vp = native_window->GetViewport ();

      connection.OnWindowAttached (id, name.c_str (), init_string, native_window->GetHandle (), native_window->GetWidth (), native_window->GetHeight (), size_t (vp.x), size_t (vp.y), size_t (vp.x) + vp.width, size_t (vp.y) + vp.height);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::ClientWindow::ClientWindow");
      throw;
    }
  }

/// Деструктор
  ~ClientWindow ()
  {
    try
    {
      native_window->DetachListener (this);

      connection.OnWindowDetached (id);
    }
    catch (...)
    {
    }
  }

/// События окна
  void OnSizeChanged (unsigned int width, unsigned int height)
  {
    try
    {
      connection.OnWindowSizeChanged (id, width, height);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::ClientWindow::OnSizeChanged");
      throw;
    }
  }

  void OnViewportChanged (const manager::Rect& vp)
  {
    try
    {
      connection.OnWindowViewportChanged (id, size_t (vp.x), size_t (vp.y), size_t (vp.x) + vp.width, size_t (vp.y) + vp.height);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::ClientWindow::OnViewportChanged");
      throw;
    }
  }

  void OnHandleChanged (void* handle)
  {
    try
    {
      connection.OnWindowHandleChanged (id, handle);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::ClientWindow::OnHandleChanged");
      throw;
    }
  }

  void OnPaint ()
  {
    try
    {
      connection.OnWindowPaint (id);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::ClientWindow::OnPaint");
      throw;
    }
  }
};

typedef xtl::intrusive_ptr<ClientWindow>                     WindowPtr;
typedef stl::hash_map<stl::hash_key<const char*>, WindowPtr> WindowMap;

}

/*
    Описание реализации внутреннего менеджера окон
*/

struct ClientWindowManager::Impl
{
  ServerLoopbackConnection& connection; //соединение с сервером
  WindowMap                 windows;    //окна
  size_t                    current_id; //текущий идентификатор окна

/// Конструктор
  Impl (ServerLoopbackConnection& in_connection) : connection (in_connection), current_id (1) {}
};

/*
    Конструктор / деструктор
*/

ClientWindowManager::ClientWindowManager (ServerLoopbackConnection& connection)
  : impl (new Impl (connection))
{
}

ClientWindowManager::~ClientWindowManager ()
{
  DetachAllWindows ();
}

/*
    Присоединение окон
*/

void ClientWindowManager::AttachWindow (const char* name, syslib::Window& in_window, const common::PropertyMap& properties)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    if (impl->windows.find (name) != impl->windows.end ())
      throw xtl::format_operation_exception ("", "Window '%s' has been already attached", name);

    stl::string init_string, value;

    init_string.reserve (INIT_STRING_RESERVE_SIZE);

    for (size_t i=0, count=properties.Size (); i<count; i++)
    {
      const char* name = properties.PropertyName (i);

      properties.GetProperty (i, value);

      init_string += name;
      init_string += "='";
      init_string += value;
      init_string += "' ";
    }

    WindowPtr window (new ClientWindow (in_window, impl->current_id, name, init_string.c_str (), impl->connection), false);

    impl->windows [name] = window;

    impl->current_id++;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ClientWindowManager::AttachWindow");
    throw;
  }
}

void ClientWindowManager::DetachWindow (const char* name)
{
  try
  {
    if (!name)
      return;

    impl->windows.erase (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ClientWindowManager::DetachWindow");
    throw;
  }
}

void ClientWindowManager::DetachAllWindows ()
{
  try
  {
    impl->windows.clear ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ClientWindowManager::DetachAllWindows");
    throw;
  }
}
