#include "shared.h"

using namespace render;
using namespace render::low_level;

//TODO: SetId - register as named material
//TODO: dynamic texture

namespace
{

/*
    ���������� �����
*/

struct Texmap
{
  TextureProxy texture; //������ ��������
  SamplerProxy sampler; //������� ��������
  
  Texmap (const TextureProxy& in_texture_proxy, const SamplerProxy& in_sampler_proxy)
    : texture (in_texture_proxy)
    , sampler (in_sampler_proxy)
  {
  }
};

}

/*
    �������� ���������� ���������
*/

typedef stl::vector<Texmap> TexmapArray;

struct MaterialImpl::Impl
{
  TextureManagerPtr   texture_manager; //�������� �������
  stl::string         id;              //������������� ���������
  common::PropertyMap properties;      //�������� ���������
  TexmapArray         texmaps;         //���������� �����
  
///�����������
  Impl (const TextureManagerPtr& in_texture_manager)
    : texture_manager (in_texture_manager)
  {
  }
  
///������������ �� ����
  void DetachCache (CacheHolder& holder)
  {
    for (TexmapArray::iterator iter=texmaps.begin (), end=texmaps.end (); iter!=end; ++iter)
    {
      iter->texture.Detach (holder);
      iter->sampler.Detach (holder);
    }
  }
  
///������������� � ����
  void AttachCache (CacheHolder& holder)
  {
    try
    {
      for (TexmapArray::iterator iter=texmaps.begin (), end=texmaps.end (); iter!=end; ++iter)
      {
        iter->texture.Attach (holder);
        iter->sampler.Attach (holder);
      }      
    }
    catch (...)
    {
      DetachCache (holder);
      throw;
    }
  }
};

/*
    ����������� / ����������
*/

MaterialImpl::MaterialImpl (const TextureManagerPtr& texture_manager)
  : impl (new Impl (texture_manager))
{
}

MaterialImpl::~MaterialImpl ()
{
  impl->DetachCache (*this);
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
    
    //TODO: dynamic texture?????
    
  return impl->texmaps [index].texture.Resource ();
}

const char* MaterialImpl::TextureName (size_t index)
{
  if (index >= impl->texmaps.size ())
    throw xtl::make_range_exception ("render::MaterialImpl::TextureName", "index", index, impl->texmaps.size ());

  return impl->texmaps [index].texture.Name ();
}

LowLevelSamplerStatePtr MaterialImpl::Sampler (size_t index)
{
  if (index >= impl->texmaps.size ())
    throw xtl::make_range_exception ("render::MaterialImpl::Sampler", "index", index, impl->texmaps.size ());
    
  return impl->texmaps [index].sampler.Resource ();
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
      
      Texmap new_texmap (impl->texture_manager->GetTextureProxy (texmap.Image ()), impl->texture_manager->GetSamplerProxy (texmap.Sampler ()));
      
      new_texmaps.push_back (new_texmap);
    }    
    
    impl->properties = new_properties;
    
    impl->DetachCache (*this);    

    impl->texmaps.swap (new_texmaps);
    
    impl->AttachCache (*this);
    
    Invalidate ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::MaterialImpl::Update");
    throw;
  }
}
