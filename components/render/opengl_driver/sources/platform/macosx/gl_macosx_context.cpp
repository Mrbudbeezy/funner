#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::macosx;

/*
    �������� ���������� ��������� OpenGL
*/

typedef xtl::com_ptr<Library>          LibraryPtr;
typedef stl::vector<IContextListener*> ListenerArray;

struct Context::Impl
{
  Log                         log;                   //��������
  AGLContext                  context;               //�������� OpenGL
  AGLPixelFormat              pixel_format;          //������ ��������
  ISwapChain*                 swap_chain;            //������� ������� ������
  xtl::trackable::slot_type   on_destroy_swap_chain; //���������� �������� ������� ������
  ListenerArray               listeners;             //��������� ������� ���������
  LibraryPtr                  opengl_library;        //���������� ������ OpenGL
  static Impl*                current_context;       //������� ��������

  Impl (Library* in_library)
    : context (0), pixel_format (0), swap_chain (0),
      on_destroy_swap_chain (xtl::bind (&Impl::OnDestroySwapChain, this)), opengl_library (in_library)
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

    if (!library)
      throw xtl::make_null_argument_exception ("", "library");

    ISwapChainImpl* swap_chain = cast_object<ISwapChainImpl> (in_swap_chain, "", "swap_chain");

      //��������� ������� ��������

    impl->pixel_format = swap_chain->GetPixelFormat ();

    if (!impl->pixel_format)
      throw xtl::format_operation_exception ("", "Null pixel format");

      //�������� ���������

    impl->context = aglCreateContext (impl->pixel_format, 0);

    impl->log.Printf ("Create context (id=%u)...", GetId ());

    if (!impl->context)
      raise_agl_error ("::aglCreateContext");

    impl->log.Printf ("...context created successfull (handle=%08X)", impl->context);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::macosx::Context::Context");
    throw;
  }
}

Context::~Context ()
{
  try
  {
    impl->log.Printf ("Destroy context (id=%u)...", GetId ());

      //������ �������� ���������

    if (Impl::current_context == impl.get ())
    {
      impl->LostCurrentNotify ();
      Impl::current_context = 0;
    }

      //�������� ���������

    impl->log.Printf ("...delete context (handle=%08X)", impl->context);

    if (!aglDestroyContext (impl->context))
      raise_agl_error ("::aglDestroyContext");

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

      casted_swap_chain->SetContext (impl->context);

        //�������� �� ������� �������� ������� ������

      casted_swap_chain->RegisterDestroyHandler (impl->on_destroy_swap_chain);

      impl->swap_chain = swap_chain;
    }

      //��������� �������� ���������

    if (!aglSetCurrentContext (impl->context))
      raise_agl_error ("::aglSetCurrentContext");

    if (Impl::current_context)
      Impl::current_context->LostCurrentNotify ();

    Impl::current_context = impl.get ();

      //���������� �� ��������� �������� ���������

    impl->SetCurrentNotify ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::macosx::Context::MakeCurrent");
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
    ISwapChainImpl* casted_swap_chain = cast_object<ISwapChainImpl> (in_swap_chain, "", "in_swap_chain");

    return impl->pixel_format == casted_swap_chain->GetPixelFormat ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::macosx::Context::IsCompatible");
    throw;
  }
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
