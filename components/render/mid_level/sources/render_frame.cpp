#include "shared.h"

using namespace render::mid_level;
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
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::FrameImpl");
}

FrameImpl::~FrameImpl ()
{
}

/*
    ������� ������ ���������
*/

void FrameImpl::SetRenderTargets (const RenderTargetPtr& render_target, const RenderTargetPtr& depth_stencil_target)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::SetRenderTargets");
}

const RenderTargetPtr& FrameImpl::RenderTarget ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::RenderTarget");
}

const RenderTargetPtr& FrameImpl::DepthStencilTarget ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::DepthStencilTarget");
}

/*
    ������� ������
*/

void FrameImpl::SetViewport (const Rect&)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::SetViewport");
}

const Rect& FrameImpl::Viewport ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::Viewport");
}

/*
    ������� ���������
*/

void FrameImpl::SetScissor (const Rect&)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::SetScissor");
}

const Rect& FrameImpl::Scissor ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::Scissor");
}

void FrameImpl::SetScissorState (bool state)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::SetScissorState");
}

bool FrameImpl::ScissorState ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::ScissorState");
}

/*
    ����� ������� �����
*/

void FrameImpl::SetClearFlags (size_t clear_flags)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::SetClearFlags");
}

size_t FrameImpl::ClearFlags ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::ClearFlags");
}

/*
    ��������� ������� ������ �����
*/

void FrameImpl::SetClearColor (const math::vec4f& color)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::SetClearColor");
}

const math::vec4f& FrameImpl::ClearColor ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::ClearColor");
}

/*
    ��������� ������� ������ ������������� ���������
*/

void FrameImpl::SetClearDepthValue (float depth_value)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::SetClearDepthValue");
}

void FrameImpl::SetClearStencilIndex (unsigned char stencil_index)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::SetClearStencilIndex");
}

float FrameImpl::ClearDepthValue ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::ClearDepthValue");
}

unsigned char FrameImpl::ClearStencilIndex ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::ClearStencilIndex");
}

/*
    ������� ���� / ������� ��������������
*/

void FrameImpl::SetViewMatrix (const math::mat4f& tm)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::SetViewMatrix");
}

void FrameImpl::SetProjectionMatrix (const math::mat4f& tm)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::SetProjectionMatrix");
}

const math::mat4f& FrameImpl::ViewMatrix ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::ViewMatrix");
}

const math::mat4f& FrameImpl::ProjectionMatrix ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::ProjectionMatrix");
}

/*
    ��������� ������� ����������
*/

void FrameImpl::SetTechnique (const char* name)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::SetTechnique");
}

const char* FrameImpl::Tehcnique ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::Technique");
}

/*
    �������� ����������
*/

void FrameImpl::SetProperties (const common::PropertyMap& properties)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::SetProperties");
}

const common::PropertyMap& FrameImpl::Properties ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::Properties");
}

/*
    ������ ���������
*/

size_t FrameImpl::EntitiesCount ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::EntitiesCount");
}

void FrameImpl::AddEntity (const EntityPtr& entity)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::AddEntity");
}

void FrameImpl::RemoveAllEntities ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::RemoveAllEntities");
}

/*
    ���������� ���-���������� � ����-����������
*/

void FrameImpl::AddFrame (FrameOrder order, const Frame& frame)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::AddFrame");
}

void FrameImpl::RemoveAllFrames ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::RemoveAllFrames");
}

/*
    �������� ������ � ���������� �� � �������
*/

FramePtr FrameImpl::AddPreRenderFrame ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::AddPreRenderFrame");
}

FramePtr FrameImpl::AddChildFrame ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::AddChildFrame");
}

FramePtr FrameImpl::AddPostRenderFrame ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::AddPostRenderFrame");  
}

/*
    �������� ���� �������� �� �����
*/

void FrameImpl::RemoveAll ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::RemoveAll");
}

/*
    ��������� �����
*/

void FrameImpl::Draw ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::FrameImpl::Draw");
}

/*
    ��������� ������
*/

Frame FrameImpl::Wrap ()
{
  return Wrappers::Wrap<Frame> (this);
}
