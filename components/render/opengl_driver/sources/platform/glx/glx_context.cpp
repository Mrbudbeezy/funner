#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::egl;

/*
    �������� ���������� ���������
*/

typedef stl::vector<IContextListener*> ListenerArray;
typedef xtl::com_ptr<Adapter>          AdapterPtr;

struct Context::Impl
{  
  Log                       log;                   //�������� �������� ���������
  Output::Pointer           output;                //������� ���������� ������
  AdapterPtr                adapter;               //������� ������� ���������  
  EGLDisplay                egl_display;           //������� ������� ���������
  EGLSurface                egl_surface;           //������� ����������� ���������
  EGLContext                egl_context;           //������� �������� ���������
  bool                      vsync;                 //�������� �� ������������ �������������
  ListenerArray             listeners;             //��������� ������� ���������
  xtl::trackable::slot_type on_destroy_swap_chain; //���������� �������� ������� ������  
  PrimarySwapChain*         swap_chain;            //������� ������� ������
  static Impl*              current_context;       //������� ��������
  
///�����������
  Impl ()
    : egl_display (0)
    , egl_surface (0) 
    , egl_context (0)
    , vsync (false)
    , on_destroy_swap_chain (xtl::bind (&Impl::OnDestroySwapChain, this))
    , swap_chain (0)
  {    
  }
  
///����� �������� ���������
  void ResetContext ()
  {
    if (current_context != this)
      return;

    LostCurrentNotify ();
      
    eglMakeCurrent (egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    current_context = 0;
  }

///���������� �������� ������� ������
  void OnDestroySwapChain ()
  {    
    if (current_context == this)
      ResetContext ();
    
    swap_chain  = 0;
    egl_display = 0;
    egl_surface = 0;    
  }

///���������� � ������ ���������
  void LostCurrentNotify ()
  {
    try
    {
      for (ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
        (*iter)->OnLostCurrent ();
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }

///���������� �� ��������� �������� ���������
  void SetCurrentNotify ()
  {
    try
    {
      for (ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
        (*iter)->OnSetCurrent ();
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }
};

Context::Impl* Context::Impl::current_context = 0;

/*
    ����������� / ����������
*/

Context::Context (ISwapChain* in_swap_chain)
{
  try
  {
      //�������� ������������ ����������
      
    if (!in_swap_chain)
      throw xtl::make_null_argument_exception ("", "swap_chain");

    PrimarySwapChain* swap_chain = cast_object<PrimarySwapChain> (in_swap_chain, "", "swap_chain");    

      //�������� ����������

    impl = new Impl;    

    impl->output  = cast_object<Output> (swap_chain->GetContainingOutput (), "", "output");
    impl->adapter = cast_object<Adapter> (swap_chain->GetAdapter (), "", "adapter");

      //�������� ���������

    impl->log.Printf ("Create context (id=%d)...", GetId ());

    impl->egl_context = eglCreateContext (swap_chain->GetEglDisplay (), swap_chain->GetEglConfig (), 0, 0);        
    
    if (!impl->egl_context)
      raise_error ("::eglCreateContext");          
      
      //���������� � �������� ���������
    
    impl->log.Printf ("...context successfully created (handle=%08x)", impl->egl_context);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::egl::Context::Context");
    throw;
  }
}

Context::~Context ()
{
  try
  {
    impl->log.Printf ("Delete context (id=%d)...", GetId ());
    
      //������ �������� ���������

    if (Impl::current_context == impl.get ())
      impl->ResetContext ();

      //�������� ���������

    impl->log.Printf ("...delete context (handle=%08X)", impl->egl_context);

    eglDestroyContext (impl->output->GetEglDisplay (), impl->egl_context);    

    impl->log.Printf ("...context successfully destroyed");
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

/*
    ��������� �������� ���������
*/

void Context::MakeCurrent (ISwapChain* swap_chain)
{
  try
  {
    if (!swap_chain)
      throw xtl::make_null_argument_exception ("", "swap_chain");

    if (swap_chain != impl->swap_chain)
    {
        //��������� ������� ������� ������

      PrimarySwapChain* casted_swap_chain = cast_object<PrimarySwapChain> (swap_chain, "", "swap_chain");

        //�������� �� ������� �������� ������� ������

      casted_swap_chain->RegisterDestroyHandler (impl->on_destroy_swap_chain);

      impl->swap_chain  = casted_swap_chain;
      impl->egl_display = impl->swap_chain->GetEglDisplay ();
      impl->egl_surface = impl->swap_chain->GetEglSurface ();

      SwapChainDesc desc;

      casted_swap_chain->GetDesc (desc);

      impl->vsync = desc.vsync;
    }
    
      //���������� � ������ �������� ���������
    
    if (Impl::current_context)
    {
      Impl::current_context->LostCurrentNotify ();    
      
      impl->current_context = 0;
    }

      //��������� �������� ���������    
      
    if (!eglMakeCurrent (impl->egl_display, impl->egl_surface, impl->egl_surface, impl->egl_context))
      raise_error ("::eglMakeCurrent");

    if (!eglSwapInterval (impl->egl_display, impl->vsync))
      raise_error ("::eglSwapInterval");

    Impl::current_context = impl.get ();

      //���������� �� ��������� �������� ���������

    impl->SetCurrentNotify ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::egl::Context::MakeCurrent");
    throw;
  }
}

/*
    �������� ������������� ������� ������ � ����������
*/

bool Context::IsCompatible (ISwapChain* in_swap_chain)
{
  try
  {
    PrimarySwapChain* swap_chain = cast_object<PrimarySwapChain> (in_swap_chain, "", "swap_chain");

    return swap_chain->GetEglDisplay () == impl->output->GetEglDisplay (); //������ ����� �� ���������!!!
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::egl::Context::IsCompatible");
    throw;
  }
}

/*
    ��������� ���������� ���������� OpenGL
*/

ILibrary& Context::GetLibrary ()
{
  return impl->adapter->GetLibrary ();
}

/*
    �������� �� ������� ���������
*/

void Context::AttachListener (IContextListener* listener)
{
  if (!listener)
    return;

  impl->listeners.push_back (listener);
}

void Context::DetachListener (IContextListener* listener)
{
  impl->listeners.erase (stl::remove (impl->listeners.begin (), impl->listeners.end (), listener), impl->listeners.end ());
}
