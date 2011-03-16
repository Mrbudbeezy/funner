#include "shared.h"

using namespace render;
using namespace render::low_level;

//TODO: SetId - register as named material
//TODO: dynamic texture

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
  TexturePtr              cached_texture;        //�������������� ��������
  LowLevelSamplerStatePtr cached_sampler;        //�������������� �������
  LowLevelTexturePtr      cached_device_texture; //�������������� ��������
  
  Texmap (MaterialImpl& in_material, const TextureProxy& in_texture_proxy, const SamplerProxy& in_sampler_proxy)
    : material (in_material)
    , texture (in_texture_proxy)
    , sampler (in_sampler_proxy)
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
      //TODO: dynamic texture
      
      cached_texture        = texture.Resource ();
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
};

}

/*
    �������� ���������� ���������
*/

typedef xtl::intrusive_ptr<Texmap> TexmapPtr;
typedef stl::vector<TexmapPtr>     TexmapArray;

struct MaterialImpl::Impl: public CacheHolder
{
  TextureManagerPtr   texture_manager; //�������� �������
  ShadingManagerPtr   shading_manager; //�������� ��������
  stl::string         id;              //������������� ���������
  ProgramProxy        program;         //������ ���������
  common::PropertyMap properties;      //�������� ���������
  TexmapArray         texmaps;         //���������� �����  
  LowLevelProgramPtr  cached_program;  //�������������� ���������
  
///�����������
  Impl (const TextureManagerPtr& in_texture_manager, const ShadingManagerPtr& in_shading_manager)
    : texture_manager (in_texture_manager)
    , shading_manager (in_shading_manager)
    , program (shading_manager->GetProgramProxy (DEFAULT_PROGRAM_NAME))
  {
  }
  
///������ � �����
  void ResetCacheCore ()
  {
    cached_program = LowLevelProgramPtr ();
  }
  
  void UpdateCacheCore ()
  {
    try
    {
      cached_program = program.Resource ();
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::MaterialImpl::Impl::UpdateCacheCore");
      throw;
    }
  }
  
  using CacheHolder::UpdateCache;  
};

/*
    ����������� / ����������
*/

MaterialImpl::MaterialImpl (const TextureManagerPtr& texture_manager, const ShadingManagerPtr& shading_manager)
  : impl (new Impl (texture_manager, shading_manager))
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
    ���������
*/

LowLevelProgramPtr MaterialImpl::Program ()
{
  impl->UpdateCache ();
  
  return impl->cached_program;
}

/*
    �������� ���������
*/

const common::PropertyMap& MaterialImpl::Properties ()
{
  return impl->properties;
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
    ���������� ���������
*/

void MaterialImpl::Update (const media::rfx::Material& material)
{
  try
  {
    common::PropertyMap new_properties = material.Properties ().Clone ();
    
    TexmapArray new_texmaps;
    
    new_texmaps.reserve (material.TexmapCount ());
    
    for (size_t i=0, count=material.TexmapCount (); i<count; i++)
    {
      const media::rfx::Texmap& texmap = material.Texmap (i);
      
      TexmapPtr new_texmap (new Texmap (*this, impl->texture_manager->GetTextureProxy (texmap.Image ()), impl->texture_manager->GetSamplerProxy (texmap.Sampler ())), false);
      
      new_texmaps.push_back (new_texmap);
    }    
    
    impl->program = impl->shading_manager->GetProgramProxy (material.Program ());
    
    impl->properties = new_properties;
    
    impl->texmaps.swap (new_texmaps);
    
    Invalidate ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::MaterialImpl::Update");
    throw;
  }
}
