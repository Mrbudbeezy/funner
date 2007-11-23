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

ISwapChain* ContextManager::CreateSwapChain (OutputManager& output_manager, const SwapChainDesc& swap_chain_desc)
{
  return new SwapChain (output_manager, swap_chain_desc);
}

