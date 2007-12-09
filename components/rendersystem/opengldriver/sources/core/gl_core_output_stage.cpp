#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    �������� ���������� ��������� ������ ��������� OpenGL
*/

struct OutputStage::Impl
{
  ContextManager context_manager; //�������� ����������
  
  Impl (ContextManager& in_context_manager) : context_manager (in_context_manager) {}
};

/*
    ����������� / ����������
*/

OutputStage::OutputStage (ContextManager& context_manager)
  : impl (new Impl (context_manager))
{
}

OutputStage::~OutputStage ()
{
}

/*
    �������� ������� �����
*/

IFrameBuffer* OutputStage::CreateFrameBuffer (const FrameBufferDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::CreateFrameBuffer(const FrameBufferDesc&)");
  return 0;
}

IFrameBuffer* OutputStage::CreateFrameBuffer (ISwapChain*)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::CreateFrameBuffer(ISwapChain*)");
  return 0;
}

IFrameBuffer* OutputStage::CreateFrameBuffer (ITexture* render_target)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::CreateFrameBuffer(ITexture*)");
  return 0;
}

/*
    �������� ��������� ��������� ������
*/

IBlendState* OutputStage::CreateBlendState ()
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::CreateBlendState");
  return 0;
}

IDepthStencilState* OutputStage::CreateDepthStencilState ()
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::CreateDepthStencilState");
  return 0;
}

/*
    ������ � ������� �����
*/

void OutputStage::SetFrameBuffer (IFrameBuffer*)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::SetFrameBuffer");
}

IFrameBuffer* OutputStage::GetFrameBuffer () const
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::GetFrameBuffer");
  return 0;
}

/*
    ��������� ��������� ���������� ������
*/

void OutputStage::SetBlendState (IBlendState*)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::SetBlendState");
}

IBlendState* OutputStage::GetBlendState () const
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::GetBlendState");
  return 0;
}

/*
    ��������� ��������� ������������� ���������
*/

void OutputStage::SetDepthStencil (IDepthStencilState* state)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::SetDepthStencil");
}

void OutputStage::SetStencilReference (size_t reference)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::SetStencilReference");
}

IDepthStencilState* OutputStage::GetDepthStencilState () const
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::GetDepthStencilState");
  return 0;
}

size_t OutputStage::GetStencilReference () const
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::GetDepthStencilReference");
  return 0;
}
