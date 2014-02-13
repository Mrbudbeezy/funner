#include "shared.h"

using namespace render::manager;
using namespace render::low_level;

//TODO: ������ ���������� ������ ��� �������� ��� ������� �����
//TODO: ������� ��������� �������� �� ������� ���������

/*
    �������� ���������� ���������
*/

namespace
{

///����� ������ �����
struct MeshCommonData
{
  DeviceManagerPtr   device_manager; //�������� ���������� ���������
  LowLevelBufferPtr  index_buffer;   //��������� �����
  stl::string        name;           //��� ����  
  
  MeshCommonData (const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)
  {
    if (!device_manager)
      throw xtl::make_null_argument_exception ("render::manager::MeshCommonData::MeshCommonData", "device_manager");
  }
};

///�������� ����
struct MeshPrimitive: public xtl::reference_counter, public CacheHolder, public DebugIdHolder
{
  MeshCommonData&                  common_data;                  //����� ������ ��� ���������
  size_t                           primitive_index;              //������ ��������� � ����
  render::low_level::PrimitiveType type;                         //��� ���������
  VertexBufferPtr                  vertex_buffer;                //��������� �����
  LowLevelInputLayoutPtr           layout;                       //������ ���������
  size_t                           first;                        //������ ������ �������/�������
  size_t                           count;                        //���������� ����������
  size_t                           base_vertex;                  //������ ������� �������
  MaterialProxy                    material;                     //��������
  Log                              log;                          //����� ����������������
  MaterialPtr                      cached_material;              //�������������� ��������
  RendererPrimitive                cached_primitive;             //�������� �������������� ��� ����������
  size_t                           cached_state_block_mask_hash; //��� ����� ��������������� ����� ���������
  LowLevelStateBlockPtr            cached_state_block;           //�������������� ���� ���������  
  
///�����������
  MeshPrimitive (CacheHolder& parent_holder, size_t in_primitive_index, const MaterialProxy& in_material, MeshCommonData& in_common_data)
    : common_data (in_common_data)
    , primitive_index (in_primitive_index)
    , type (PrimitiveType_PointList)
    , first (0)
    , count (0)    
    , base_vertex (0)
    , material (in_material)
    , cached_state_block_mask_hash (0)
  {
    parent_holder.AttachCacheSource (*this);
    
    material.AttachCacheHolder (*this);
    
    memset (&cached_primitive, 0, sizeof (cached_primitive));    
    
    if (common_data.device_manager->Settings ().HasDebugLog ())
      log.Printf ("Mesh '%s' primitive #%u created (id=%u)", common_data.name.c_str (), primitive_index, Id ());
  }
  
///����������
  ~MeshPrimitive ()
  {
      //��������������� ������� ��������� ��� ����������� ����������� ������� �������� �������� �� � ����� �������� �������    
    
    cached_material              = MaterialPtr ();
    cached_state_block           = LowLevelStateBlockPtr ();
    cached_state_block_mask_hash = 0;
    
    if (common_data.device_manager->Settings ().HasDebugLog ())
      log.Printf ("Mesh '%s' primitive #%u destroyed (id=%u)", common_data.name.c_str (), primitive_index, Id ());    
  }
  
///����� ����
  void ResetCacheCore ()
  {
    if (common_data.device_manager->Settings ().HasDebugLog ())
      log.Printf ("Reset mesh '%s' primitive #%u cache (id=%u)", common_data.name.c_str (), primitive_index, Id ());        
    
    cached_material              = MaterialPtr ();   
    cached_state_block           = LowLevelStateBlockPtr ();
    cached_state_block_mask_hash = 0;
  }
  
///���������� ����
  void UpdateCacheCore ()
  {
    try
    {
      bool has_debug_log = common_data.device_manager->Settings ().HasDebugLog ();
      
      if (has_debug_log)
        log.Printf ("Update mesh '%s' primitive #%u cache (id=%u)", common_data.name.c_str (), primitive_index, Id ());
      
      memset (&cached_primitive, 0, sizeof (cached_primitive));
      
        //������ ��������� ���������
      
      cached_material = material.Resource ();
      
      LowLevelStateBlockPtr material_state_block = cached_material ? cached_material->StateBlock () : LowLevelStateBlockPtr ();
      
      render::low_level::IDevice&        device  = common_data.device_manager->Device ();
      render::low_level::IDeviceContext& context = common_data.device_manager->ImmediateContext ();
        
      render::low_level::StateBlockMask mask;      

      if (material_state_block)
      {
        material_state_block->Apply (&context);
        material_state_block->GetMask (mask);
      }
      
        //��������� ���������/���������� ������
        
      mask.is_index_buffer = true; //0 index buffer is need to be set
      mask.is_layout       = true;
      
      const LowLevelBufferPtr* streams = vertex_buffer->Streams ();

      for (size_t i=0, streams_count=vertex_buffer->StreamsCount (); i<streams_count; i++)
      {        
        context.ISSetVertexBuffer (i, streams [i].get ());
        
        mask.is_vertex_buffers [i] = true;
      }

      context.ISSetInputLayout  (layout.get ());
      context.ISSetIndexBuffer  (common_data.index_buffer.get ());
      
        //���������� ����� ��������� ���������
      
      size_t mask_hash = mask.Hash ();
      
      if (cached_state_block_mask_hash != mask_hash)
      {
        if (has_debug_log)
          log.Printf ("...create state block for mesh primitive");
        
        cached_state_block           = LowLevelStateBlockPtr (device.CreateStateBlock (mask), false);
        cached_state_block_mask_hash = mask_hash;
      }
      
      cached_state_block->Capture (&context);
      
        //����������� ���������� ��������� ��� ���������
      
      cached_primitive.material      = cached_material.get ();
      cached_primitive.state_block   = cached_state_block.get ();
      cached_primitive.indexed       = common_data.index_buffer != LowLevelBufferPtr ();
      cached_primitive.type          = type;
      cached_primitive.first         = first;
      cached_primitive.count         = count;
      cached_primitive.base_vertex   = base_vertex;
      cached_primitive.tags_count    = cached_material ? cached_material->TagsCount () : 0;
      cached_primitive.tags          = cached_material ? cached_material->Tags () : (const size_t*)0;
      
        //���������� ������������ ������, ��������� ����� ��������� ���������/��������� ������ ���� ���������� �� ��������� ����
        
      InvalidateCacheDependencies ();
      
      if (has_debug_log)
        log.Printf ("...mesh primitive cache updated");
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::MeshPrimitive::UpdateCacheCore");
      throw;
    }
  }
  
  using CacheHolder::UpdateCache;  
  using CacheHolder::ResetCache;
};

typedef xtl::intrusive_ptr<MeshPrimitive> MeshPrimitivePtr;
typedef stl::vector<MeshPrimitivePtr>     MeshPrimitiveArray;
typedef stl::vector<RendererPrimitive>    RendererPrimitiveArray;

///���
struct Mesh: public xtl::reference_counter, public MeshCommonData, public CacheHolder, public DebugIdHolder
{
  MeshPrimitiveArray     primitives;         //������ ���������� ����
  RendererPrimitiveArray cached_primitives;  //�������������� ���������
  RendererPrimitiveGroup cached_group;       //������ �������������� ����������
  Log                    log;                //����� ����������������
  
///�����������
  Mesh (CacheHolder& parent_holder, const char* in_name, const DeviceManagerPtr& device_manager)
    : MeshCommonData (device_manager)    
  {
    if (!in_name)
      throw xtl::make_null_argument_exception ("render::manager::Mesh::Mesh", "name");

    name = in_name;
    
    parent_holder.AttachCacheSource (*this);
    
    if (device_manager->Settings ().HasDebugLog ())
      log.Printf ("Mesh '%s' created (id=%u)", name.c_str (), Id ());
  }
  
///����������
  ~Mesh ()
  {
      //��������������� ������� ��������� ��� ����������� ����������� ������� �������� �������� �� � ����� �������� �������
      
    memset (&cached_group, 0, sizeof (cached_group));      

    cached_primitives.clear ();
    primitives.clear ();
    
    if (device_manager->Settings ().HasDebugLog ())
      log.Printf ("Mesh '%s' destroyed (id=%u)", name.c_str (), Id ());    
  }
  
///����� ����
  void ResetCacheCore ()
  {
    if (device_manager->Settings ().HasDebugLog ())
      log.Printf ("Reset mesh '%s' cache (id=%u)", name.c_str (), Id ());        
        
    cached_primitives.clear ();   
  }
  
///���������� ����
  void UpdateCacheCore ()
  {
    try
    {
      bool has_debug_log = device_manager->Settings ().HasDebugLog ();
      
      if (has_debug_log)
        log.Printf ("Update mesh '%s' cache (id=%u)", name.c_str (), Id ());
      
      cached_primitives.clear ();
      cached_primitives.reserve (primitives.size ());
      
      for (MeshPrimitiveArray::iterator iter=primitives.begin (), end=primitives.end (); iter!=end; ++iter)
      {
        MeshPrimitive& src_primitive = **iter;

        if (!src_primitive.IsValid ())
          continue;        

        cached_primitives.push_back (src_primitive.cached_primitive);
      }
      
      memset (&cached_group, 0, sizeof (cached_group));
      
      cached_group.primitives_count = cached_primitives.size ();
      
      if (!cached_primitives.empty ())
        cached_group.primitives = &cached_primitives [0];
        
        //���������� ������������ ������, ��������� ����� ��������� ���������/��������� ������ ���� ���������� �� ��������� ����
        
      InvalidateCacheDependencies ();        
        
      if (has_debug_log)
        log.Printf ("...mesh cache updated");        
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::Mesh::UpdateCacheCore");
      throw;
    }
  }  
  
  using CacheHolder::UpdateCache;
  using CacheHolder::ResetCache;
};

typedef xtl::intrusive_ptr<Mesh>                        MeshPtr;
typedef stl::vector<MeshPtr>                            MeshArray;
typedef stl::vector<RendererPrimitiveGroup>             RenderPrimitiveGroupsArray;
typedef xtl::intrusive_ptr<SimplePrimitiveListImplBase> SimplePrimitiveListPtr;

enum SimplePrimitiveListType
{
  SimplePrimitiveListType_Sprite,
  SimplePrimitiveListType_Line,
};

struct SimplePrimitiveListDesc
{
  SimplePrimitiveListPtr  list;
  SimplePrimitiveListType type;
  RendererPrimitive*      primitive;

  SimplePrimitiveListDesc (const SimplePrimitiveListPtr& in_list, SimplePrimitiveListType in_type, RendererPrimitive* in_primitive)
    : list (in_list)
    , type (in_type)
    , primitive (in_primitive)
  {
  }
};


typedef stl::vector<SimplePrimitiveListDesc> SimplePrimitiveListArray;

}

struct PrimitiveImpl::Impl: public DebugIdHolder
{
  DeviceManagerPtr           device_manager;                               //�������� ����������
  MaterialManagerPtr         material_manager;                             //�������� ����������
  BuffersPtr                 buffers;                                      //������ ���������
  MeshArray                  meshes;                                       //����
  SimplePrimitiveListArray   entity_independent_dynamic_primitive_lists;   //������ ������������ ����������
  SimplePrimitiveListArray   entity_dependent_dynamic_primitive_lists;     //������ ������������ ����������
  RendererPrimitiveArray     cached_entity_independent_dynamic_primitives; //�������������� ������������ ��������� �� ��������� �� �������
  size_t                     line_lists_count;                             //���������� ������� � �������
  size_t                     sprite_lists_count;                           //���������� ������� �� ���������
  stl::string                name;                                         //��� ���������
  RenderPrimitiveGroupsArray render_groups;                                //������
  Log                        log;                                          //����� ����������������

///�����������
  Impl (const DeviceManagerPtr& in_device_manager, const MaterialManagerPtr& in_material_manager, const BuffersPtr& in_buffers, const char* in_name)
    : device_manager (in_device_manager)
    , material_manager (in_material_manager)
    , buffers (in_buffers)
    , line_lists_count ()
    , sprite_lists_count ()
  {
    static const char* METHOD_NAME = "render::manager::PrimitiveImpl::Impl::Impl";
    
    if (!device_manager)
      throw xtl::format_operation_exception (METHOD_NAME, "No DeviceManager binded");

    if (!buffers)
      throw xtl::make_null_argument_exception (METHOD_NAME, "buffers");
      
    if (!in_name)
      throw xtl::make_null_argument_exception (METHOD_NAME, "name");
      
    name = in_name;
      
    if (device_manager->Settings ().HasDebugLog ())
      log.Printf ("Primitive '%s' created (id=%u)", name.c_str (), Id ());
  }

///����������
  ~Impl ()
  {
      //��������������� ������� ��������� ��� ����������� ����������� ������� �������� �������� �� � ����� �������� �������    
    
    meshes.clear ();
    render_groups.clear ();
    
    if (device_manager->Settings ().HasDebugLog ())
      log.Printf ("Primitive '%s' destroyed (id=%u)", name.c_str (), Id ());
  }
};

/*
    ����������� / ����������
*/

PrimitiveImpl::PrimitiveImpl (const DeviceManagerPtr& device_manager, const MaterialManagerPtr& material_manager, const BuffersPtr& buffers, const char* name)
{
  try
  {
    impl = new Impl (device_manager, material_manager, buffers, name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveImpl::PrimitiveImpl");
    throw;
  }
}

PrimitiveImpl::~PrimitiveImpl ()
{
}

/*
    ���
*/

const char* PrimitiveImpl::Name ()
{
  return impl->name.c_str ();
}

void PrimitiveImpl::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::manager::PrimitiveImpl::SetName", "name");
    
  if (impl->device_manager->Settings ().HasDebugLog ())
    impl->log.Printf ("Primitive '%s' name changed to '%s' (id=%u)", impl->name.c_str (), name, impl->Id ());
    
  impl->name = name;    
}

/*
    ������ ���������
*/

const PrimitiveImpl::BuffersPtr& PrimitiveImpl::Buffers ()
{
  return impl->buffers;
}

/*
    ������ � ������
*/

size_t PrimitiveImpl::MeshesCount ()
{
  return impl->meshes.size ();
}

size_t PrimitiveImpl::AddMesh (const media::geometry::Mesh& source, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage)
{
  try
  {
    MeshPtr mesh (new Mesh (*this, source.Name (), impl->device_manager), false);
    
      //����������� ��������� �������
             
    stl::vector<VertexBufferPtr> vertex_buffers;
    
    vertex_buffers.reserve (source.VertexBuffersCount ());
    
    for (size_t i=0, count=source.VertexBuffersCount (); i<count; i++)
      vertex_buffers.push_back (impl->buffers->CreateVertexBuffer (source.VertexBuffer (i), vb_usage));

      //����������� ���������� ������ (���� �� ����)
      
    InputDataType index_type = InputDataType_UShort;
    
    if (source.IndexBuffer ().Size ())
    {
      mesh->index_buffer = impl->buffers->CreateIndexBuffer (source.IndexBuffer (), ib_usage);
      
      switch (source.IndexBuffer ().DataType ())
      {
        case media::geometry::IndexType_UInt32:
          index_type = InputDataType_UInt;
          break;
        case media::geometry::IndexType_UInt16:
          index_type = InputDataType_UShort;
          break;
        case media::geometry::IndexType_UInt8:
          index_type = InputDataType_UByte;
          break;          
        default:
          throw xtl::format_operation_exception ("", "Unexpected index buffer data type %s", get_index_type_name (source.IndexBuffer ().DataType ()));
      }
    }

      //����������� ����������
    
    mesh->primitives.reserve (source.PrimitivesCount ());
    
    for (size_t i=0, count=source.PrimitivesCount (); i<count; i++)
    {
      const media::geometry::Primitive& src_primitive = source.Primitive (i);
      
      MeshPrimitivePtr dst_primitive (new MeshPrimitive (*mesh, i, impl->material_manager->GetMaterialProxy (src_primitive.material), *mesh), false);

      switch (src_primitive.type)
      {
        case media::geometry::PrimitiveType_LineList:
          dst_primitive->type  = PrimitiveType_LineList;
          dst_primitive->count = src_primitive.count * 2;
          break;
        case media::geometry::PrimitiveType_LineStrip:
          dst_primitive->type  = PrimitiveType_LineStrip;
          dst_primitive->count = src_primitive.count + 1;
          break;
        case media::geometry::PrimitiveType_TriangleList:
          dst_primitive->type  = PrimitiveType_TriangleList;
          dst_primitive->count = src_primitive.count * 3;
          break;
        case media::geometry::PrimitiveType_TriangleStrip:
          dst_primitive->type  = PrimitiveType_TriangleStrip;
          dst_primitive->count = src_primitive.count + 2;
          break;
        case media::geometry::PrimitiveType_TriangleFan:
          dst_primitive->type  = PrimitiveType_TriangleFan;
          dst_primitive->count = src_primitive.count + 2;
          break;
        default:
          throw xtl::format_operation_exception ("", "Bad primitive #%u type %s", i, get_type_name (src_primitive.type));
      }

      dst_primitive->base_vertex = src_primitive.base_vertex;

      if (src_primitive.vertex_buffer >= vertex_buffers.size ())
        throw xtl::format_operation_exception ("", "Bad primitive #%u vertex buffer index %u (vertex buffers count is %u)", i, src_primitive.vertex_buffer,
          vertex_buffers.size ());          

      dst_primitive->vertex_buffer = vertex_buffers [src_primitive.vertex_buffer];
      dst_primitive->first         = src_primitive.first;
      dst_primitive->layout        = dst_primitive->vertex_buffer->CreateInputLayout (impl->device_manager->InputLayoutManager (), index_type);
      
      mesh->primitives.push_back (dst_primitive);
    }
    
      //���������� ����      
      
    impl->meshes.push_back (mesh);
    
    InvalidateCache ();
    
    return impl->meshes.size () - 1;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveImpl::AddMesh");
    throw;
  }
}

void PrimitiveImpl::RemoveMeshes (size_t first_mesh, size_t meshes_count)
{
  if (first_mesh >= impl->meshes.size ())
    return;
    
  if (first_mesh + meshes_count > impl->meshes.size ())
    meshes_count = impl->meshes.size () - first_mesh;        

  impl->meshes.erase (impl->meshes.begin () + first_mesh, impl->meshes.begin () + first_mesh + meshes_count);

  InvalidateCache ();  
}

void PrimitiveImpl::RemoveAllMeshes ()
{
  impl->meshes.clear ();
  
  InvalidateCache ();  
}

/*
    ������ �� ���������
*/

size_t PrimitiveImpl::SpriteListsCount ()
{
  return impl->sprite_lists_count;
}

SpriteListPtr PrimitiveImpl::AddStandaloneSpriteList (SpriteMode mode, const math::vec3f& up, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage)
{
  try
  {
    switch (vb_usage)
    {
      case MeshBufferUsage_Static:
      case MeshBufferUsage_Dynamic:
      case MeshBufferUsage_Stream:
        break;
      default:
        throw xtl::make_argument_exception ("", "vb_usage", vb_usage);
    }

    switch (ib_usage)
    {
      case MeshBufferUsage_Static:
      case MeshBufferUsage_Dynamic:
      case MeshBufferUsage_Stream:
        break;
      default:
        throw xtl::make_argument_exception ("", "ib_usage", ib_usage);
    }

    switch (mode)
    {
      case SpriteMode_Billboard:
      case SpriteMode_Oriented:
      case SpriteMode_OrientedBillboard:
        break;
      default:
        throw xtl::make_argument_exception ("", "mode", mode);
    }

    SpriteListPtr list (create_standalone_sprite_list (impl->material_manager, mode, up, vb_usage, ib_usage), false);

    AddSimplePrimitiveList (list.get (), SimplePrimitiveListType_Sprite);

    return list;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveImpl::AddStandaloneSpriteList");
    throw;
  }
}

SpriteListPtr PrimitiveImpl::AddBatchingSpriteList (SpriteMode mode, const math::vec3f& up)
{
  try
  {
    switch (mode)
    {
      case SpriteMode_Billboard:
      case SpriteMode_Oriented:
      case SpriteMode_OrientedBillboard:
        break;
      default:
        throw xtl::make_argument_exception ("", "mode", mode);
    }

    SpriteListPtr list (create_batching_sprite_list (&impl->buffers->BatchingManager (), impl->material_manager, mode, up), false);

    AddSimplePrimitiveList (list.get (), SimplePrimitiveListType_Sprite);

    return list;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveImpl::AddBatchingSpriteList");
    throw;
  }
}

void PrimitiveImpl::RemoveSpriteList (const SpriteListPtr& list)
{
  RemoveSimplePrimitiveList (list.get ());
}

void PrimitiveImpl::RemoveAllSpriteLists ()
{
  RemoveAllSimplePrimitiveLists (SimplePrimitiveListType_Sprite);
}

/*
    ������ � �������
*/

size_t PrimitiveImpl::LineListsCount ()
{
  return impl->line_lists_count;
}

LineListPtr PrimitiveImpl::AddStandaloneLineList (MeshBufferUsage vb_usage, MeshBufferUsage ib_usage)
{
  try
  {
    switch (vb_usage)
    {
      case MeshBufferUsage_Static:
      case MeshBufferUsage_Dynamic:
      case MeshBufferUsage_Stream:
      default:
        throw xtl::make_argument_exception ("", "vb_usage", vb_usage);
    }

    switch (ib_usage)
    {
      case MeshBufferUsage_Static:
      case MeshBufferUsage_Dynamic:
      case MeshBufferUsage_Stream:
      default:
        throw xtl::make_argument_exception ("", "ib_usage", ib_usage);
    }

    LineListPtr list (create_standalone_line_list (impl->material_manager, vb_usage, ib_usage), false);

    AddSimplePrimitiveList (list.get (), SimplePrimitiveListType_Line);

    return list;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveImpl::AddStandaloneLineList");
    throw;
  }
}

LineListPtr PrimitiveImpl::AddBatchingLineList ()
{
  try
  {
    LineListPtr list (create_batching_line_list (&impl->buffers->BatchingManager (), impl->material_manager), false);

    AddSimplePrimitiveList (list.get (), SimplePrimitiveListType_Line);

    return list;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveImpl::AddBatchingLineList");
    throw;
  }
}

void PrimitiveImpl::RemoveLineList (const LineListPtr& list)
{
  RemoveSimplePrimitiveList (list.get ());
}

void PrimitiveImpl::RemoveAllLineLists ()
{
  RemoveAllSimplePrimitiveLists (SimplePrimitiveListType_Line);
}

/*
    ������ ��������� ����������
*/

size_t PrimitiveImpl::RendererPrimitiveGroupsCount ()
{
  UpdateCache ();
  
  return impl->render_groups.size ();
}

RendererPrimitiveGroup* PrimitiveImpl::RendererPrimitiveGroups ()
{
  UpdateCache ();
  
  if (impl->render_groups.empty ())
    return 0;
  
  return &impl->render_groups [0];
}

/*
    ��������� ������������ ����������
*/

void PrimitiveImpl::FillDynamicPrimitiveStorage (DynamicPrimitiveEntityStorage& storage)
{
  try
  {
    for (SimplePrimitiveListArray::iterator iter=impl->entity_dependent_dynamic_primitive_lists.begin (), end=impl->entity_dependent_dynamic_primitive_lists.end (); iter!=end; ++iter)
    {
      SimplePrimitiveListImplBase& list = *iter->list;

      if (DynamicPrimitivePtr primitive = storage.FindPrimitive (&list, true))
      {
        primitive->UpdateCache ();
        continue;
      }

      DynamicPrimitivePtr primitive (list.CreateDynamicPrimitiveInstanceCore (), false);

      primitive->UpdateCache ();

      storage.AddPrimitive (primitive, &list);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveImpl::FillDynamicPrimitiveStorage");
    throw;
  }
}

/*
    ����������� ������������ ����������
*/

void PrimitiveImpl::AddSimplePrimitiveList (SimplePrimitiveListImplBase* list, int type)
{
  RendererPrimitive* primitive = list->StandaloneRendererPrimitive ();

  if (primitive) impl->entity_independent_dynamic_primitive_lists.push_back (SimplePrimitiveListDesc (list, (SimplePrimitiveListType)type, primitive));
  else           impl->entity_dependent_dynamic_primitive_lists.push_back (SimplePrimitiveListDesc (list, (SimplePrimitiveListType)type, primitive));

  AttachCacheSource (*list);

  switch (type)
  {
    case SimplePrimitiveListType_Sprite:
      impl->sprite_lists_count++;
      break;
    case SimplePrimitiveListType_Line:
      impl->line_lists_count++;
      break;
    default:
      break;
  }
}

void PrimitiveImpl::RemoveSimplePrimitiveList (SimplePrimitiveListImplBase* list)
{
  if (!list)
    return;

  SimplePrimitiveListArray* arrays [2] = {&impl->entity_dependent_dynamic_primitive_lists, &impl->entity_independent_dynamic_primitive_lists};

  for (size_t i=0; i<sizeof (arrays) / sizeof (*arrays); i++)
  {
    SimplePrimitiveListArray& lists = *arrays [i];

    for (SimplePrimitiveListArray::iterator iter=lists.begin (), end=lists.end (); iter!=end; ++iter)
      if (iter->list == list)
      {
        switch (iter->type)
        {
          case SimplePrimitiveListType_Sprite:
            impl->sprite_lists_count--;
            break;
          case SimplePrimitiveListType_Line:
            impl->line_lists_count--;
            break;
          default:
            break;
        }

        DetachCacheSource (*list);

        lists.erase (iter);

        return;
      }
  }
}

void PrimitiveImpl::RemoveAllSimplePrimitiveLists (int type)
{
  switch (type)
  {
    case SimplePrimitiveListType_Sprite:
    case SimplePrimitiveListType_Line:
      break;
    default:
      return;
  }

  SimplePrimitiveListArray* arrays [2] = {&impl->entity_dependent_dynamic_primitive_lists, &impl->entity_independent_dynamic_primitive_lists};

  for (size_t i=0; i<sizeof (arrays) / sizeof (*arrays); i++)
  {
    SimplePrimitiveListArray& lists = *arrays [i];

    for (SimplePrimitiveListArray::iterator iter=lists.begin (); iter!=lists.end ();)
      if (iter->type == type)
      {
        DetachCacheSource (*iter->list);

        lists.erase (iter);
      }
      else ++iter;
  }  
}

/*
    ���������� ������������
*/

void PrimitiveImpl::UpdateCacheCore ()
{
  try
  {
    bool has_debug_log = impl->device_manager->Settings ().HasDebugLog ();
     
    if (has_debug_log)
      impl->log.Printf ("Update primitive '%s' cache (id=%u)", impl->name.c_str (), impl->Id ());    
    
    impl->render_groups.clear ();
    impl->render_groups.reserve (impl->meshes.size () + 1);

    impl->cached_entity_independent_dynamic_primitives.clear ();
    impl->cached_entity_independent_dynamic_primitives.reserve (impl->entity_independent_dynamic_primitive_lists.size ());
    
    for (MeshArray::iterator iter=impl->meshes.begin (), end=impl->meshes.end (); iter!=end; ++iter)
    {
      Mesh& mesh = **iter;
      
      if (!mesh.IsValid ())
        continue;

      impl->render_groups.push_back (mesh.cached_group);
    }

    for (SimplePrimitiveListArray::iterator iter=impl->entity_independent_dynamic_primitive_lists.begin (), end=impl->entity_independent_dynamic_primitive_lists.end (); iter!=end; ++iter)
    {
      SimplePrimitiveListImplBase& list             = *iter->list;
      RendererPrimitive*           cached_primitive = iter->primitive;

      if (!cached_primitive)
        continue;

      impl->cached_entity_independent_dynamic_primitives.push_back (*cached_primitive);
    }

    if (!impl->cached_entity_independent_dynamic_primitives.empty ())
    {
      RendererPrimitiveGroup group;

      group.primitives_count = impl->cached_entity_independent_dynamic_primitives.size ();
      group.primitives       = &impl->cached_entity_independent_dynamic_primitives [0];

      impl->render_groups.push_back (group);
    }
    
    InvalidateCacheDependencies ();
    
    if (has_debug_log)
      impl->log.Printf ("...primitive cache updated");
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveImpl::UpdateCacheCore");
    throw;
  }
}

void PrimitiveImpl::ResetCacheCore ()
{
  bool has_debug_log = impl->device_manager->Settings ().HasDebugLog ();
     
  if (has_debug_log)
    impl->log.Printf ("Reset primitive '%s' cache (id=%u)", impl->name.c_str (), impl->Id ());

  for (MeshArray::iterator iter=impl->meshes.begin (), end=impl->meshes.end (); iter!=end; ++iter)
  {
    Mesh& mesh = **iter;        
    
    mesh.ResetCache ();
    
    for (MeshPrimitiveArray::iterator iter=mesh.primitives.begin (), end=mesh.primitives.end (); iter!=end; ++iter)
      (*iter)->ResetCache ();
  }
}
