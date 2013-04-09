#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

namespace
{

/*
    ���������
*/

const size_t ADAPTERS_RESERVE_SIZE = 8;

}

/*
    ����������� / ����������  
*/

Driver::Driver ()
{
  try
  {
      //�������� �������

    IDXGIFactory* factory_ptr = 0;

    check_errors ("::CreateDXGIFactory", CreateDXGIFactory (__uuidof (IDXGIFactory), (void**)&factory_ptr));

    if (!factory_ptr)
      throw xtl::format_operation_exception ("", "::CreateDXGIFactory failed");

    factory = factory_ptr;

      //�������������� ����� ��� �������� ���������

    registered_adapters.reserve (ADAPTERS_RESERVE_SIZE);
    adapters.reserve (ADAPTERS_RESERVE_SIZE);

      //������������ ���������

    IDXGIAdapter* dx_adapter = 0;

    for (UINT i=0; factory->EnumAdapters (i, &dx_adapter) != DXGI_ERROR_NOT_FOUND; i++)
    { 
      AdapterPtr adapter (new Adapter (DxAdapterPtr (dx_adapter)), false);

      RegisterAdapter (&*adapter);

      adapters.push_back (adapter);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Driver::Driver");
    throw;
  }
}

Driver::~Driver ()
{
}
  
/*
    �������� ��������
*/

const char* Driver::GetDescription ()
{
  return "render::low_level::dx11::Driver";
}

/*
    ������������ ��������� ���������
*/

size_t Driver::GetAdaptersCount ()
{
  return registered_adapters.size ();
}

IAdapter* Driver::GetAdapter (size_t index)
{
  if (index >= registered_adapters.size ())
    throw xtl::make_range_exception ("render::low_level::dx11::Driver::GetAdapter", "index", index, registered_adapters.size ());

  return registered_adapters [index];
}

/*
    ����������� ���������
*/

void Driver::RegisterAdapter (IAdapter* adapter)
{
  registered_adapters.push_back (adapter);

  try
  {      
    adapter->RegisterDestroyHandler (xtl::bind (&Driver::UnregisterAdapter, this, adapter), GetTrackable ());
  }
  catch (...)
  {
    registered_adapters.pop_back ();
    throw;
  }
}

void Driver::UnregisterAdapter (IAdapter* adapter)
{
  registered_adapters.erase (stl::remove (registered_adapters.begin (), registered_adapters.end (), adapter), registered_adapters.end ());
}

/*
    �������� ��������
*/

IAdapter* Driver::CreateAdapter (const char* name, const char* path, const char* init_string)
{
  try
  {
      //�������� ������������ ����������
      
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    if (!path)
      throw xtl::make_null_argument_exception ("", "path");

      //�������� ��������

    typedef xtl::com_ptr<IAdapter> AdapterPtr;
    
    AdapterPtr adapter (new Adapter (factory, name, path, init_string), false);
    
      //����������� �������� � ��������

    RegisterAdapter (&*adapter);

      //����������� ��������
      
    adapter->AddRef ();

    return &*adapter;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Driver::CreateAdapter");
    throw;
  }
}

/*
    �������� ������� ������
*/

ISwapChain* Driver::CreateSwapChain (size_t prefered_adapters_count, IAdapter** prefered_adapters, const SwapChainDesc& desc)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    �������� ���������� ���������
*/

IDevice* Driver::CreateDevice (ISwapChain* swap_chain, const char* init_string)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ��������� ������� ����������� ���������������� ��������
*/

void Driver::SetDebugLog (const LogFunction& fn)
{
  log_fn = fn;
}

const LogFunction& Driver::GetDebugLog ()
{
  return log_fn;
}
