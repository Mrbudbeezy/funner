#ifndef RENDER_SCENE_WINDOW_MANAGER_SHARED_HEADER
#define RENDER_SCENE_WINDOW_MANAGER_SHARED_HEADER

#include <stl/auto_ptr.h>

#include <render/manager/window.h>

namespace render
{

namespace scene
{

namespace server
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class WindowManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    WindowManager  ();
    ~WindowManager ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � �������� ����  
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AttachWindow (size_t id, const char* name, void* handle, size_t width, size_t height, const manager::Rect& rect);    
    void DetachWindow (size_t id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetWindowSize     (size_t id, size_t width, size_t height);
    void SetWindowViewport (size_t id, const manager::Rect& rect);
    void SetWindowHandle   (size_t id, void* handle);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RepaintWindow (size_t id);

  private:
    WindowManager (const WindowManager&); //no impl
    WindowManager& operator = (const WindowManager&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

}

#endif
