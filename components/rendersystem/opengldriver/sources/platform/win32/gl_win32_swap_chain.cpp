#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    ����������� / ����������
*/

SwapChain::SwapChain ()
{
}

SwapChain::~SwapChain ()
{
}

/*
    ��������� �����������
*/

void SwapChain::GetDesc (SwapChainDesc& out_desc)
{
  out_desc = desc;
}

/*
    ��������� ���������� ������
*/

IOutput* SwapChain::GetOutput ()
{
  return output;
}

/*
    ����� �������� ������� ������ � ��������� ������
*/

void SwapChain::Present ()
{
  if (!SwapBuffers (hDC))
    raise_error (format ("render::low_level::opengl::SwapChain::Present(device-name='%s')", output->GetName ()).c_str ());
}

/*
    ��������� / ������ ��������� full-screen mode
*/

void SwapChain::SetFullscreenState (bool state)
{
  RaiseNotImplemented ("render::low_level::opengl::SwapChain::SetFullscreenState");
}

bool SwapChain::GetFullscreenState ()
{
  RaiseNotImplemented ("render::low_level::opengl::SwapChain::SetFullscreenState");
  return false;
}
