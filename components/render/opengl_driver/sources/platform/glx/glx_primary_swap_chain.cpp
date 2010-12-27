#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

/*
    �������� ���������� ��������� ������� ������
*/

typedef xtl::com_ptr<Adapter> AdapterPtr;
typedef Output::Pointer       OutputPtr;

struct PrimarySwapChain::Impl
{
  Log               log;         //��������
  AdapterPtr        adapter;     //�������, �������� ����������� ����������
  AdapterLibraryPtr library;     //���������� ��������
  Display*          display;     //���������� � ��������
  Window            window;      //����
  SwapChainDesc     desc;        //���������� ������� ������
  PropertyList      properties;  //�������� ������� ������

///�����������
  Impl (Adapter* in_adapter, const SwapChainDesc& in_desc)
    : adapter (in_adapter)
    , library (in_adapter->GetLibrary ())
    , display ((Display*)syslib::x11::DisplayManager::DisplayHandle ())
    , window ((Window) in_desc.window_handle)
    , desc (in_desc)
  {
    try
    {
    }
    catch (...)
    {
      throw;
    }
  }
  
///����������
  ~Impl ()
  {

  }  
};

/*
    ����������� / ����������
*/

PrimarySwapChain::PrimarySwapChain (Adapter* adapter, const SwapChainDesc& desc)
{
  try
  {
    if (!adapter)
      throw xtl::make_null_argument_exception ("", "adapter");
    
    impl = new Impl (adapter, desc);        

    impl->log.Printf ("...swap chain successfully created");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::PrimarySwapChain::PrimarySwapChain");
    throw;
  }
}

PrimarySwapChain::~PrimarySwapChain ()
{
}

/*
    ��������� ��������
*/

IAdapter* PrimarySwapChain::GetAdapter ()
{
  return impl->adapter.get ();
}

/*
    ��������� �����������
*/

void PrimarySwapChain::GetDesc (SwapChainDesc& out_desc)
{
  out_desc = impl->desc;
}

/*
    ��������� ���������� ������ � ������������ �������� ������� ����������
*/

IOutput* PrimarySwapChain::GetContainingOutput ()
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::PrimarySwapChain::GetContainingOutput");
}

/*
    ��������� / ������ ��������� full-screen mode
*/

void PrimarySwapChain::SetFullscreenState (bool state)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::PrimarySwapChain::SetFullscreenState");
}

bool PrimarySwapChain::GetFullscreenState ()
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::PrimarySwapChain::GetFullscreenState");
}

/*
    ����� �������� ������� ������ � ��������� ������
*/

void PrimarySwapChain::Present ()
{
  try
  {
    impl->library->SwapBuffers (impl->display, impl->window);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::PrimarySwapChain::Present");
    throw;
  }
}

/*
    ������ ������� ���������� ������
*/

IPropertyList* PrimarySwapChain::GetProperties ()
{
  return &impl->properties;
}
