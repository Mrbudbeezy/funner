#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

typedef Singleton<Driver> OpenGLDriverSingleton;

/*
    ����������� / ����������
*/

Driver::Driver ()
{
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
    ������������ ��������� ��������� ������
*/

size_t Driver::GetOutputsCount ()
{
  return output_manager.GetOutputsCount ();
}

IOutput* Driver::GetOutput (size_t index)
{
  return output_manager.GetOutput (index);
}

/*
    �������� ������� ������
*/

ISwapChain* Driver::CreateSwapChain (const SwapChainDesc& desc)
{
  try
  {
    return create_swap_chain (output_manager, desc);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::Driver::CreateSwapChain(const SwapChainDesc&)");
    throw;
  }
}

/*
    �������� ���������� ���������
*/

IDevice* Driver::CreateDevice (ISwapChain* swap_chain, const char* init_string)
{
  if (!swap_chain)
    RaiseNullArgument ("render::low_level::opengl::Driver::CreateDevice", "swap_chain");
    
  if (!init_string)
    init_string = "";
  
  try
  {
    return new Device (swap_chain, init_string);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::Driver::CreateDevice");
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

const Driver::LogFunction& Driver::GetDebugLog ()
{
  return log_fn;
}

/*
    ��������� ��������
*/

namespace render
{

namespace low_level
{

IDriver* get_opengl_driver ()
{
  return OpenGLDriverSingleton::InstancePtr ();
}

}

}
