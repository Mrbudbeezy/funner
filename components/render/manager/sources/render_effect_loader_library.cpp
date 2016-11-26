#include "shared.h"

using namespace render::manager;
using namespace render::low_level;

/*
===================================================================================================
    EffectLoaderLibrary::Library<T>
===================================================================================================
*/

template <class Ptr>
struct EffectLoaderLibrary::Library<Ptr>::Impl
{
  struct Entry
  {
    stl::string name;
    Ptr         value;

    Entry (const char* in_name, const Ptr& in_value) : name (in_name), value (in_value) {}
  };  

  typedef stl::hash_map<stl::hash_key<const char*>, Entry> ItemMap;
  
  ItemMap items; //элементы библиотеки
};

/*
    Конструктор / деструктор
*/

template <class Ptr>
EffectLoaderLibrary::Library<Ptr>::Library ()
  : impl (new Impl)
{
}

template <class Ptr>
EffectLoaderLibrary::Library<Ptr>::~Library ()
{
}

/*
    Количество элементов
*/

template <class Ptr>
size_t EffectLoaderLibrary::Library<Ptr>::Size ()
{
  return impl->items.size ();
}

/*
    Добавление / удаление элементов
*/

template <class Ptr>
void EffectLoaderLibrary::Library<Ptr>::Add (const char* id, const Item& item)
{
  static const char* METHOD_NAME = "render::manager::EffectLoaderLibrary::Library<T>";

  if (!id)
    throw xtl::make_null_argument_exception (METHOD_NAME, "id");
    
  if (impl->items.find (id) != impl->items.end ())
    throw xtl::make_argument_exception (METHOD_NAME, "id", id, "Item has already registered");
    
  if (!item)
    throw xtl::make_null_argument_exception (METHOD_NAME, "item");

  impl->items.insert_pair (id, typename Impl::Entry (id, item));
}

template <class Ptr>
void EffectLoaderLibrary::Library<Ptr>::Remove (const char* id)
{
  if (!id)
    return;
    
  impl->items.erase (id);
}

template <class Ptr>
void EffectLoaderLibrary::Library<Ptr>::Clear ()
{
  impl->items.clear ();
}

/*
    Поиск элемента
*/

template <class Ptr>
typename EffectLoaderLibrary::Library<Ptr>::Item EffectLoaderLibrary::Library<Ptr>::Find (const char* id)
{
  if (!id)
    return Item ();
    
  typedef typename Impl::ItemMap ItemMap;
  typename ItemMap::iterator iter = impl->items.find (id);
  
  if (iter == impl->items.end ())
    return Item ();
    
  return iter->second.value;
}

/*
    Обход библиотеки
*/

template <class Ptr>
void EffectLoaderLibrary::Library<Ptr>::ForEach (const VisitFunction& fn)
{
  typedef typename Impl::ItemMap ItemMap;

  for (typename ItemMap::iterator iter=impl->items.begin (), end=impl->items.end (); iter!=end; ++iter)
    fn (iter->second.name.c_str (), iter->second.value);
}

/*
    Инстанцирование
*/

template class EffectLoaderLibrary::Library<LowLevelRasterizerStatePtr>;
template class EffectLoaderLibrary::Library<LowLevelBlendStatePtr>;
template class EffectLoaderLibrary::Library<LowLevelDepthStencilStatePtr>;
template class EffectLoaderLibrary::Library<LowLevelSamplerStatePtr>;
template class EffectLoaderLibrary::Library<LowLevelTextureDescPtr>;
template class EffectLoaderLibrary::Library<ProgramPtr>;
template class EffectLoaderLibrary::Library<EffectPassPtr>;
template class EffectLoaderLibrary::Library<EffectPtr>;

/*
===================================================================================================
    EffectLoaderLibrary
===================================================================================================
*/

struct EffectLoaderLibrary::Impl
{
  RasterizerStateLibrary     rasterizer_states;           //библиотека состояний растеризатора
  RasterizerStateLibrary     rasterizer_scissor_states;   //библиотека состояний растеризатора с включенным тестом отсечения
  BlendStateLibrary          blend_states;                //библиотека состояний уровня смешивания цветов
  DepthStencilStateLibrary   depth_stencil_states;        //библиотека состояний уровня отсечения
  SamplerStateLibrary        sampler_states;              //библиотека состояний текстурных сэмплеров
  TextureDescLibrary         texture_descs;               //библиотека описателей текстур
  ProgramLibrary             programs;                    //библиотека программ
  media::rfx::ShaderLibrary  shaders;                     //библиотека шейдеров
  EffectPassLibrary          effect_passes;               //библиотека проходов эффекта
  EffectLibrary              effects;                     //библиотека эффектов
};

/*
    Конструктор / деструктор
*/

EffectLoaderLibrary::EffectLoaderLibrary ()
  : impl (new Impl)
{
}

EffectLoaderLibrary::~EffectLoaderLibrary ()
{
}

/*
    Доступные библиотеки
*/

EffectLoaderLibrary::RasterizerStateLibrary& EffectLoaderLibrary::RasterizerStates ()
{
  return impl->rasterizer_states;
}

EffectLoaderLibrary::RasterizerStateLibrary& EffectLoaderLibrary::RasterizerScissorStates ()
{
  return impl->rasterizer_scissor_states;
}

EffectLoaderLibrary::BlendStateLibrary& EffectLoaderLibrary::BlendStates ()
{
  return impl->blend_states;
}

EffectLoaderLibrary::DepthStencilStateLibrary& EffectLoaderLibrary::DepthStencilStates ()
{
  return impl->depth_stencil_states;
}

EffectLoaderLibrary::SamplerStateLibrary& EffectLoaderLibrary::SamplerStates ()
{
  return impl->sampler_states;
}

EffectLoaderLibrary::TextureDescLibrary& EffectLoaderLibrary::TextureDescs ()
{
  return impl->texture_descs;
}

EffectLoaderLibrary::ProgramLibrary& EffectLoaderLibrary::Programs ()
{
  return impl->programs;
}

media::rfx::ShaderLibrary& EffectLoaderLibrary::Shaders ()
{
  return impl->shaders;
}

EffectLoaderLibrary::EffectPassLibrary& EffectLoaderLibrary::EffectPasses ()
{
  return impl->effect_passes;
}

EffectLoaderLibrary::EffectLibrary& EffectLoaderLibrary::Effects ()
{
  return impl->effects;
}
