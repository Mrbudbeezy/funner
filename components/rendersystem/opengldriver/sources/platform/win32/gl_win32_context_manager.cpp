#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    �������� ���������� ContextManager
*/

struct ContextManager::Impl
{

};

/*
    ����������� / ����������
*/

ContextManager::ContextManager ()
  : impl (new Impl)
{
}

ContextManager::~ContextManager ()
{
}

/*
    �������� ������� ������
*/

ISwapChain* ContextManager::CreateSwapChain (IOutput* target_output, const SwapChainDesc& swap_chain_desc)
{
  RaiseNotImplemented ("render::low_level::opengl::ContextManager::CreateSwapChain");
  return 0;
}
