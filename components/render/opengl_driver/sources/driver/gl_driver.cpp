#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

namespace
{

/*
    ���������
*/

const char*  DRIVER_NAME                = "OpenGL";                  //��� ��������
const char*  COMPONENT_NAME             = "render.low_level.opengl"; //��� ����������
const size_t ADAPTER_ARRAY_RESERVE_SIZE = 4;                         //������������� ���������� ��������� ���������

}

/*
    ����������� / ����������
*/

Driver::Driver ()
{
  try
  {
      //�������������� ����� ��� ��������

    adapters.reserve (ADAPTER_ARRAY_RESERVE_SIZE);

      //����������� ��������� "�� ���������"

    PlatformManager::EnumDefaultAdapters (xtl::bind (&Driver::RegisterAdapter, this, _1));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Driver::Driver");
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
  return "render::low_level::opengl::Driver";
}

/*
    ������������ ��������� ���������
*/

size_t Driver::GetAdaptersCount ()
{
  return adapters.size ();
}

IAdapter* Driver::GetAdapter (size_t index)
{
  if (index >= adapters.size ())
    throw xtl::make_range_exception ("render::low_level::opengl::Driver::GetAdapter", "index", index, adapters.size ());
    
  return adapters [index];
}

/*
    ����������� ���������
*/

void Driver::RegisterAdapter (IAdapter* adapter)
{
  adapters.push_back (adapter);

  try
  {      
    adapter->RegisterDestroyHandler (xtl::bind (&Driver::UnregisterAdapter, this, adapter), GetTrackable ());
  }
  catch (...)
  {
    adapters.pop_back ();
    throw;
  }
}

void Driver::UnregisterAdapter (IAdapter* adapter)
{
  adapters.erase (stl::remove (adapters.begin (), adapters.end (), adapter), adapters.end ());
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
    
    AdapterPtr adapter (PlatformManager::CreateAdapter (name, path, init_string), false);
    
      //����������� �������� � ��������

    RegisterAdapter (&*adapter);

      //����������� ��������
      
    adapter->AddRef ();

    return &*adapter;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Driver::CreateAdapter");
    throw;
  }
}

/*
    �������� ������� ������
*/

ISwapChain* Driver::CreateSwapChain (size_t prefered_adapters_count, IAdapter** prefered_adapters, const SwapChainDesc& swap_chain_desc)
{
  try
  {
    return PlatformManager::CreateSwapChain (prefered_adapters_count, prefered_adapters, swap_chain_desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Driver::CreateSwapChain");
    throw;
  }
}

/*
    �������� ���������� ���������
*/

IDevice* Driver::CreateDevice (ISwapChain* swap_chain, const char* init_string)
{
  if (!swap_chain)
    throw xtl::make_null_argument_exception ("render::low_level::opengl::Driver::CreateDevice", "swap_chain");
    
  if (!init_string)
    init_string = "";
  
  try
  {
    return new Device (swap_chain, init_string);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Driver::CreateDevice");
    throw;
  }
}

/*
    ��������� ������� ����������� ���������������� ��������
*/

void Driver::SetDebugLog (const LogFunction& in_log_fn)
{
  log_fn = in_log_fn;
}

const LogFunction& Driver::GetDebugLog ()
{
  return log_fn;
}

/*
    ��������� ��������
*/

namespace components
{

namespace opengl_driver
{

class OpenGLDriverComponent
{
  public:
    OpenGLDriverComponent ()
    {
      DriverManager::RegisterDriver (DRIVER_NAME, xtl::com_ptr<Driver> (new Driver, false).get ());
    }
};

extern "C"
{

common::ComponentRegistrator<OpenGLDriverComponent> OpenGLDriver (COMPONENT_NAME);

}

}

}
