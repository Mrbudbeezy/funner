#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    ����������� / ����������
*/

SwapChain::SwapChain (const SwapChainDesc& desc)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

SwapChain::~SwapChain ()
{
}

/*
    ��������� ��������
*/

IAdapter* SwapChain::GetAdapter ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ��������� �����������
*/

void SwapChain::GetDesc (SwapChainDesc&)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ��������� ���������� ������ � ������������ �������� ������� ����������
*/

IOutput* SwapChain::GetContainingOutput ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ��������� / ������ ��������� full-screen mode
*/

void SwapChain::SetFullscreenState (bool state)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

bool SwapChain::GetFullscreenState ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ����� �������� ������� ������ � ��������� ������
*/

void SwapChain::Present ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ������ ������� ������� ������
*/

IPropertyList* SwapChain::GetProperties ()
{
  return &properties;
}
