#include "shared.h"

using namespace render;
using namespace render::manager;
using namespace render::low_level;

//TODO: ���������� ���������� �� ���������� ����������
//TODO: EntityLodCommonData::states cache (flush, update after MaterialProxy was changed)

namespace
{

/*
    ���������
*/

const size_t LODS_RESERVE = 4; //������������� ���������� ������� �����������

/*
    �������� ������ �������
*/

class EntityPrimitiveStateMap;

class EntityLodCommonData: public CacheHolder, public DebugIdHolder
{
  public:
///�����������
    EntityLodCommonData (EntityImpl& owner, const TextureManagerPtr& in_texture_manager, const DeviceManagerPtr& in_device_manager)
      : entity (owner)
      , device_manager (in_device_manager)
      , texture_manager (in_texture_manager)
      , properties (in_device_manager)
      , entity_parameters_layout (ProgramParametersLayout::Create (&in_device_manager->Device (), &in_device_manager->Settings ()))
      , shader_options_cache (&in_device_manager->CacheManager ())
      , scissor_state (false)
      , world_tm (1.0f)
      , inv_world_tm (1.0f)
      , need_update_inv_world_tm (false)
    {
      AttachCacheSource (shader_options_cache);
      AttachCacheSource (properties);

      StateBlockMask mask;
      
      mask.ss_constant_buffers [ProgramParametersSlot_Entity] = true;
      
      default_state_block = LowLevelStateBlockPtr (device_manager->Device ().CreateStateBlock (mask), false);            
      
      entity_parameters_layout->AttachSlot (ProgramParametersSlot_Entity, properties.Properties ());
      
      if (device_manager->Settings ().HasDebugLog ())
        Log ().Printf ("Entity created (id=%u)", Id ());
    }
    
///����������
    ~EntityLodCommonData ()
    {
      if (device_manager->Settings ().HasDebugLog ())
        Log ().Printf ("Entity destroyed (id=%u)", Id ());      
    }

///�������� ������ �� �������
    EntityImpl& Entity () { return entity; }
    
///�������� ���������� ���������
    DeviceManagerPtr DeviceManager () { return device_manager; }
    
///�������� �������
    TextureManagerPtr& TextureManager () { return texture_manager; }
    
///�������� �������
    PropertyBuffer& Properties () { return properties; }
    
///������������ �������
    ProgramParametersLayout& ParametersLayout () { return *entity_parameters_layout; }

///������������ ��������
    DynamicTextureEntityStorage& DynamicTextures () { return dynamic_textures; }
    
///����� ���������    
    struct MaterialStateDesc
    {
      low_level::IStateBlock*  state_block;
      Program*                 program;
      ProgramParametersLayout* parameters_layout;
    };

    void GetMaterialStateDesc (MaterialImpl* material, MaterialStateDesc& out_desc)
    {
        //��������� �������� ������ ���������� ���������

      if (!material)
      {
        out_desc.state_block       = 0;
        out_desc.program           = 0;
        out_desc.parameters_layout = 0;

        return;
      }

        //����� ��������� �� ���������

      StateMap::iterator iter = states.find (material);
            
      if (iter != states.end ())
      {
        MaterialState& state = *iter->second;

          //���������� ���� ������������� ���������

        state.UpdateCache ();

        out_desc.state_block       = &*state.state_block;
        out_desc.program           = &*state.program;
        out_desc.parameters_layout = &*state.parameters_layout;

        return;
      }

        //��������� �������� ������� ��� �������� MaterialState

      if (!properties.Properties ().Size () && !material->HasDynamicTextures () && !shader_options_cache.Properties ().Size ())
      {
        out_desc.state_block       = 0;
        out_desc.parameters_layout = &*material->ParametersLayout ();
        out_desc.program           = &*material->Program ();

        return;
      }

        //�������� ������ ���������

      StatePtr state (new MaterialState (*this, material), false);
      
      states.insert_pair (material, state);
      
      state->UpdateCache ();

      out_desc.state_block       = &*state->state_block;
      out_desc.program           = &*state->program;
      out_desc.parameters_layout = &*state->parameters_layout;
    }
        
///��� ����� �������
    render::manager::ShaderOptionsCache& ShaderOptionsCache () { return shader_options_cache; }
    
///����� ���������
    void SetScissorState (bool state)
    {
      if (state == scissor_state)
        return;
      
      scissor_state = state;
      
      InvalidateCache ();
    }
    
    bool ScissorState () { return scissor_state; }
    
///������� ���������
    void SetWorldScissor (const BoxArea& rect)
    {
      scissor_rect = rect;
      
      InvalidateCache ();
    }
    
    const BoxArea& WorldScissor () { return scissor_rect; }

///������� ������� ��������������
    void SetWorldMatrix (const math::mat4f& tm)
    {
      world_tm                 = tm;
      need_update_inv_world_tm = true;
    }

    const math::mat4f& WorldMatrix () { return world_tm; }

    const math::mat4f& InverseWorldMatrix ()
    {
      if (!need_update_inv_world_tm)
        return inv_world_tm;

      inv_world_tm             = math::inverse (world_tm);
      need_update_inv_world_tm = false;

      return inv_world_tm;
    }

///���������� ������������
    using CacheHolder::UpdateCache;
    using CacheHolder::ResetCache;

  protected:
    void FlushUnusedMaterials ()
    {
      for (StateMap::iterator iter=states.begin (), end=states.end (); iter!=end;)
        if (iter->second->material->use_count () == 1)
        {
          StateMap::iterator next = iter;
          
          ++next;
          
          states.erase (iter);
          
          iter = next;
        }
        else ++iter;
    }
    
///������ � �����
    void ResetCacheCore ()
    {
      if (device_manager->Settings ().HasDebugLog ())
        Log ().Printf ("Reset entity cache (id=%u)", Id ());      
    }
    
    void UpdateCacheCore ()
    {
      try
      {
        if (device_manager->Settings ().HasDebugLog ())
          Log ().Printf ("Update entity cache (id=%u)", Id ());

        FlushUnusedMaterials ();

        dynamic_textures.FlushUnusedTextures ();

        low_level::IDeviceContext& context = device_manager->ImmediateContext ();

        context.SSSetConstantBuffer (ProgramParametersSlot_Entity, properties.Buffer ().get ());

        default_state_block->Capture (&context);
        
        InvalidateCacheDependencies (); //TODO: ��������� �������������
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::EntityLodCommonData::Impl::UpdateCacheCore");
        throw;
      }
    }    

  private:
    struct MaterialState: public xtl::reference_counter, public CacheHolder, public DebugIdHolder
    {
      LowLevelStateBlockPtr         state_block;
      ProgramPtr                    program;
      ProgramParametersLayoutPtr    parameters_layout;
      EntityLodCommonData&          common_data;
      MaterialPtr                   material;
      size_t                        state_block_mask_hash;
      DynamicTextureMaterialStorage dynamic_textures;
      
      MaterialState (EntityLodCommonData& in_common_data, MaterialImpl* in_material)
        : common_data (in_common_data)
        , material (in_material)
        , state_block_mask_hash (0)
        , dynamic_textures (common_data.TextureManager (), in_material, common_data.Entity ())
      {
        if (!material)
          throw xtl::make_null_argument_exception ("render::manager::EntityLodCommonData::MaterialState::MaterialState", "material");

        parameters_layout = ProgramParametersLayout::Create (&common_data.DeviceManager ()->Device (), &common_data.DeviceManager ()->Settings ());
          
        common_data.AttachCacheSource (*this);
        
        AttachCacheSource (common_data.properties);
        AttachCacheSource (common_data.shader_options_cache);
        AttachCacheSource (dynamic_textures);        
        
        if (common_data.DeviceManager ()->Settings ().HasDebugLog ())
          Log ().Printf ("Entity material instance created (entity_id=%u, id=%u)", common_data.Id (), Id ());
      }
      
      ~MaterialState ()
      {
        if (common_data.DeviceManager ()->Settings ().HasDebugLog ())
          Log ().Printf ("Entity material instance destroyed (entity_id=%u, id=%u)", common_data.Id (), Id ());        
      }
      
      using CacheHolder::UpdateCache;
      
      void ResetCacheCore ()
      {
        if (common_data.DeviceManager ()->Settings ().HasDebugLog ())
          Log ().Printf ("Reset entity material instance cache (entity_id=%u, id=%u)", common_data.Id (), Id ());        
      }
      
      void UpdateCacheCore ()
      {
        try
        {
          if (common_data.DeviceManager ()->Settings ().HasDebugLog ())
            Log ().Printf ("Update entity material instance cache (entity_id=%u, id=%u)", common_data.Id (), Id ());

          if (material->HasDynamicTextures ())
          {          
            StateBlockMask mask;
            
            mask.ss_constant_buffers [ProgramParametersSlot_Entity] = true;
            
            dynamic_textures.UpdateMask (mask);
            
            render::low_level::IDevice&        device  = common_data.DeviceManager ()->Device ();
            render::low_level::IDeviceContext& context = common_data.DeviceManager ()->ImmediateContext ();
            
            size_t mask_hash = mask.Hash ();
            
            if (!state_block || state_block_mask_hash != mask_hash)
            {
              state_block           = LowLevelStateBlockPtr (device.CreateStateBlock (mask), false);
              state_block_mask_hash = mask_hash;
            }
              
            dynamic_textures.Apply (context);

            context.SSSetConstantBuffer (ProgramParametersSlot_Entity, common_data.Properties ().Buffer ().get ());
            
            state_block->Capture (&context);
          }
          else state_block = common_data.default_state_block;
          
          parameters_layout->DetachAll ();
          parameters_layout->Attach (*material->ParametersLayout ());
          parameters_layout->Attach (*common_data.entity_parameters_layout);

          program = material->Program ();

          if (program && common_data.shader_options_cache.Properties ().Size ())
            program = &program->DerivedProgram (common_data.shader_options_cache);
        }
        catch (xtl::exception& e)
        {
          state_block = LowLevelStateBlockPtr ();
          e.touch ("render::manager::EntityLodCommonData::MaterialState::UpdateCacheCore");
          throw;
        }
        catch (...)
        {
          state_block = LowLevelStateBlockPtr ();          
          throw;
        }
      }
    };
    
    typedef xtl::intrusive_ptr<MaterialState>      StatePtr;
    typedef stl::hash_map<MaterialImpl*, StatePtr> StateMap;

  private:  
    EntityImpl&                          entity;                   //�������� ������ �� ������
    DeviceManagerPtr                     device_manager;           //�������� ���������� ���������
    TextureManagerPtr                    texture_manager;          //�������� �������
    PropertyBuffer                       properties;               //�������� ����������
    ProgramParametersLayoutPtr           entity_parameters_layout; //������ ������������ �������
    StateMap                             states;                   //����� ���������
    DynamicTextureEntityStorage          dynamic_textures;         //��������� ������������ ������� ������� ����������
    render::manager::ShaderOptionsCache  shader_options_cache;     //��� ����� �������
    bool                                 scissor_state;            //��������� ������ ���������
    BoxArea                              scissor_rect;             //������� ���������
    LowLevelStateBlockPtr                default_state_block;      //���� ��������� �� ���������
    math::mat4f                          world_tm;                 //������� ������� ��������������
    math::mat4f                          inv_world_tm;             //��������� ������� ��������������
    bool                                 need_update_inv_world_tm; //���������� �������� ��������� ������� ��������������
};

/*
    �������� ������ �����������
*/

typedef stl::vector<RendererOperation> RendererOperationArray;

struct EntityLod: public xtl::reference_counter, public EntityLodDesc, public CacheHolder, public DebugIdHolder
{
  EntityLodCommonData&          common_data;           //����� ������ ��� ���� ������� �����������
  size_t                        level_of_detail;       //����� ������ �����������
  PrimitiveProxy                primitive;             //��������
  PrimitivePtr                  cached_primitive;      //�������������� ��������
  DynamicPrimitiveEntityStorage dynamic_primitives;    //������������ ��������� �������
  RendererOperationArray        cached_operations;     //�������������� �������� ����������
  RendererOperationList         cached_operation_list; //�������������� �������� ����������

///�����������
  EntityLod (EntityLodCommonData& in_common_data, size_t in_level_of_detail, const PrimitiveProxy& in_primitive)
    : EntityLodDesc (cached_operation_list, dynamic_primitives)
    , common_data (in_common_data)
    , level_of_detail (in_level_of_detail)
    , primitive (in_primitive)
    , dynamic_primitives (common_data.Entity ())
  {
    AttachCacheSource (common_data);
    AttachCacheSource (dynamic_primitives);
    
    primitive.AttachCacheHolder (*this);
    
    memset (&cached_operation_list, 0, sizeof (cached_operation_list));

    if (common_data.DeviceManager ()->Settings ().HasDebugLog ())
      Log ().Printf ("Entity lod created (entity_id=%u, id=%u)", common_data.Id (), Id ());
  }
  
///����������
  ~EntityLod ()
  {
    if (common_data.DeviceManager ()->Settings ().HasDebugLog ())
      Log ().Printf ("Entity lod destroyed (entity_id=%u, id=%u)", common_data.Id (), Id ());
  }
  
///����������� ������� ����
  void ResetCacheCore ()
  {
    if (common_data.DeviceManager ()->Settings ().HasDebugLog ())
      Log ().Printf ("Reset entity lod cache (entity_id=%u, id=%u)", common_data.Id (), Id ());

    cached_primitive = PrimitivePtr ();

    dynamic_primitives.RemoveAllPrimitives ();
    
    cached_operations.clear ();

    has_frame_dependent_operations    = false;
    has_frame_independent_operations  = false;
    has_entity_dependent_operations   = false;
    has_entity_independent_operations = false;
    
    memset (&cached_operation_list, 0, sizeof (cached_operation_list));
  }  

  void UpdateCacheCore ()
  {
    try
    {
      has_frame_dependent_operations    = false;
      has_frame_independent_operations  = false;
      has_entity_dependent_operations   = false;
      has_entity_independent_operations = false;

         //����������������

      if (common_data.DeviceManager ()->Settings ().HasDebugLog ())
        Log ().Printf ("Update entity lod cache (entity_id=%u, id=%u)", common_data.Id (), Id ());

      cached_primitive = primitive.Resource ();

      if (!cached_primitive)
      {  
        dynamic_primitives.RemoveAllPrimitives ();
        cached_operations.clear ();

        memset (&cached_operation_list, 0, sizeof (cached_operation_list));

        throw xtl::format_operation_exception ("", "Primitive '%s' not found", primitive.Name ());
      }

        //���������� ������������ ���������� (���������� ��������� ������������� ����� ������� ��� ������ EndUpdate)

      {
        DynamicPrimitiveEntityStorage::UpdateScope scope (dynamic_primitives);

        cached_primitive->FillDynamicPrimitiveStorage (dynamic_primitives);
      }
        
        //��������� �����

      size_t groups_count         = cached_primitive->RendererPrimitiveGroupsCount (),
             dynamic_groups_count = dynamic_primitives.RendererPrimitiveGroupsCount (),
             operations_count     = 0;
      
      const RendererPrimitiveGroup*        groups         = cached_primitive->RendererPrimitiveGroups ();
      const RendererDynamicPrimitiveGroup* dynamic_groups = dynamic_primitives.RendererPrimitiveGroups ();
      
        //�������������� �������� ����������

      for (size_t i=0; i<groups_count; i++)
        operations_count += groups [i].primitives_count;

      for (size_t i=0; i<dynamic_groups_count; i++)
        operations_count += dynamic_groups [i].primitives_count;

      cached_operations.clear ();
      cached_operations.reserve (operations_count);
      
      const BoxAreaImpl* scissor = common_data.ScissorState () ? Wrappers::Unwrap<BoxAreaImpl> (common_data.WorldScissor ()).get () : (const BoxAreaImpl*)0;

        //���������� ������ �������� ���������� ����������� ����������

      const RendererPrimitiveGroup* group = groups;
        
      for (size_t i=0; i<groups_count; i++, group++)
        BuildRendererOperations (*group, scissor, 0);

        //���������� ������ �������� ���������� ������������ ����������

      const RendererDynamicPrimitiveGroup* dynamic_group = dynamic_groups;

      for (size_t i=0; i<dynamic_groups_count; i++, dynamic_group++)
        BuildRendererOperations (*dynamic_group, scissor, dynamic_group->dynamic_primitive);

      cached_operation_list.operations_count = cached_operations.size ();
      cached_operation_list.operations       = cached_operations.empty () ? (RendererOperation*)0 : &cached_operations [0];
      
      InvalidateCacheDependencies ();
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::EntityLod::UpdateCacheCore");
      throw;
    }
  }  

  void BuildRendererOperations (const RendererPrimitiveGroup& group, const BoxAreaImpl* scissor, DynamicPrimitive* dynamic_primitive)
  {
    const RendererPrimitive* primitives       = group.primitives;        
    size_t                   primitives_count = group.primitives_count;
    
    for (size_t j=0; j<primitives_count; j++)
    {
      const RendererPrimitive& renderer_primitive = primitives [j];
      
      RendererOperation operation;
      
      memset (&operation, 0, sizeof (operation));
      
      operation.primitive         = &renderer_primitive;
      operation.dynamic_primitive = dynamic_primitive;
      operation.entity            = &common_data.Entity ();

      bool frame_dependent  = dynamic_primitive ? dynamic_primitive->IsFrameDependent () : false,
           entity_dependent = dynamic_primitive ? dynamic_primitive->IsEntityDependent () : false;

      has_frame_dependent_operations    = has_frame_dependent_operations || frame_dependent;
      has_frame_independent_operations  = has_frame_independent_operations || !frame_dependent;
      has_entity_dependent_operations   = has_entity_dependent_operations || entity_dependent;
      has_entity_independent_operations = has_entity_independent_operations || !entity_dependent;

      MaterialImpl* material = renderer_primitive.material;

      EntityLodCommonData::MaterialStateDesc material_state;

      common_data.GetMaterialStateDesc (material, material_state);

      operation.state_block              = material_state.state_block;
      operation.entity_parameters_layout = material_state.parameters_layout;
      operation.program                  = material_state.program;
      operation.scissor                  = scissor;
      operation.batching_hash            = get_batching_hash (operation);

      cached_operations.push_back (operation);
    }
  }

  using CacheHolder::UpdateCache;
  using CacheHolder::ResetCache;  
  using CacheHolder::InvalidateCache;
};

typedef xtl::intrusive_ptr<EntityLod> EntityLodPtr;
typedef stl::vector<EntityLodPtr>     EntityLodArray;

/*
    ������� ��������� �����
*/

struct EntityLodLess
{
  bool operator () (const EntityLodPtr& lod1, const EntityLodPtr& lod2) const { return lod1->level_of_detail < lod2->level_of_detail; }
  bool operator () (const EntityLodPtr& lod, size_t level_of_detail) const    { return lod->level_of_detail < level_of_detail; }    
  bool operator () (size_t level_of_detail, const EntityLodPtr& lod) const    { return level_of_detail < lod->level_of_detail; }  
};

struct EntityLodGreater
{
  bool operator () (const EntityLodPtr& lod1, const EntityLodPtr& lod2) const { return lod1->level_of_detail > lod2->level_of_detail; }
  bool operator () (const EntityLodPtr& lod, size_t level_of_detail) const    { return lod->level_of_detail > level_of_detail; }    
  bool operator () (size_t level_of_detail, const EntityLodPtr& lod) const    { return level_of_detail > lod->level_of_detail; }  
};

}

/*
    �������� ���������� ������� ����������
*/

struct EntityImpl::Impl: public EntityLodCommonData
{
  EntityLodArray      lods;              //������ �����������
  bool                need_resort;       //������ ����������� ������� ��������������
  PrimitiveManagerPtr primitive_manager; //�������� ����������
  math::vec3f         lod_point;         //����� ������� lod-������
  void*               user_data;         //���������������� ������

///�����������
  Impl (EntityImpl& owner, const DeviceManagerPtr& device_manager, const TextureManagerPtr& texture_manager, const PrimitiveManagerPtr& in_primitive_manager)
    : EntityLodCommonData (owner, texture_manager, device_manager)
    , need_resort (false)
    , primitive_manager (in_primitive_manager)
    , user_data ()
  {
    lods.reserve (LODS_RESERVE);
  }
  
///��������� ������ �����������
  int FindLodIndex (size_t index, bool find_nearest = false)
  {
    if (need_resort)
    {
      if (!lods.empty ())
        stl::sort (lods.begin (), lods.end (), EntityLodLess ());
      
      need_resort = false;
    }

    EntityLodArray::iterator iter = find_nearest ? stl::lower_bound (lods.begin (), lods.end (), index, EntityLodGreater ()) : 
      stl::equal_range (lods.begin (), lods.end (), index, EntityLodLess ()).first;

    if (iter != lods.end ())
      return iter - lods.begin ();
      
    return -1;
  }
  
  EntityLodPtr FindLod (size_t level_of_detail, bool find_nearest = false)
  {
    int index = FindLodIndex (level_of_detail, find_nearest);
    
    return index != -1 ? lods [index] : EntityLodPtr ();
  }  
    
  using CacheHolder::UpdateCache;
};

/*
    ����������� / ����������
*/

EntityImpl::EntityImpl (const DeviceManagerPtr& device_manager, const TextureManagerPtr& texture_manager, const PrimitiveManagerPtr& primitive_manager)
{
  try
  {
    if (!device_manager)
      throw xtl::make_null_argument_exception ("", "device_manager");
      
    if (!texture_manager)
      throw xtl::make_null_argument_exception ("", "texture_manager");
      
    if (!primitive_manager)
      throw xtl::make_null_argument_exception ("", "primitive_manager");
    
    impl = new Impl (*this, device_manager, texture_manager, primitive_manager);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::EntityImpl::EntityImpl");
    throw;
  }
}

EntityImpl::~EntityImpl ()
{
}

/*
    ������� ������� ��������������
*/

void EntityImpl::SetWorldMatrix (const math::mat4f& tm)
{
  impl->SetWorldMatrix (tm);
}

const math::mat4f& EntityImpl::WorldMatrix ()
{
  return impl->WorldMatrix ();
}

const math::mat4f& EntityImpl::InverseWorldMatrix ()
{
  return impl->InverseWorldMatrix ();
}

/*
    ��������� ������������ �������
*/

DynamicTextureEntityStorage& EntityImpl::DynamicTextureStorage ()
{
  return impl->DynamicTextures ();
}

/*
    �������� ����������
*/

void EntityImpl::SetProperties (const common::PropertyMap& properties)
{
  impl->Properties ().SetProperties (properties);

  impl->ParametersLayout ().AttachSlot (ProgramParametersSlot_Entity, properties);
}

const common::PropertyMap& EntityImpl::Properties ()
{
  return impl->Properties ().Properties ();
}

/*
    �����-����������� �������
*/

void EntityImpl::SetShaderOptions (const common::PropertyMap& properties)
{
  try
  {
    impl->ShaderOptionsCache ().SetProperties (properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::EntityImpl::SetShaderOptions");
    throw;
  }
}

const common::PropertyMap& EntityImpl::ShaderOptions ()
{
  return impl->ShaderOptionsCache ().Properties ();
}

/*
    ���������������� ������
*/

void EntityImpl::SetUserData (void* data)
{
  impl->user_data = data;
}

void* EntityImpl::UserData ()
{
  return impl->user_data;
}

/*
    ������ � ������� (��� ���������)
      �������������� ���������� �� ������� EntityImpl::Transformation � ������ ���� ��� �� ��������    
*/

void EntityImpl::SetJointsCount (size_t count)
{
  throw xtl::make_not_implemented_exception ("render::manager::EntityImpl::SetJointsCount");
}

size_t EntityImpl::JointsCount ()
{
  throw xtl::make_not_implemented_exception ("render::manager::EntityImpl::JointsCount");
}

void EntityImpl::SetJointTransformation (size_t joint_index, const math::mat4f&)
{
  throw xtl::make_not_implemented_exception ("render::manager::EntityImpl::SetJointTransformation");
}

const math::mat4f& EntityImpl::JointTransformation (size_t joint_index)
{
  throw xtl::make_not_implemented_exception ("render::manager::EntityImpl::JointTransformation");
}

/*
    ������ � �������� �����������
*/

size_t EntityImpl::LodsCount ()
{
  return impl->lods.size ();
}

/*
    ������ � ����������
*/

PrimitivePtr EntityImpl::Primitive (size_t level_of_detail)
{
  static const char* METHOD_NAME = "render::manager::EntityImpl::Primitive";

  EntityLodPtr lod = impl->FindLod (level_of_detail);

  if (lod)
  {
    PrimitivePtr result = lod->primitive.Resource ();
    
    if (result)
      return result;
      
    if (*lod->primitive.Name ())
      throw xtl::format_operation_exception (METHOD_NAME, "Lod #%u primitive '%s' not found", level_of_detail, lod->primitive.Name ());
  }

  throw xtl::make_argument_exception (METHOD_NAME, "level_of_detail", level_of_detail, "Lod primitive is not set");
}

const char* EntityImpl::PrimitiveName (size_t level_of_detail)
{
  static const char* METHOD_NAME = "render::manager::EntityImpl::PrimitiveName";

  EntityLodPtr lod = impl->FindLod (level_of_detail);

  if (lod)
    return lod->primitive.Name ();

  throw xtl::make_argument_exception (METHOD_NAME, "level_of_detail", level_of_detail, "Lod primitive is not set");
}

void EntityImpl::ResetPrimitive (size_t level_of_detail)
{
  int index = impl->FindLodIndex (level_of_detail);
  
  if (index == -1)
    return;
    
  impl->lods.erase (impl->lods.begin () + index);
}

void EntityImpl::SetPrimitive (const PrimitivePtr& primitive, size_t level_of_detail)
{
  try
  {
    PrimitiveProxy proxy = impl->primitive_manager->GetPrimitiveProxy ("");
    
    proxy.SetResource (primitive);
    
    EntityLodPtr lod (new EntityLod (*impl, level_of_detail, proxy), false);  
    
    int lod_index = impl->FindLodIndex (level_of_detail);
    
    if (lod_index == -1)
    {
      impl->lods.push_back (lod);
      impl->need_resort = true;
    }
    else impl->lods [lod_index] = lod;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::EntityImpl::SetPrimitive(const PrimitivePtr&,size_t)");
    throw;
  }
}

void EntityImpl::SetPrimitive (const char* name, size_t level_of_detail)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    if (!*name)
      throw xtl::make_argument_exception ("", "name", name, "Empty primitive name is not allowed as entity level of detail");
    
    EntityLodPtr lod (new EntityLod (*impl, level_of_detail, impl->primitive_manager->GetPrimitiveProxy (name)), false);
      
    int lod_index = impl->FindLodIndex (level_of_detail);
    
    if (lod_index == -1)
    {
      impl->lods.push_back (lod);
      impl->need_resort = true;
    }
    else impl->lods [lod_index] = lod;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::EntityImpl::SetPrimitive(const char*,size_t)");
    throw;
  }  
}

bool EntityImpl::HasPrimitive (size_t level_of_detail)
{
  return impl->FindLodIndex (level_of_detail) != -1;
}

void EntityImpl::ResetAllPrimitives ()
{
  impl->lods.clear ();

  impl->need_resort = false;
}

/*
    ����� � ��������� ������� ��������� ������� ��� ������� ����������� �� ������
*/

void EntityImpl::SetLodPoint (const math::vec3f& p)
{
  impl->lod_point = p;
}

const math::vec3f& EntityImpl::LodPoint ()
{
  return impl->lod_point;
}

/*
    ���������� �������� ��������� �������
*/

void EntityImpl::SetWorldScissor (const BoxArea& scissor)
{
  impl->SetWorldScissor (scissor);
}

const BoxArea& EntityImpl::WorldScissor ()
{
  return impl->WorldScissor ();
}

void EntityImpl::SetScissorState (bool state)
{
  impl->SetScissorState (state);
}

bool EntityImpl::ScissorState ()
{
  return impl->ScissorState ();
}

/*
    ��������� ���������� �� ������ �����������
*/

const EntityLodDesc& EntityImpl::GetLod (size_t level_of_detail, bool find_nearest)
{
  try
  {    
    EntityLodPtr lod = impl->FindLod (level_of_detail, find_nearest);

    if (!lod)
      throw xtl::make_argument_exception ("", "level_of_detail", level_of_detail, "Lod primitive is not set");

    impl->Properties ().UpdateCache (); //TODO: ������!!!

    lod->UpdateCache ();

    return *lod;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::EntityImpl::GetLod");
    throw;
  }
}

/*
    ���������� ������������
*/

void EntityImpl::UpdateCache ()
{
  impl->Properties ().UpdateCache ();

  impl->UpdateCache ();

  for (EntityLodArray::iterator iter=impl->lods.begin (), end=impl->lods.end (); iter!=end; ++iter)
    (*iter)->UpdateCache ();
}

void EntityImpl::ResetCache ()
{
  impl->Properties ().ResetCache ();

  impl->ResetCache ();
  
  for (EntityLodArray::iterator iter=impl->lods.begin (), end=impl->lods.end (); iter!=end; ++iter)
    (*iter)->ResetCache ();  
}
  