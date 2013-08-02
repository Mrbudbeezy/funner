#ifndef ANDROID

#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::egl;

/*
    �������� ���������� �����������
*/

struct EglSurface::Impl
{
  EGLDisplay display;
  EGLSurface surface;
};

/*
    ����������� / ����������
*/

EglSurface::EglSurface (EGLDisplay display, EGLConfig config, const void* window_handle)
  : impl (new Impl)
{
  impl->display = display;
  impl->surface = eglCreateWindowSurface (display, config, (NativeWindowType)window_handle, 0);

  if (!impl->surface)
    raise_error ("::eglCreateWindowSurface");
}

EglSurface::~EglSurface ()
{
  eglDestroySurface (impl->display, impl->surface);  
}

/*
    �����������
*/

EGLSurface EglSurface::GetSurface () const
{
  return impl->surface;
}

#endif
