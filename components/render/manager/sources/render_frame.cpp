#include "shared.h"

using namespace render;
using namespace render::low_level;

namespace
{

/*
    ���������
*/

const size_t RESERVED_ENTITIES_COUNT = 512; //������������� ���������� ������������ ��������
const size_t RESERVED_FRAMES_COUNT   = 32;  //������������� ���������� ������������ �������

}

/*
    �������� ���������� �����
*/

typedef stl::vector<FramePtr>  FrameArray;
typedef stl::vector<EntityPtr> EntityArray;

struct FrameImpl::Impl
{
  EntityArray entities;  //������ ��������, ������������ � �����
  FrameArray  frames;    //������ ��������� ������

///�����������
  Impl ()
  {
    entities.reserve (RESERVED_ENTITIES_COUNT);
    frames.reserve (RESERVED_FRAMES_COUNT);
  }
};

/*
   ����������� / ����������
*/

FrameImpl::FrameImpl ()
  : impl (new Impl)
{
}

FrameImpl::~FrameImpl ()
{
}

/*
    ����������� ������� ������� ���������
*/

void FrameImpl::SetRenderTarget (const char* name, const RenderTargetPtr& target)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void FrameImpl::SetRenderTarget (const char* name, const RenderTargetPtr& target, const RectAreaPtr& viewport)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void FrameImpl::SetRenderTarget (const char* name, const RenderTargetPtr& target, const RectAreaPtr& viewport, const RectAreaPtr& scissor)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void FrameImpl::RemoveRenderTarget (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void FrameImpl::RemoveAllRenderTargets ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ��������� ������� ������� ���������
*/

RenderTargetPtr FrameImpl::FindRenderTarget (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

RectAreaPtr FrameImpl::FindViewport (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

RectAreaPtr FrameImpl::FindScissor (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

RenderTargetPtr FrameImpl::RenderTarget (const char* name)
{
  static const char* METHOD_NAME = "render::FrameImpl::RenderTarget";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  RenderTargetPtr target = FindRenderTarget (name);
  
  if (target)
    return target;
    
  throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Render target not found");
}

RectAreaPtr FrameImpl::Viewport (const char* name)
{
  static const char* METHOD_NAME = "render::FrameImpl::Viewport";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  RectAreaPtr area = FindViewport (name);
  
  if (area)
    return area;
    
  throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Viewport not found");
}

RectAreaPtr FrameImpl::Scissor (const char* name)
{
  static const char* METHOD_NAME = "render::FrameImpl::Scissor";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  RectAreaPtr area = FindScissor (name);
  
  if (area)
    return area;
    
  throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Scissor not found");
}

/*
    ������� ���������
*/

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
    ��������� ��������
*/

void FrameImpl::SetLocalTexture (const char* name, const TexturePtr& texture)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void FrameImpl::RemoveLocalTexture (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void FrameImpl::RemoveAllLocalTextures ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ��������� ��������� ��������
*/

TexturePtr FrameImpl::FindLocalTexture (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

TexturePtr FrameImpl::LocalTexture (const char* name)
{
  static const char* METHOD_NAME = "render::FrameImpl::LocalTexture";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  TexturePtr texture = FindLocalTexture (name);
  
  if (texture)
    return texture;
    
  throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Local texture not found");

}

/*
    ��������� ������� ����������
*/

void FrameImpl::SetEffect (const char* name)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::SetEffect");
}

const char* FrameImpl::Effect ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::Effect");
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
  return impl->entities.size ();
}

void FrameImpl::AddEntity (const EntityPtr& entity)
{
  impl->entities.push_back (entity);
}

void FrameImpl::RemoveAllEntities ()
{
  impl->entities.clear ();
}

/*
    ���������� ���-���������� � ����-����������
*/

size_t FrameImpl::FramesCount ()
{
  return impl->frames.size ();
}

void FrameImpl::AddFrame (const FramePtr& frame)
{
  impl->frames.push_back (frame);
}

void FrameImpl::RemoveAllFrames ()
{
  impl->frames.clear ();
}

/*
    ��������� �����
*/

void FrameImpl::Draw ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::Draw");
}
