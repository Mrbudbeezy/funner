#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::iphone;

/*
    �������� ���������� ��������� OpenGL
*/

typedef stl::vector<IContextListener*> ListenerArray;

struct Context::Impl
{
  Log                         log;                   //��������
  EAGLContext*                context;               //�������� OpenGL
  ISwapChain*                 swap_chain;            //������� ������� ������
  xtl::trackable::slot_type   on_destroy_swap_chain; //���������� �������� ������� ������
  ListenerArray               listeners;             //��������� ������� ���������
  Library*                    opengl_library;        //���������� ������ OpenGL
  static Impl*                current_context;       //������� ��������

  Impl (Library* in_library)
    : swap_chain (0), on_destroy_swap_chain (xtl::bind (&Impl::OnDestroySwapChain, this)), opengl_library (in_library)
    {}

///���������� �������� ������� ������
  void OnDestroySwapChain ()
  {
    swap_chain = 0;
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

Context::Context (ISwapChain* in_swap_chain, Library* library)
  : impl (new Impl (library))
{
  try
  {
      //�������� ������������ ����������

    if (!in_swap_chain)
      throw xtl::make_null_argument_exception ("", "swap_chain");

    impl->swap_chain = in_swap_chain;

    if (!library)
      throw xtl::make_null_argument_exception ("", "library");

      //�������� ���������

#ifdef OPENGL_ES2_SUPPORT
    impl->context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
#else
    impl->context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
#endif

    impl->log.Printf ("Create context (id=%u)...", GetId ());

    if (!impl->context)
      throw xtl::format_operation_exception ("", "Can't create EAGL context");

    ISwapChainImpl* swap_chain = cast_object<ISwapChainImpl> (impl->swap_chain, "", "swap_chain");

    swap_chain->InitializeForContext (impl->context, this);

      //�������� �� ������� �������� ������� ������

    impl->swap_chain->RegisterDestroyHandler (impl->on_destroy_swap_chain);

    impl->log.Printf ("...context created successfull (handle=%08X)", impl->context);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::iphone::Context::Context");
    throw;
  }
}

Context::~Context ()
{
  try
  {
    impl->log.Printf ("Destroy context (id=%u)...", GetId ());

    if (impl->swap_chain)
    {
      try
      {
        ISwapChainImpl* swap_chain = cast_object<ISwapChainImpl> (impl->swap_chain, "", "swap_chain");

        swap_chain->DoneForContext ();
      }
      catch (xtl::exception& e)
      {
        impl->log.Printf ("Exception while closing context for swap chain: '%s'", e.what ());
      }
      catch (...)
      {
        impl->log.Printf ("Unknown exception while closing context for swap chain");
      }
    }

      //������ �������� ���������

    if (Impl::current_context == impl.get ())
    {
      impl->LostCurrentNotify ();
      Impl::current_context = 0;
    }

      //�������� ���������

    impl->log.Printf ("...delete context (handle=%08X)", impl->context);

    [impl->context release];

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
      throw xtl::format_operation_exception ("", "Can't set another swap chain for context");

      //��������� �������� ���������

    if (Impl::current_context)
      Impl::current_context->LostCurrentNotify ();

    Impl::current_context = 0;

    if (![EAGLContext setCurrentContext:impl->context])
      throw xtl::format_operation_exception ("EAGLContext::setCurrentContext", "Can't set current context");

    Impl::current_context = impl.get ();

      //���������� �� ��������� �������� ���������

    impl->SetCurrentNotify ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::iphone::Context::MakeCurrent");
    throw;
  }
}

/*
   �������� ������������� ������� ������ � ����������
*/

bool Context::IsCompatible (ISwapChain* in_swap_chain)
{
  return true;
}

/*
    ��������� ���������� ���������� OpenGL
*/

ILibrary& Context::GetLibrary ()
{
  return *impl->opengl_library;
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
