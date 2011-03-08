#include "shared.h"

using namespace render;
using namespace render::low_level;

/*
    �������� ���������� �����
*/

struct FrameImpl::Impl
{
};

/*
   ����������� / ����������
*/

FrameImpl::FrameImpl ()
  : impl (new Impl)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::FrameImpl");
}

FrameImpl::~FrameImpl ()
{
}

/*
    ������� ������ ���������
*/

void FrameImpl::SetRenderTargets (const RenderTargetPtr& render_target, const RenderTargetPtr& depth_stencil_target)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::SetRenderTargets");
}

const RenderTargetPtr& FrameImpl::RenderTarget ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::RenderTarget");
}

const RenderTargetPtr& FrameImpl::DepthStencilTarget ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::DepthStencilTarget");
}

/*
    ������� ������
*/

void FrameImpl::SetViewport (const render::Rect&)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::SetViewport");
}

const render::Rect& FrameImpl::Viewport ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::Viewport");
}

/*
    ������� ���������
*/

void FrameImpl::SetScissor (const render::Rect&)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::SetScissor");
}

const render::Rect& FrameImpl::Scissor ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::Scissor");
}

void FrameImpl::SetScissorState (bool state)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::SetScissorState");
}

bool FrameImpl::ScissorState ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::ScissorState");
}

/*
    ����� ������� �����
*/

void FrameImpl::SetClearFlags (size_t clear_flags)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::SetClearFlags");
}

size_t FrameImpl::ClearFlags ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::ClearFlags");
}

/*
    ��������� ������� ������ �����
*/

void FrameImpl::SetClearColor (const math::vec4f& color)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::SetClearColor");
}

const math::vec4f& FrameImpl::ClearColor ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::ClearColor");
}

/*
    ��������� ������� ������ ������������� ���������
*/

void FrameImpl::SetClearDepthValue (float depth_value)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::SetClearDepthValue");
}

void FrameImpl::SetClearStencilIndex (unsigned char stencil_index)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::SetClearStencilIndex");
}

float FrameImpl::ClearDepthValue ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::ClearDepthValue");
}

unsigned char FrameImpl::ClearStencilIndex ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::ClearStencilIndex");
}

/*
    ������� ���� / ������� ��������������
*/

void FrameImpl::SetViewMatrix (const math::mat4f& tm)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::SetViewMatrix");
}

void FrameImpl::SetProjectionMatrix (const math::mat4f& tm)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::SetProjectionMatrix");
}

const math::mat4f& FrameImpl::ViewMatrix ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::ViewMatrix");
}

const math::mat4f& FrameImpl::ProjectionMatrix ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::ProjectionMatrix");
}

/*
    ��������� ������� ����������
*/

void FrameImpl::SetTechnique (const char* name)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::SetTechnique");
}

const char* FrameImpl::Technique ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::Technique");
}

/*
    �������� ����������
*/

void FrameImpl::SetProperties (const common::PropertyMap& properties)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::SetProperties");
}

const common::PropertyMap& FrameImpl::Properties ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::Properties");
}

/*
    ������ ���������
*/

size_t FrameImpl::EntitiesCount ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::EntitiesCount");
}

void FrameImpl::AddEntity (const EntityPtr& entity)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::AddEntity");
}

void FrameImpl::RemoveAllEntities ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::RemoveAllEntities");
}

/*
    ���������� ���-���������� � ����-����������
*/

void FrameImpl::AddFrame (FrameOrder order, const FramePtr& frame)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::AddFrame");
}

void FrameImpl::RemoveAllFrames ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::RemoveAllFrames");
}

/*
    �������� ������ � ���������� �� � �������
*/

FramePtr FrameImpl::AddPreRenderFrame ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::AddPreRenderFrame");
}

FramePtr FrameImpl::AddChildFrame ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::AddChildFrame");
}

FramePtr FrameImpl::AddPostRenderFrame ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::AddPostRenderFrame");  
}

/*
    �������� ���� �������� �� �����
*/

void FrameImpl::RemoveAll ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::RemoveAll");
}

/*
    ��������� �����
*/

void FrameImpl::Draw ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::Draw");
}
