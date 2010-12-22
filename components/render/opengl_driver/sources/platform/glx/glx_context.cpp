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
  Output::Pointer           output;                //������� ���������� ������
  AdapterPtr                adapter;               //������� ������� ���������
  bool                      vsync;                 //�������� �� ������������ �������������
  ListenerArray             listeners;             //��������� ������� ���������
  xtl::trackable::slot_type on_destroy_swap_chain; //���������� �������� ������� ������
  PrimarySwapChain*         swap_chain;            //������� ������� ������
  static Impl*              current_context;       //������� ��������
  
///�����������
  Impl ()
    : vsync (false)
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
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::Context::Context");
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
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::Context::MakeCurrent");
}

/*
    �������� ������������� ������� ������ � ����������
*/

bool Context::IsCompatible (ISwapChain* in_swap_chain)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::Context::IsCompatible");
}

/*
    ��������� ���������� ���������� OpenGL
*/

IAdapterLibrary& Context::GetLibrary ()
{
  return *impl->adapter->GetLibrary ();
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
