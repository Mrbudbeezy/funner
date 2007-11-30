#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    �������� ���������� ���������
*/

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

struct Context::Impl
{
  SwapChain*             swap_chain;           //������� ������� ������
  HGLRC                  gl_context;           //�������� OpenGL
  GLEWContext            glew_context;         //�������� ���������� OpenGL
  Trackable::HandlerSlot swap_destroy_handler; //���������� �������� ������� ������
  size_t                 lock_count;           //���������� �������� ���������

  Impl (SwapChain* in_swap_chain) : gl_context (0), swap_chain (0),
    swap_destroy_handler (xtl::bind (&Impl::OnDestroySwapChain, this)), lock_count (0)
  {
    SetSwapChain (in_swap_chain);
    
    ThreadLock lock;

    gl_context = wglCreateContext (swap_chain->GetDC ());

    if (!gl_context)
      raise_error ("wglCreateContext");

    try
    {
      if (!wglMakeCurrent (swap_chain->GetDC (), gl_context))
        raise_error ("wglMakeCurrent");

      GLenum status = glewContextInit (&glew_context);

      if (status != GLEW_OK)
        RaiseInvalidOperation ("glewContextInit", "%s", glewGetString (status));
    }
    catch (...)
    {
      wglDeleteContext (gl_context);
      throw;
    }
  }

  ~Impl ()
  {
    if (IsCurrent ())
    {
      set_current_glew_context (0, 0);
      wglMakeCurrent (0, 0);

      if (ThreadLock::IsLocked ())
        ThreadLock::Unlock ();
    }

    wglDeleteContext (gl_context);
  }

    //���������� ������� �������� ������� ������
  void OnDestroySwapChain ()
  {
    swap_chain = 0;
  }  
  
    //�������� �������� �� �������� �������
  bool IsCurrent ()
  {
    return wglGetCurrentContext () == gl_context && wglGetCurrentDC () == swap_chain->GetDC ();
  }
  
    //������ ���������
  void Lock ()
  {
    if (!lock_count)
    {
      if (!swap_chain)
        RaiseInvalidOperation ("render::low_level::opengl::Context::Lock", "Swap chain was lost");      
        
      try
      {
        ThreadLock::Lock ();

        if (!IsCurrent () && !wglMakeCurrent (swap_chain->GetDC (), gl_context))
          raise_error ("wglMakeCurrent");

        set_current_glew_context (&glew_context, swap_chain->GetWGLEWContext ());
      }
      catch (common::Exception& exception)
      {
        exception.Touch ("render::low_level::opengl::Context::Lock");
        throw;
      }
    }

    lock_count++;
  }

    //������������ ���������
  void Unlock ()
  {
    if (!lock_count)
      return;

    if (!--lock_count)
    {
      if (IsCurrent ())
      {
        set_current_glew_context (0, 0);
        ThreadLock::Unlock ();
      }
    }
  }

    //��������: �������� �� ��������
  bool IsLocked () { return lock_count > 0; }

    //��������� ������� ������� ������
  void SetSwapChain (SwapChain* in_swap_chain)
  {
    if (!in_swap_chain)
    {
      swap_chain = 0;
      swap_destroy_handler.disconnect ();

      return;
    }

      //����������� �������

    swap_chain = in_swap_chain;

    swap_chain->RegisterDestroyHandler (swap_destroy_handler);    
  }

    //��������� ������� ������� ������
  SwapChain* GetSwapChain () { return swap_chain; }
};

/*
    ����������� / ����������
*/

Context::Context (ISwapChain* in_swap_chain)
{
  if (!in_swap_chain)
    RaiseNullArgument ("render::low_level::opengl::Context::Context", "swap_chain");
   
  try
  {
    impl = new Impl (cast_object<SwapChain> (in_swap_chain, "render::low_level::opengl::Context::Context", "swap_chain"));
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::Context::Context");
    throw;
  }
}

Context::~Context ()
{
}

/*
    ��������� ���������� ������ ���������
*/

void Context::Lock ()
{
  impl->Lock ();
}

void Context::Unlock ()
{
  impl->Unlock ();
}

bool Context::IsLocked () const
{
  return impl->IsLocked ();
}

/*
    ��������� ������� ������
*/

void Context::SetSwapChain (ISwapChain* swap_chain)
{
  impl->SetSwapChain (cast_object<SwapChain> (swap_chain, "render::low_level::opengl::Context::SetSwapChain", "swap_chain"));
}

ISwapChain* Context::GetSwapChain () const
{
  return impl->GetSwapChain ();
}

/*
    ��������� ������ ����������, ��������� �� ������� ������� ������
*/

const char* Context::GetSwapChainExtensionString () const
{
  if (!impl->swap_chain)
    return "";

  impl->Lock ();

  const char* result = WGLEW_ARB_extensions_string ? wglGetExtensionsStringARB (impl->swap_chain->GetDC ()) : "";

  impl->Unlock ();

  return result;
}
