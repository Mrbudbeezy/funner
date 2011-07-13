#include "shared.h"

using namespace render;
using namespace render::low_level;

//TODO: SetId - register as named material

namespace
{

/*
    ���������
*/

const char* DEFAULT_PROGRAM_NAME = ""; //��� ��������� �� ���������

/*
    ���������� �����
*/

struct Texmap: public xtl::reference_counter, public CacheHolder
{
  MaterialImpl&           material;              //�������� ������ �� ��������
  TextureProxy            texture;               //������ ��������
  SamplerProxy            sampler;               //������� ��������
  bool                    is_dynamic;            //�������� �� �������� ������������  
  TexturePtr              cached_texture;        //�������������� ��������
  LowLevelSamplerStatePtr cached_sampler;        //�������������� �������
  LowLevelTexturePtr      cached_device_texture; //�������������� ��������
  
  Texmap (MaterialImpl& in_material, const TextureProxy& in_texture_proxy, const SamplerProxy& in_sampler_proxy, bool in_is_dynamic)
    : material (in_material)
    , texture (in_texture_proxy)
    , sampler (in_sampler_proxy)
    , is_dynamic (in_is_dynamic)
  {
    material.AttachCacheSource (*this);
    
    texture.AttachCacheHolder (*this);
    sampler.AttachCacheHolder (*this);
  }
  
  void ResetCacheCore ()
  {
    cached_sampler        = LowLevelSamplerStatePtr ();
    cached_device_texture = LowLevelTexturePtr ();
    cached_texture        = TexturePtr ();
  }
  
  void UpdateCacheCore ()
  {
    try
    {
      cached_texture        = is_dynamic ? TexturePtr () : texture.Resource ();
      cached_device_texture = cached_texture ? cached_texture->DeviceTexture () : LowLevelTexturePtr (); 
      cached_sampler        = sampler.Resource ();
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::Texmap::UpdateCacheCore");
      throw;
    }
  }
  
  using CacheHolder::UpdateCache;
  using CacheHolder::ResetCache;
};

}

/*
    �������� ���������� ���������
*/

typedef xtl::intrusive_ptr<Texmap> TexmapPtr;
typedef stl::vector<TexmapPtr>     TexmapArray;
typedef stl::vector<size_t>        TagHashArray;

struct MaterialImpl::Impl: public CacheHolder, public DebugIdHolder
{
  DeviceManagerPtr           device_manager;             //�������� ���������� ���������
  TextureManagerPtr          texture_manager;            //�������� �������
  ProgramManagerPtr          program_manager;            //�������� ��������
  stl::string                id;                         //������������� ���������
  TagHashArray               tags;                       //���� ���������
  ProgramProxy               program;                    //������ ���������
  PropertyBuffer             properties;                 //�������� ���������
  ProgramParametersLayoutPtr material_properties_layout; //������������ ������� ���������
  TexmapArray                texmaps;                    //���������� �����
  bool                       has_dynamic_textures;       //���� �� � ��������� ������������ ��������  
  ProgramPtr                 cached_program;             //�������������� ���������
  LowLevelBufferPtr          cached_properties;          //�������������� ����� ��������
  LowLevelStateBlockPtr      cached_state_block;         //�������������� ���� ���������
  ProgramParametersLayoutPtr cached_properties_layout;   //������������ ������� ��������� � ���������  
  Log                        log;                        //�������� ���������� ���������
  
///�����������
  Impl (const DeviceManagerPtr& in_device_manager, const TextureManagerPtr& in_texture_manager, const ProgramManagerPtr& in_program_manager)
    : device_manager (in_device_manager)
    , texture_manager (in_texture_manager)
    , program_manager (in_program_manager)
    , program (program_manager->GetProgramProxy (DEFAULT_PROGRAM_NAME))
    , properties (in_device_manager)
    , has_dynamic_textures (false)
  {
    AttachCacheSource (properties);
    
    if (device_manager->Settings ().HasDebugLog ())
      log.Printf ("Material created (id=%u)", Id ());
  }
  
///����������
  ~Impl ()
  {
    if (device_manager->Settings ().HasDebugLog ())
      log.Printf ("Material destroyed (id=%u)", Id ());
  }
  
///������ � �����
  void ResetCacheCore ()
  {
    if (device_manager->Settings ().HasDebugLog ())
      log.Printf ("Reset material cache (id=%u)", Id ());

    cached_properties_layout = ProgramParametersLayoutPtr ();
    cached_program           = ProgramPtr ();
    cached_properties        = LowLevelBufferPtr ();
  }
  
  void UpdateCacheCore ()
  {
    try
    {
      bool has_debug_log = device_manager->Settings ().HasDebugLog ();
      
      if (has_debug_log)
        log.Printf ("Update material cache (id=%u)", Id ());
                
      cached_program    = program.Resource ();
      cached_properties = properties.Buffer ();
      
      if (cached_program)
        cached_properties_layout = device_manager->ProgramParametersManager ().GetParameters (&*material_properties_layout, &*cached_program->ParametersLayout (), 0);
      
      render::low_level::IDevice& device = device_manager->Device ();

      device.SSSetConstantBuffer (ProgramParametersSlot_Material, cached_properties.get ());
      device.SSSetConstantBuffer (ProgramParametersSlot_Program, 0);

      if (cached_program)
      {
        LowLevelStateBlockPtr program_state_block = cached_program->StateBlock ();

        if (program_state_block)
          program_state_block->Apply ();
      }            

      if (!cached_state_block)
      {
        StateBlockMask mask;
        
        mask.ss_constant_buffers [ProgramParametersSlot_Material] = true;
        mask.ss_constant_buffers [ProgramParametersSlot_Program]  = true;
        
        cached_state_block = LowLevelStateBlockPtr (device_manager->Device ().CreateStateBlock (mask), false);        
        
        for (size_t i=0, count=stl::min (texmaps.size (), DEVICE_SAMPLER_SLOTS_COUNT); i<count; i++)
        {
          Texmap& texmap = *texmaps [i];
          
          texmap.UpdateCache ();

          if (texmap.cached_device_texture)
          {
            device.SSSetTexture (i, texmap.cached_device_texture.get ());

            mask.ss_textures [i] = true;
          }
          
          device.SSSetSampler (i, texmap.cached_sampler.get ());
          
          mask.ss_samplers [i] = true;
        }
          
        cached_state_block = LowLevelStateBlockPtr (device.CreateStateBlock (mask), false);
      }

      cached_state_block->Capture ();

      if (has_debug_log)
        log.Printf ("...cached updated");
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::MaterialImpl::Impl::UpdateCacheCore");
      throw;
    }
  }
  
  using CacheHolder::UpdateCache;  
  using CacheHolder::ResetCache;
  using CacheHolder::InvalidateCache;
};

/*
    ����������� / ����������
*/

MaterialImpl::MaterialImpl (const DeviceManagerPtr& device_manager, const TextureManagerPtr& texture_manager, const ProgramManagerPtr& program_manager)
  : impl (new Impl (device_manager, texture_manager, program_manager))
{
  AttachCacheSource (*impl);
}

MaterialImpl::~MaterialImpl ()
{
}

/*
    ������������� ���������
*/

const char* MaterialImpl::Id ()
{
  return impl->id.c_str ();
}

void MaterialImpl::SetId (const char* id)
{
  if (!id)
    throw xtl::make_null_argument_exception ("render::MaterialImpl::SetId", "id");
    
  impl->id = id;
}

/*
    ����
*/

size_t MaterialImpl::TagsCount ()
{
  impl->UpdateCache ();
  
  return impl->tags.size ();
}

const size_t* MaterialImpl::Tags ()
{
  impl->UpdateCache ();
  
  if (impl->tags.empty ())
    return 0;
    
  return &impl->tags [0];
}

/*
    ���� ��������� ���������
*/

LowLevelStateBlockPtr MaterialImpl::StateBlock ()
{
  impl->UpdateCache ();
  
  return impl->cached_state_block;
}

/*
    ��������� ��������
*/

ProgramPtr MaterialImpl::Program ()
{
  return impl->cached_program;
}

/*
    ��������� ��������
*/

size_t MaterialImpl::TexturesCount ()
{
  return impl->texmaps.size ();
}

TexturePtr MaterialImpl::Texture (size_t index)
{
  if (index >= impl->texmaps.size ())
    throw xtl::make_range_exception ("render::MaterialImpl::Texture", "index", index, impl->texmaps.size ());
    
  Texmap& texmap = *impl->texmaps [index];  
  
  texmap.UpdateCache ();
    
  return texmap.cached_texture;
}

LowLevelTexturePtr MaterialImpl::DeviceTexture (size_t index)
{
  if (index >= impl->texmaps.size ())
    throw xtl::make_range_exception ("render::MaterialImpl::DeviceTexture", "index", index, impl->texmaps.size ());
    
  Texmap& texmap = *impl->texmaps [index];
    
  texmap.UpdateCache ();
  
  return texmap.cached_device_texture;
}

const char* MaterialImpl::TextureName (size_t index)
{
  if (index >= impl->texmaps.size ())
    throw xtl::make_range_exception ("render::MaterialImpl::TextureName", "index", index, impl->texmaps.size ());
    
  return impl->texmaps [index]->texture.Name ();
}

LowLevelSamplerStatePtr MaterialImpl::Sampler (size_t index)
{
  if (index >= impl->texmaps.size ())
    throw xtl::make_range_exception ("render::MaterialImpl::Sampler", "index", index, impl->texmaps.size ());
    
  Texmap& texmap = *impl->texmaps [index];    
  
  texmap.UpdateCache ();
    
  return texmap.cached_sampler;
}

/*
    ���� �� � ��������� ������������ ��������
*/

bool MaterialImpl::HasDynamicTextures ()
{
  return impl->has_dynamic_textures;
}

/*
    ���������� ���������
*/

void MaterialImpl::Update (const media::rfx::Material& material)
{
  try
  {
    common::PropertyMap new_properties = material.Properties ();
    ProgramProxy        new_program    = impl->program_manager->GetProgramProxy (material.Program ());    
    
    TexmapArray new_texmaps;

    new_texmaps.reserve (material.TexmapCount ());
    
    bool new_has_dynamic_textures = false;
    
    for (size_t i=0, count=material.TexmapCount (); i<count; i++)
    {
      const media::rfx::Texmap& texmap = material.Texmap (i);
      
      bool is_dynamic_image = impl->texture_manager->IsDynamicTexture (texmap.Image ());

      if (is_dynamic_image)
        new_has_dynamic_textures = true;
      
      TexmapPtr new_texmap (new Texmap (*this, impl->texture_manager->GetTextureProxy (texmap.Image ()),
        impl->texture_manager->GetSamplerProxy (texmap.Sampler ()), is_dynamic_image), false);

      new_texmaps.push_back (new_texmap);
    }
    
    TagHashArray new_tag_hashes (material.TagHashes (), material.TagHashes () + material.TagsCount ());
    
    if (new_tag_hashes.empty ())
      impl->log.Printf ("Warning: material '%s' has no tags. Will not be displayed", material.Name ());
    
    ProgramParametersLayoutPtr new_layout = impl->device_manager->ProgramParametersManager ().GetParameters (ProgramParametersSlot_Material, new_properties.Layout ());

    new_program.AttachCacheHolder (*impl);

    impl->program.DetachCacheHolder (*impl);

    impl->program = new_program;

    impl->properties.SetProperties (new_properties);

    impl->texmaps.swap (new_texmaps);
    impl->tags.swap (new_tag_hashes);

    impl->material_properties_layout = new_layout;
    impl->cached_state_block         = LowLevelStateBlockPtr ();
    impl->has_dynamic_textures       = new_has_dynamic_textures;
    
    impl->InvalidateCache (); //TODO: ����� ������
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::MaterialImpl::Update");
    throw;
  }
}

/*
    ��������� ������� ������������ ���������� ��������� ��������
*/

ProgramParametersLayoutPtr MaterialImpl::ParametersLayout ()
{
  try
  {
    impl->UpdateCache ();
    
    return impl->cached_properties_layout;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::MaterialImpl::ParametersLayout");
    throw;
  }
}

/*
    ���������� ������������
*/

void MaterialImpl::UpdateCache ()
{
  CacheSource::UpdateCache ();

  impl->UpdateCache ();
}

void MaterialImpl::ResetCache ()
{
  CacheSource::ResetCache ();

  impl->ResetCache ();
  
  for (TexmapArray::iterator iter=impl->texmaps.begin (), end=impl->texmaps.end (); iter!=end; ++iter)
    (*iter)->ResetCache ();
}
