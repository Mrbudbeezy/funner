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

struct FrameImpl::Impl: public CacheHolder
{
  EffectManagerPtr  effect_manager;  //������ �� �������� ��������
  EffectProxy       effect;          //������ �����
  EffectRendererPtr effect_renderer; //������ �������
  EntityArray       entities;        //������ ��������, ������������ � �����
  FrameArray        frames;          //������ ��������� ������
  EffectPtr         cached_effect;   //�������������� ������

///�����������
  Impl (const EffectManagerPtr& in_effect_manager)
    : effect_manager (in_effect_manager)
    , effect (effect_manager->GetEffectProxy (""))
  {
    entities.reserve (RESERVED_ENTITIES_COUNT);
    frames.reserve (RESERVED_FRAMES_COUNT);
  }
  
///������ � �����
  void ResetCacheCore ()
  {
    cached_effect   = EffectPtr ();
    effect_renderer = EffectRendererPtr ();
  }

  void UpdateCacheCore ()
  {
    try
    {
      cached_effect = effect.Resource ();
      
      if (!cached_effect)
        return;
        
      effect_renderer = EffectRendererPtr (new render::EffectRenderer (cached_effect), false);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::FrameImpl::Impl::UpdateCacheCore");
      throw;
    }
  }
    
  using CacheHolder::UpdateCache;
  using CacheHolder::Invalidate;
};

/*
   ����������� / ����������
*/

FrameImpl::FrameImpl (const EffectManagerPtr& effect_manager)
{
  try
  {
    if (!effect_manager)
      throw xtl::make_null_argument_exception ("", "effect_manager");
    
    impl = new Impl (effect_manager);
    
    impl->effect.AttachCacheHolder (*impl);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::FrameImpl::FrameImpl");
    throw;
  }
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
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
    
    impl->effect = impl->effect_manager->GetEffectProxy (name);
    
    impl->Invalidate ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::FrameImpl::SetEffect");
    throw;
  }
}

const char* FrameImpl::Effect ()
{
  return impl->effect.Name ();
}

/*
    ������ �������
*/

EffectRendererPtr FrameImpl::EffectRenderer ()
{
  try
  {
    impl->UpdateCache ();
    
    return impl->effect_renderer;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::FrameImpl::EffectRenderer");
    throw;
  }
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
