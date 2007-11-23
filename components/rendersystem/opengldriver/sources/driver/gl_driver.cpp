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
  return context_manager.CreateSwapChain (output_manager, desc);
}

/*
    �������� ���������� ���������
*/

IDevice* Driver::CreateDevice (ISwapChain* swap_chain, const char* init_string)
{
  RaiseNotImplemented ("render::low_level::opengl::Driver::CreateSwapChain");
  return 0;
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
