#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

/*
    �������� ���������� ���������
*/

typedef stl::vector<IContextListener*> ListenerArray;
typedef xtl::com_ptr<Adapter>          AdapterPtr;

struct Context::Impl
{  
  Log                       log;                   //�������� �������� ���������
  AdapterPtr                adapter;               //������� ������� ���������
  Display*                  display;               //���������� ����������� ��� �������� ���������
  Window                    window;                //���� ���������
  GLXContext                glx_context;           //�������� GLX-����������
  ListenerArray             listeners;             //��������� ������� ���������
  xtl::trackable::slot_type on_destroy_swap_chain; //���������� �������� ������� ������
  ISwapChainImpl*           swap_chain;            //������� ������� ������
  static Impl*              current_context;       //������� ��������
  
///�����������
  Impl ()
    : on_destroy_swap_chain (xtl::bind (&Impl::OnDestroySwapChain, this))
    , swap_chain (0)
  {    
  }
  
///����� �������� ���������
  void ResetContext ()
  {
    if (current_context != this)
      return;

    LostCurrentNotify ();

    current_context = 0;
  }

///���������� �������� ������� ������
  void OnDestroySwapChain ()
  {    
    if (current_context == this)
      ResetContext ();
    
    swap_chain  = 0;
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

    ISwapChainImpl* swap_chain = cast_object<ISwapChainImpl> (in_swap_chain, "", "swap_chain");    

      //�������� ����������

    impl = new Impl;    

    impl->adapter = cast_object<Adapter> (swap_chain->GetAdapter (), "", "adapter");
    impl->display = swap_chain->GetDisplay ();
    
    DisplayLock lock (impl->display);

      //�������� ���������            

    impl->log.Printf ("Create context (id=%d)...", GetId ());
    
    impl->glx_context = impl->adapter->GetLibrary ().CreateContext (swap_chain->GetDisplay (), 
                                                                    swap_chain->GetFBConfig (),
                                                                    GLX_RGBA_TYPE, NULL, True);
    
    if (!impl->glx_context)
      raise_error ("::glxCreateContext");
      
      //���������� � �������� ���������
    
    impl->log.Printf ("...context successfully created (handle=%08x)", impl->glx_context);
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

    DisplayLock lock (impl->display);

    impl->log.Printf ("...delete context (handle=%08X)", impl->glx_context);

    impl->adapter->GetLibrary ().DestroyContext (impl->display, impl->glx_context);    

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

      ISwapChainImpl* casted_swap_chain = cast_object<ISwapChainImpl> (swap_chain, "", "swap_chain");

        //�������� �� ������� �������� ������� ������

      casted_swap_chain->RegisterDestroyHandler (impl->on_destroy_swap_chain);

      impl->swap_chain = casted_swap_chain;
      impl->display    = impl->swap_chain->GetDisplay ();
    }
    
      //���������� � ������ �������� ���������
    
    if (Impl::current_context)
    {
      Impl::current_context->LostCurrentNotify ();    
      
      impl->current_context = 0;
    }
    
    DisplayLock lock (impl->display);
    
      //��������� �������� ���������                
      
    impl->adapter->GetLibrary ().MakeCurrent (impl->display, impl->window, impl->window, impl->glx_context);

      // TODO: ��������� ���������      
      
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
    ISwapChainImpl* swap_chain = cast_object<ISwapChainImpl> (in_swap_chain, "", "swap_chain");

    return swap_chain->GetDisplay () == impl->display; //������ ����� �� ���������!!!
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

AdapterLibrary& Context::GetLibrary ()
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
