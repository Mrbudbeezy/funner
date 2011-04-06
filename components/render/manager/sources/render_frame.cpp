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

/*
    ��������������� ���������
*/

struct RenderTargetDesc: public xtl::reference_counter
{
  RenderTargetPtr render_target; //������� ����� ���������
  RectAreaPtr     viewport;      //������� ������
  RectAreaPtr     scissor;       //������� ���������
  
  RenderTargetDesc (const RenderTargetPtr& in_render_target, const RectAreaPtr& in_viewport, const RectAreaPtr& in_scissor)
    : render_target (in_render_target)
    , viewport (in_viewport)
    , scissor (in_scissor)
  {
  }
};

typedef xtl::intrusive_ptr<RenderTargetDesc>                           RenderTargetDescPtr;
typedef stl::hash_map<stl::hash_key<const char*>, RenderTargetDescPtr> RenderTargetDescMap;
typedef stl::hash_map<stl::hash_key<const char*>, TexturePtr>          TextureMap;

}

/*
    �������� ���������� �����
*/

typedef stl::vector<FramePtr>  FrameArray;
typedef stl::vector<EntityPtr> EntityArray;

struct FrameImpl::Impl: public CacheHolder
{
  EffectManagerPtr    effect_manager;      //������ �� �������� ��������
  EffectProxy         effect;              //������ �����
  EffectRendererPtr   effect_renderer;     //������ �������
  EntityArray         entities;            //������ ��������, ������������ � �����
  FrameArray          frames;              //������ ��������� ������
  RenderTargetDescMap render_targets;      //������� ������ ���������
  TextureMap          textures;            //��������� �������� ������
  bool                scissor_state;       //�������� �� ���������
  size_t              clear_flags;         //����� �������
  math::vec4f         clear_color;         //���� ������� ����
  float               clear_depth_value;   //�������� ������ ������� ����� �������
  unsigned char       clear_stencil_index; //�������� ������ ��������� ����� �������
  EffectPtr           cached_effect;       //�������������� ������

///�����������
  Impl (const EffectManagerPtr& in_effect_manager)
    : effect_manager (in_effect_manager)
    , effect (effect_manager->GetEffectProxy (""))
    , scissor_state (false)
    , clear_flags (ClearFlag_All)
    , clear_depth_value (1.0f)
    , clear_stencil_index (0)
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
  SetRenderTarget (name, target, RectAreaPtr (), RectAreaPtr ());
}

void FrameImpl::SetRenderTarget (const char* name, const RenderTargetPtr& target, const RectAreaPtr& viewport)
{
  SetRenderTarget (name, target, viewport, RectAreaPtr ());
}

void FrameImpl::SetRenderTarget (const char* name, const RenderTargetPtr& target, const RectAreaPtr& viewport, const RectAreaPtr& scissor)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    if (!target)
      throw xtl::make_null_argument_exception ("", "target");
      
    RenderTargetDescMap::iterator iter = impl->render_targets.find (name);
    
    if (iter != impl->render_targets.end ())
      throw xtl::make_argument_exception ("", "name", name, "Render target has already registered");
      
    impl->render_targets.insert_pair (name, RenderTargetDescPtr (new RenderTargetDesc (target, viewport, scissor), false));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::FrameImpl::SetRenderTarget(const char*,const RenderTargetPtr&,const RectAreaPtr&,const RectAreaPtr&)");
    throw;
  }
}

void FrameImpl::RemoveRenderTarget (const char* name)
{
  if (!name)
    return;
    
  impl->render_targets.erase (name);
}

void FrameImpl::RemoveAllRenderTargets ()
{
  impl->render_targets.clear ();
}

/*
    ��������� ������� ������� ���������
*/

RenderTargetPtr FrameImpl::FindRenderTarget (const char* name)
{
  if (!name)
    return RenderTargetPtr ();

  RenderTargetDescMap::iterator iter = impl->render_targets.find (name);

  if (iter == impl->render_targets.end ())
    return RenderTargetPtr ();
    
  return iter->second->render_target;
}

RectAreaPtr FrameImpl::FindViewport (const char* name)
{
  if (!name)
    return RectAreaPtr ();

  RenderTargetDescMap::iterator iter = impl->render_targets.find (name);

  if (iter == impl->render_targets.end ())
    return RectAreaPtr ();
    
  return iter->second->viewport;
}

RectAreaPtr FrameImpl::FindScissor (const char* name)
{
  if (!name)
    return RectAreaPtr ();

  RenderTargetDescMap::iterator iter = impl->render_targets.find (name);

  if (iter == impl->render_targets.end ())
    return RectAreaPtr ();
    
  return iter->second->scissor;
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
  impl->scissor_state = state;
}

bool FrameImpl::ScissorState ()
{
  return impl->scissor_state;
}

/*
    ����� ������� �����
*/

void FrameImpl::SetClearFlags (size_t clear_flags)
{
  impl->clear_flags = clear_flags;
}

size_t FrameImpl::ClearFlags ()
{
  return impl->clear_flags;
}

/*
    ��������� ������� ������ �����
*/

void FrameImpl::SetClearColor (const math::vec4f& color)
{
  impl->clear_color = color;
}

const math::vec4f& FrameImpl::ClearColor ()
{
  return impl->clear_color;
}

/*
    ��������� ������� ������ ������������� ���������
*/

void FrameImpl::SetClearDepthValue (float depth_value)
{
  impl->clear_depth_value = depth_value;
}

void FrameImpl::SetClearStencilIndex (unsigned char stencil_index)
{
  impl->clear_stencil_index = stencil_index;
}

float FrameImpl::ClearDepthValue ()
{
  return impl->clear_depth_value;
}

unsigned char FrameImpl::ClearStencilIndex ()
{
  return impl->clear_stencil_index;
}

/*
    ��������� ��������
*/

void FrameImpl::SetLocalTexture (const char* name, const TexturePtr& texture)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    if (!texture)
      throw xtl::make_null_argument_exception ("", "texture");
      
    TextureMap::iterator iter = impl->textures.find (name);
    
    if (iter != impl->textures.end ())
      throw xtl::make_argument_exception ("", "name", name, "Frame local texture has already registered");
      
    impl->textures.insert_pair (name, texture);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::FrameImpl::SetLocalTexture");
    throw;
  }
}

void FrameImpl::RemoveLocalTexture (const char* name)
{
  if (!name)
    return;
    
  impl->textures.erase (name);
}

void FrameImpl::RemoveAllLocalTextures ()
{
  impl->textures.clear ();
}

/*
    ��������� ��������� ��������
*/

TexturePtr FrameImpl::FindLocalTexture (const char* name)
{
  if (!name)
    return TexturePtr ();
    
  TextureMap::iterator iter = impl->textures.find (name);
  
  if (iter == impl->textures.end ())
    return TexturePtr ();
    
  return iter->second;
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
