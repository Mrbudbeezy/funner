#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::windows;

/*
    �������� ���������� ��������� OpenGL
*/

typedef xtl::com_ptr<Adapter>          AdapterPtr;
typedef stl::vector<IContextListener*> ListenerArray;

struct Context::Impl: public IContextListener
{
  AdapterPtr                  adapter;               //�������
  HGLRC                       context;               //�������� OpenGL
  int                         pixel_format;          //������ ��������
  ISwapChain*                 swap_chain;            //������� ������� ������
  HDC                         device_context;        //�������� ���������� ������� ������� ������
  bool                        vsync;                 //�������� �� ������������ �������������
  const WglExtensionEntries*  wgl_extension_entries; //������� WGL-����������
  xtl::trackable::slot_type   on_destroy_swap_chain; //���������� �������� ������� ������
  ListenerArray               listeners;             //��������� ������� ���������

  Impl () : context (0), pixel_format (0), swap_chain (0), device_context (0), vsync (false), wgl_extension_entries (0),
    on_destroy_swap_chain (xtl::bind (&Impl::OnDestroySwapChain, this)) {}

///��������� ������������ �������������
  void SetVSync ()
  {
    if (wgl_extension_entries->SwapIntervalEXT)
      wgl_extension_entries->SwapIntervalEXT (vsync);
  }

///���������� �������� ������� ������
  void OnDestroySwapChain ()
  {
    swap_chain            = 0;
    device_context        = 0;
    vsync                 = false;
    wgl_extension_entries = 0;
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

///���������� ������� ������ �������� ���������
  void OnLostCurrent ()
  {
    adapter->SetContextListener (0);

    LostCurrentNotify ();
  }  
};

/*
    ����������� / ����������
*/

Context::Context (ISwapChain* in_swap_chain)
  : impl (new Impl)
{
  try
  {
      //�������� ������������ ����������
      
    if (!in_swap_chain)
      throw xtl::make_null_argument_exception ("", "swap_chain");      

    ISwapChainImpl* swap_chain = cast_object<ISwapChainImpl> (in_swap_chain, "", "swap_chain");
    
      //������������� ��������
      
    impl->adapter = swap_chain->GetAdapterImpl ();
    
      //��������� ��������� ���������� ���������

    HDC dc = swap_chain->GetDC ();    

      //��������� ������� ��������

    impl->pixel_format = swap_chain->GetPixelFormat ();

    if (!impl->pixel_format)
      throw xtl::format_operation_exception ("", "Null pixel format");

      //�������� ���������

    impl->context = impl->adapter->CreateContext (dc);

    if (!impl->context)
      raise_error ("wglCreateContext");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::windows::Context::Context");
    throw;
  }
}

Context::~Context ()
{
  try
  {
      //������ �������� ���������
    
    if (impl->adapter->GetCurrentContext () == impl->context)
      impl->adapter->MakeCurrent (0, 0);

      //�������� ���������

    impl->adapter->DeleteContext (impl->context);
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

      impl->swap_chain            = swap_chain;
      impl->device_context        = casted_swap_chain->GetDC ();
      impl->vsync                 = casted_swap_chain->HasVSync ();
      impl->wgl_extension_entries = &casted_swap_chain->GetWglExtensionEntries ();

        //�������� �� ������� �������� ������� ������

      casted_swap_chain->RegisterDestroyHandler (impl->on_destroy_swap_chain);
    }

      //��������� �������� ���������

    impl->adapter->MakeCurrent (impl->device_context, impl->context);

      //��������� ������������ �������������

    impl->SetVSync ();

      //�������� �� ������� ������ ���������

    impl->adapter->SetContextListener (&*impl);

      //���������� �� ��������� �������� ���������

    impl->SetCurrentNotify ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::windows::Context::MakeCurrent");
    throw;
  }
}

/*
   �������� ������������� ������� ������ � ����������
*/

bool Context::IsCompatible (ISwapChain* in_swap_chain)
{
  if (!in_swap_chain)
    return false;

  ISwapChainImpl* swap_chain = dynamic_cast<ISwapChainImpl*> (in_swap_chain);

  if (!swap_chain)
    return false;

  return swap_chain->GetPixelFormat () == impl->pixel_format;
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
