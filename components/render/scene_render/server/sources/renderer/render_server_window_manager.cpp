#include "shared.h"

using namespace render;
using namespace render::scene::server;

namespace
{

/*
    ��������������� ������
*/

/// ������� ��� �������� �����
class NativeWindow: public manager::INativeWindow, public xtl::reference_counter
{
  public:
///�����������
    NativeWindow (void* in_handle, size_t in_width, size_t in_height, const manager::Rect& in_viewport)
      : handle (in_handle)
      , width (in_width)
      , height (in_height)
      , viewport (in_viewport)
    {
      static const char* METHOD_NAME = "render::scene::server::NativeWindow::NativeWindow";

      if (!handle)
        throw xtl::make_null_argument_exception (METHOD_NAME, "handle");
    }

///��������� �������� ������
    void SetSize (size_t in_width, size_t in_height)
    {
      try
      {
        width  = in_width;
        height = in_height;

        for (ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
          (*iter)->OnSizeChanged (width, height);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::server::NativeWindow::SetSize");
        throw;
      }
    }

///������� ����
    size_t GetWidth  () { return width; }
    size_t GetHeight () { return height; }

///��������� ������� ������
    void SetViewport (const manager::Rect& rect)
    {
      try
      {
        viewport = rect;

        for (ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
          (*iter)->OnViewportChanged (viewport);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::server::NativeWindow::SetViewport");
        throw;
      }
    }

///������� ������
    manager::Rect GetViewport () { return viewport; }

///��������� ���������������� �����������
    void SetHandle (void* in_handle)
    {
      try
      {
        handle = in_handle;

        for (ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
          (*iter)->OnHandleChanged (handle);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::server::NativeWindow::SetHandle");
        throw;
      }
    }

///�������������� ����������
    void* GetHandle () { return handle; }

///���������
    void Repaint ()
    {
      try
      {
        for (ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
          (*iter)->OnPaint ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::server::NativeWindow::Repaint");
        throw;
      }
    }

///��������� ��������� �������
    void AttachListener (manager::INativeWindowListener* listener) { if (listener) listeners.push_back (listener); }
    void DetachListener (manager::INativeWindowListener* listener) { if (listener) listeners.erase (stl::remove (listeners.begin (), listeners.end (), listener), listeners.end ()); }

///������� ������
    void AddRef  () { addref (this); }
    void Release () { release (this); }

  private:
    typedef stl::vector<manager::INativeWindowListener*> ListenerArray;

  private:
    void*         handle;
    size_t        width;
    size_t        height;
    manager::Rect viewport;
    ListenerArray listeners;
};

typedef xtl::intrusive_ptr<NativeWindow> NativeWindowPtr;

/// ����
class Window: public xtl::reference_counter
{
  public:
/// �����������
    Window (RenderManager& render_manager, const char* in_name, const char* init_string, void* handle, size_t width, size_t height, const manager::Rect& viewport)
      : log (render_manager.Log ())
    {
      try
      {
        if (!in_name)
          throw xtl::make_null_argument_exception ("", "name");

        if (!init_string)
          throw xtl::make_null_argument_exception ("", "init_string");

        name = in_name;

        common::PropertyMap properties = common::parse_init_string (init_string);

        native_window = NativeWindowPtr (new ::NativeWindow (handle, width, height, viewport), false);

        render_window.reset (new manager::Window (render_manager.Manager ().CreateWindow (native_window.get (), properties)));

        log.Printf ("Render window '%s' has been created", name.c_str ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::server::Window::Window");
        throw;
      }
    }

/// ����������
    ~Window ()
    {
      render_window.reset ();

      log.Printf ("Render window '%s' has been destroyed", name.c_str ());
    }

/// ���
    const char* Name () { return name.c_str (); }

/// ���� ����������
    manager::Window& RenderWindow () { return *render_window; }

/// �������� ����
    ::NativeWindow& NativeWindow () { return *native_window; }

  private:
    common::Log                    log;
    NativeWindowPtr                native_window;
    stl::string                    name;
    stl::auto_ptr<manager::Window> render_window;
};

typedef xtl::intrusive_ptr<Window>                           WindowPtr;
typedef stl::hash_map<size_t, WindowPtr>                     WindowMap;
typedef stl::hash_map<stl::hash_key<const char*>, WindowPtr> WindowNamedMap;

}

/*
    �������� ���������� ��������� ����
*/

struct WindowManager::Impl
{
  RenderManager  render_manager; //�������� ����������
  WindowMap      windows;        //����
  WindowNamedMap named_windows;  //����������� ����

/// �����������
  Impl (const RenderManager& in_render_manager)
    : render_manager (in_render_manager)
  {
  }

/// ��������� ����
  Window& GetWindow (size_t id)
  {
    try
    {
      WindowMap::iterator iter = windows.find (id);

      if (iter == windows.end ())
        throw xtl::make_argument_exception ("", "window_id", id, "No window with such id");

      return *iter->second;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::WindowManager::Impl::GetWindow");
      throw;
    }
  }
};

/*
    ����������� / ����������
*/

WindowManager::WindowManager (const RenderManager& manager)
  : impl (new Impl (manager))
{

}

WindowManager::~WindowManager ()
{
}

/*
    ���������� � �������� ����  
*/

void WindowManager::AttachWindow (size_t id, const char* name, const char* init_string, void* handle, size_t width, size_t height, const manager::Rect& rect)
{
  try
  {
    WindowMap::iterator iter = impl->windows.find (id);

    if (iter != impl->windows.end ())
      throw xtl::format_operation_exception ("", "Window with id %u has been already attached", id);
   
    WindowPtr window (new Window (impl->render_manager, name, init_string, handle, width, height, rect), false);

    impl->windows.insert_pair (id, window);

    try
    {
      impl->named_windows.insert_pair (name, window);
    }
    catch (...)
    {
      impl->windows.erase (id);
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::WindowManager::AttachWindow");
    throw;
  }
}

void WindowManager::DetachWindow (size_t id)
{
  try
  {
    WindowMap::iterator iter = impl->windows.find (id);

    if (iter == impl->windows.end ())
      return;

    impl->named_windows.erase (iter->second->Name ());

    impl->windows.erase (iter);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::WindowManager::DetachWindow");
    throw;
  }
}

/*
    ����� ���� �� �����
*/

manager::Window& WindowManager::GetWindow (const char* name) const
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    WindowNamedMap::iterator iter = impl->named_windows.find (name);

    if (iter == impl->named_windows.end ())
      throw xtl::make_argument_exception ("", "name", name, "Window with such name has not been found");

    return iter->second->RenderWindow ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::WindowManager::GetWindow");
    throw;
  }
}

/*
    ���������� ����
*/

void WindowManager::SetWindowSize (size_t id, size_t width, size_t height)
{
  try
  {
    impl->GetWindow (id).NativeWindow ().SetSize (width, height);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::WindowManager::SetWindowSize");
    throw;
  }
}

void WindowManager::SetWindowViewport (size_t id, const manager::Rect& rect)
{
  try
  {
    impl->GetWindow (id).NativeWindow ().SetViewport (rect);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::WindowManager::SeWindowViewport");
    throw;
  }
}

void WindowManager::SetWindowHandle (size_t id, void* handle)
{
  try
  {
    impl->GetWindow (id).NativeWindow ().SetHandle (handle);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::WindowManager::SetWindowHandle");
    throw;
  }
}

/*
    ���������� ����
*/

void WindowManager::RepaintWindow (size_t id)
{
  try
  {
    impl->GetWindow (id).NativeWindow ().Repaint ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::WindowManager::RepaintWindow");
    throw;
  }
}
