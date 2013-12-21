#include "shared.h"

using namespace render::manager;

/*
    �������� ���������� ������ ������������ ����������
*/

template <class T> struct DynamicPrimitiveListImpl<T>::Impl
{
  typedef typename DynamicPrimitiveListImpl<T>::Item Item;
  typedef stl::vector<Item>                          ItemArray;

  MaterialManagerPtr            material_manager; //�������� ����������
  stl::auto_ptr<MaterialProxy>  material;         //��������
  ItemArray                     items;            //�������� ��������

/// �����������
  Impl (const MaterialManagerPtr& in_material_manager)
    : material_manager (in_material_manager)
  {
    if (!material_manager)
      throw xtl::make_null_argument_exception ("render::manager::DynamicPrimitiveListImpl<T>::DynamicPrimitiveListImpl", "material_manager");
  }
};

/*
    ����������� / ����������
*/

template <class T>
DynamicPrimitiveListImpl<T>::DynamicPrimitiveListImpl (const MaterialManagerPtr& material_manager)
  : impl (new Impl (material_manager))
{
}

template <class T>
DynamicPrimitiveListImpl<T>::~DynamicPrimitiveListImpl ()
{
}

/*
    ���������� ����������
*/

template <class T>
size_t DynamicPrimitiveListImpl<T>::Size ()
{
  return impl->items.size ();
}

/*
    ��������
*/
template <class T>
void DynamicPrimitiveListImpl<T>::SetMaterial (const char* material)
{
  try
  {
      //�������� ������������ ����������

    if (!material)
      throw xtl::make_null_argument_exception ("", "material");

      //������ ���������

    MaterialProxy proxy = impl->material_manager->GetMaterialProxy (material);

    if (impl->material)
    {
      impl->material->DetachCacheHolder (*this);

      *impl->material = proxy;
    }
    else
    {
      impl->material.reset (new MaterialProxy (proxy));
    }    

    try
    {
      proxy.AttachCacheHolder (*this);

      InvalidateCacheDependencies ();
    }
    catch (...)
    {
      impl->material.reset ();
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::DynamicPrimitiveListImpl<T>::SetMaterial");
    throw;
  }
}

template <class T>
const char* DynamicPrimitiveListImpl<T>::Material () 
{
  return impl->material ? impl->material->Name () : "";
}

/*
    ���������� / ���������� ����������
*/

template <class T>
size_t DynamicPrimitiveListImpl<T>::Add (size_t count, const Item* items)
{
  try
  {
      //�������� ������������ ����������

    if (!count)
      return impl->items.size ();

    if (!items)
      throw xtl::make_null_argument_exception ("", "items");

      //����������

    impl->items.insert (impl->items.end (), items, items + count);

    InvalidateCacheDependencies ();

    return impl->items.size () - 1;    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::DynamicPrimitiveListImpl<T>::Add");
    throw;
  }
}

template <class T>
void DynamicPrimitiveListImpl<T>::Update (size_t first, size_t count, const Item* src_items)
{
  try
  {
      //�������� ������������ ����������

    if (!count)
      return;

    if (!src_items)
      throw xtl::make_null_argument_exception ("", "items");

    if (first >= impl->items.size ())
      throw xtl::make_range_exception ("", "first", first, impl->items.size ());
    
    if (impl->items.size () - first < count)
      throw xtl::make_range_exception ("", "count", count, impl->items.size () - first);

      //���������� ��������

    const Item* src = src_items;
    Item*       dst = &impl->items [first];

    for (size_t i=0; i<count; i++, src++, dst++)
      *dst = *src;

    InvalidateCacheDependencies ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::DynamicPrimitiveListImpl<T>::Update");
    throw;
  }
}

/*
    �������� ����������
*/

template <class T>
void DynamicPrimitiveListImpl<T>::Remove (size_t first, size_t count) 
{
  if (first >= impl->items.size ())
    return;

  if (impl->items.size () - first < count)
    count = impl->items.size () - first;

  if (!count)
    return;

  impl->items.erase (impl->items.begin () + first, impl->items.begin () + first + count);

  InvalidateCacheDependencies ();
}

template <class T>
void DynamicPrimitiveListImpl<T>::Clear ()
{
  impl->items.clear ();

  InvalidateCacheDependencies ();
}

/*
    ������������� ������������
*/

template <class T>
void DynamicPrimitiveListImpl<T>::Reserve (size_t count)
{
  size_t capacity = impl->items.capacity ();

  impl->items.reserve (count);

  if (capacity < count)
    InvalidateCacheDependencies ();
}

template <class T>
size_t DynamicPrimitiveListImpl<T>::Capacity () 
{
  return impl->items.capacity (); 
}

/*
    �������� �� ���������
*/

template <class T>
void DynamicPrimitiveListImpl<T>::RemoveFromPrimitive (PrimitiveImpl&)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ���������������
*/

template class DynamicPrimitiveListImpl<Sprite>;
template class DynamicPrimitiveListImpl<Line>;
