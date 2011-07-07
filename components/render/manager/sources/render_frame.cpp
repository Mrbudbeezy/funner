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

typedef stl::hash_map<stl::hash_key<const char*>, RenderTargetDescPtr> RenderTargetDescMap;
typedef stl::hash_map<stl::hash_key<const char*>, TexturePtr>          TextureMap;

/*
    ��������� ��� �������
*/

struct EffectHolder: public CacheSource
{
  DeviceManagerPtr         device_manager;         //�������� ���������� ���������
  EffectManagerPtr         effect_manager;         //������ �� �������� ��������
  EffectProxy              effect;                 //������ �����
  EffectRendererPtr        effect_renderer;        //������ �������
  EffectPtr                cached_effect;          //�������������� ������
  ProgramParametersLayout  properties_layout;      //������������ �������
  
///�����������
  EffectHolder (const EffectManagerPtr& in_effect_manager, const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)
    , effect_manager (in_effect_manager)
    , effect (effect_manager->GetEffectProxy (""))
    , properties_layout (&device_manager->Device (), &device_manager->Settings ())
  {
    effect.AttachCacheHolder (*this);
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
        
      effect_renderer = EffectRendererPtr (new render::EffectRenderer (cached_effect, device_manager, &properties_layout), false);      
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::EffectHolder::UpdateCacheCore");
      throw;
    }
  }
    
  using CacheHolder::UpdateCache;
  using CacheHolder::ResetCache;
};

/*
    �������� ������� ����������
*/

struct EntityDesc
{
  EntityPtr                  entity;          //������
  LowLevelBufferPtr          property_buffer; //����� ������������ ������� �������
  ProgramParametersLayoutPtr layout;          //������������ ������������ ������� �������
  
  EntityDesc (const EntityPtr& in_entity)
    : entity (in_entity)
  {
  }
};

}

/*
    �������� ���������� �����
*/

typedef stl::vector<FramePtr>   FrameArray;
typedef stl::vector<EntityDesc> EntityArray;

struct FrameImpl::Impl: public CacheHolder
{
  stl::auto_ptr<EffectHolder> effect_holder;           //��������� �������
  EntityArray                 entities;                //������ ��������, ������������ � �����
  FrameArray                  frames;                  //������ ��������� ������
  PropertyBuffer              properties;              //�������� �����
  PropertyCache               entities_properties;     //������������ �������� ��������
  render::ShaderOptionsCache  shader_options_cache;    //��� ����� �������
  RenderTargetDescMap         render_targets;          //������� ������ ���������
  TextureMap                  textures;                //��������� �������� ������
  bool                        scissor_state;           //�������� �� ���������  
  size_t                      clear_flags;             //����� �������
  math::vec4f                 clear_color;             //���� ������� ����
  float                       clear_depth_value;       //�������� ������ ������� ����� �������
  unsigned char               clear_stencil_index;     //�������� ������ ��������� ����� �������
  LowLevelBufferPtr           cached_properties;       //�������������� ��������
  EntityDrawFunction          entity_draw_handler;     //���������� ��������� ��������
  EntityDrawParams            entity_draw_params;      //��������� ��������� �������� (���)

///�����������
  Impl (const DeviceManagerPtr& device_manager, const EffectManagerPtr& effect_manager)
    : effect_holder (new EffectHolder (effect_manager, device_manager))
    , properties (device_manager)
    , entities_properties (device_manager)
    , scissor_state (false)
    , clear_flags (ClearFlag_All)
    , clear_depth_value (1.0f)
    , clear_stencil_index (0)
  {
    AttachCacheSource (*effect_holder);
    AttachCacheSource (properties);
    
    entities.reserve (RESERVED_ENTITIES_COUNT);
    frames.reserve (RESERVED_FRAMES_COUNT);
  }
  
///��������� ������ ������ ����������� �� ����������
  size_t GetLod (double distance)
  {
    return size_t (log (distance));
  }
  
///������ � �����
  void ResetCacheCore ()
  {
    cached_properties = LowLevelBufferPtr ();
  }

  void UpdateCacheCore ()
  {
    try
    {
      effect_holder->UpdateCache ();
      
      cached_properties = properties.Buffer ();
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::FrameImpl::Impl::UpdateCacheCore");
      throw;
    }
  }
    
  using CacheHolder::UpdateCache;
  using CacheHolder::ResetCache;
};

/*
   ����������� / ����������
*/

FrameImpl::FrameImpl (const DeviceManagerPtr& device_manager, const EffectManagerPtr& effect_manager)
{
  try
  {
    if (!effect_manager)
      throw xtl::make_null_argument_exception ("", "effect_manager");
    
    if (!device_manager)
      throw xtl::make_null_argument_exception ("", "device_manager");
    
    impl = new Impl (device_manager, effect_manager);    
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

    impl->render_targets.insert_pair (name, RenderTargetDescPtr (new render::RenderTargetDesc (target, viewport, scissor), false));
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
  RenderTargetDescPtr desc = FindRenderTargetDesc (name);
  
  if (!desc)
    return RenderTargetPtr ();
  
  return desc->render_target;
}

RectAreaPtr FrameImpl::FindViewport (const char* name)
{
  RenderTargetDescPtr desc = FindRenderTargetDesc (name);
  
  if (!desc)
    return RectAreaPtr ();
    
  return desc->viewport;
}

RectAreaPtr FrameImpl::FindScissor (const char* name)
{
  RenderTargetDescPtr desc = FindRenderTargetDesc (name);

  if (!desc)
    return RectAreaPtr ();

  return desc->scissor;
}

RenderTargetDescPtr FrameImpl::FindRenderTargetDesc (const char* name)
{
  if (!name)
    return RenderTargetDescPtr ();

  RenderTargetDescMap::iterator iter = impl->render_targets.find (name);

  if (iter == impl->render_targets.end ())
    return RenderTargetDescPtr ();
    
  return iter->second;
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

RenderTargetDescPtr FrameImpl::RenderTargetDesc (const char* name)
{
  static const char* METHOD_NAME = "render::FrameImpl::RenderTargetDesc";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  RenderTargetDescPtr desc = FindRenderTargetDesc (name);
  
  if (desc)
    return desc;
    
  throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Render target not found");
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
    
    impl->effect_holder->effect = impl->effect_holder->effect_manager->GetEffectProxy (name);    
    
    impl->effect_holder->ResetCache ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::FrameImpl::SetEffect");
    throw;
  }
}

const char* FrameImpl::Effect ()
{
  return impl->effect_holder->effect.Name ();
}

/*
    ������ �������
*/

EffectRendererPtr FrameImpl::EffectRenderer ()
{
  try
  {
    impl->UpdateCache ();
    
    return impl->effect_holder->effect_renderer;
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
  try
  {
    impl->properties.SetProperties (properties);

    if (impl->effect_holder)
      impl->effect_holder->properties_layout.AttachSlot (ProgramParametersSlot_Frame, properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::FrameImpl::SetProperties");
    throw;
  }
}

const common::PropertyMap& FrameImpl::Properties ()
{
  return impl->properties.Properties ();
}

/*
    �����-����������� �������
*/

void FrameImpl::SetShaderOptions (const common::PropertyMap& properties)
{
  try
  {
    impl->shader_options_cache.SetProperties (properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::FrameImpl::SetShaderOptions");
    throw;
  }
}

const common::PropertyMap& FrameImpl::ShaderOptions ()
{
  return impl->shader_options_cache.Properties ();
}

ShaderOptionsCache& FrameImpl::ShaderOptionsCache ()
{
  return impl->shader_options_cache;
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
  impl->entities.push_back (EntityDesc (entity));  
}

void FrameImpl::RemoveAllEntities ()
{
  impl->entities.clear ();
  impl->entities_properties.Flush ();  
}

/*
    ���������� ��������� ������
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
    ��������� ����������������� ����������� ��������� ��������
*/

void FrameImpl::SetEntityDrawHandler (const EntityDrawFunction& handler)
{
  impl->entity_draw_handler = handler;
}

const Frame::EntityDrawFunction& FrameImpl::EntityDrawHandler ()
{
  return impl->entity_draw_handler;
}

/*
    ���������� � ��������� �����
*/

void FrameImpl::Prerender (EntityDrawFunction entity_draw_handler)
{
    //���������� ������ ��������
    
  render::EffectRenderer& renderer                = *impl->effect_holder->effect_renderer;    
  Frame                   frame                   = Wrappers::Wrap<Frame> (this);
  bool                    has_entity_draw_handler = entity_draw_handler;
  EntityDrawParams&       entity_draw_params      = impl->entity_draw_params;
  PropertyCache&          entities_properties     = impl->entities_properties;
  
  renderer.RemoveAllOperations ();
  
  entity_draw_params.mvp_matrix = math::mat4f (1.0f);
  
  for (EntityArray::iterator iter=impl->entities.begin (), end=impl->entities.end (); iter!=end; ++iter)
  {
    EntityDesc& desc = *iter;
    
      //������� ��������� ������ ��������
    
    if (!desc.entity->LodsCount ())
      continue;
      
      //���������� ������������ �������
      
    DynamicTextureEntityStorage& dynamic_textures = desc.entity->DynamicTextureStorage ();
    
    dynamic_textures.Update (this);
      
      //����� �����������      

    size_t eye_distance = 0, lod = 0;
      
    if (has_entity_draw_handler)
    {
      entity_draw_handler (frame, Wrappers::Wrap<Entity> (desc.entity), entity_draw_params);
      
        //���������� ������������ ������ ���������������� ���� frame-entity
      
      entities_properties.Convert (entity_draw_params.properties, desc.property_buffer, desc.layout);
      
        //������ ���������� �� z-near �� �������        
        
      math::vec4f mvp_lod_point = entity_draw_params.mvp_matrix * math::vec4f (desc.entity->LodPoint (), 1.0f); 
      double      distance      = stl::min (stl::max (mvp_lod_point.z / mvp_lod_point.w, 1.0f), 0.0f) * ~0u;
      
      eye_distance = size_t (distance);
      lod          = impl->GetLod (distance);
    }    

    renderer.AddOperations (desc.entity->RendererOperations (lod, true), eye_distance, entity_draw_params.mvp_matrix, desc.property_buffer.get (), desc.layout.get ());
  }
  
  for (FrameArray::iterator iter=impl->frames.begin (), end=impl->frames.end (); iter!=end; ++iter)
  {
    renderer.AddOperations (**iter);
  }
}

/*
    ��������� �����
*/

void FrameImpl::Draw (RenderingContext* previous_context)
{
  try
  {
    UpdateCache ();
    
    RenderingContext context (*this, previous_context);
    
    if (!impl->effect_holder->effect_renderer)
      throw xtl::format_operation_exception ("", "No effect assigned");
      
      //���������� ������ ��������
      
    Prerender (impl->entity_draw_handler);
      
      //���������� �������� ����������

    impl->effect_holder->effect_renderer->ExecuteOperations (context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::FrameImpl::Draw");
    throw;
  }
}

/*
    ���������� ������������
*/

void FrameImpl::UpdateCache ()
{
  impl->UpdateCache ();
  
  for (EntityArray::iterator iter=impl->entities.begin (), end=impl->entities.end (); iter!=end; ++iter)
    iter->entity->UpdateCache ();
}

void FrameImpl::ResetCache ()
{
  impl->ResetCache ();
  
  for (EntityArray::iterator iter=impl->entities.begin (), end=impl->entities.end (); iter!=end; ++iter)
    iter->entity->ResetCache ();  
}
