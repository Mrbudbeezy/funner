#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    ���������
*/

namespace
{

const char* DRIVER_NAME    = "OpenGL";                  //��� ��������
const char* COMPONENT_NAME = "render.low_level.opengl"; //��� ����������

}

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
    return SwapChainManager::CreateSwapChain (output_manager, desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Driver::CreateSwapChain(const SwapChainDesc&)");
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
    return new Device (this, swap_chain, init_string);
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
    ����������������
*/

void Driver::LogMessage (const char* message) const
{
  if (!message)
    return;
    
  try
  {
    log_fn (message);
  }
  catch (...)
  {
    //��������� ��� ����������
  }
}

/*
    ��������� ��������
*/

namespace
{

class OpenGLDriverComponent
{
  public:
    OpenGLDriverComponent ()
    {
      DriverManager::RegisterDriver (DRIVER_NAME, xtl::com_ptr<Driver> (new Driver, false).get ());
    }
};

}

extern "C"
{

common::ComponentRegistrator<OpenGLDriverComponent> OpenGLDriver (COMPONENT_NAME);

}
