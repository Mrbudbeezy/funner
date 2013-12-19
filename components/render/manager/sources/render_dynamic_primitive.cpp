#include "shared.h"

using namespace render::manager;

/*
    �������� ���������� ��������� ������������ ����������
*/

namespace
{

struct DynamicPrimitiveDesc
{
  xtl::auto_connection trackable_connection;
};

}

typedef stl::vector<DynamicPrimitive*>      DynamicPrimitiveArray;
typedef stl::vector<DynamicPrimitiveDesc>   DescArray;
typedef stl::vector<RendererPrimitiveGroup> GroupArray;

struct DynamicPrimitiveEntityStorage::Impl
{
  EntityImpl&           entity;     //������ �� ������
  DescArray             descs;      //����������� ������������ ����������
  DynamicPrimitiveArray primitives; //������ ������������ ����������
  GroupArray            groups;     //������ ���������� ����������

/// �����������
  Impl (EntityImpl& in_entity)
    : entity (in_entity)
  {
  }
};

/*
    ����������� / ����������
*/

DynamicPrimitiveEntityStorage::DynamicPrimitiveEntityStorage (EntityImpl& entity)
  : impl (new Impl (entity))
{
}

DynamicPrimitiveEntityStorage::~DynamicPrimitiveEntityStorage ()
{
  RemoveAllPrimitives ();
}

/*
    ���������� ����������
*/

size_t DynamicPrimitiveEntityStorage::PrimitivesCount ()
{
  return impl->primitives.size ();
}

/*
    ��������� ����������
*/

DynamicPrimitive** DynamicPrimitiveEntityStorage::Primitives ()
{
  if (impl->primitives.empty ())
    return 0;

  return &impl->primitives [0];
}

/*
    ���������� ������������ ����������
*/

void DynamicPrimitiveEntityStorage::AddPrimitive (DynamicPrimitive* primitive)
{
  try
  {
    if (!primitive)
      throw xtl::make_null_argument_exception ("", "primitive");

    impl->primitives.push_back (primitive);

    try
    {
      impl->descs.push_back ();

      DynamicPrimitiveDesc& desc = impl->descs.back ();

      try
      {  
        desc.trackable_connection = primitive->connect_tracker (xtl::bind (&DynamicPrimitiveEntityStorage::RemovePrimitive, this, primitive));

        AttachCacheSource (*primitive);

        InvalidateCacheDependencies ();
      }
      catch (...)
      {
        impl->descs.pop_back ();
        throw;
      }
    }
    catch (...)
    {
      impl->primitives.pop_back ();
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::DynamicPrimitiveEntityStorage::AddPrimitive");
    throw;
  }
}

void DynamicPrimitiveEntityStorage::RemovePrimitive (DynamicPrimitive* primitive)
{
  if (!primitive)
    return;

  for (DynamicPrimitiveArray::iterator iter=impl->primitives.begin (), end=impl->primitives.end (); iter!=end; ++iter)
    if (*iter == primitive)
    {
      DetachCacheSource (*primitive);

      impl->descs.erase (impl->descs.begin () + (iter - impl->primitives.begin ()));
      impl->primitives.erase (iter);

      InvalidateCacheDependencies ();

      return;
    }
}

void DynamicPrimitiveEntityStorage::RemoveAllPrimitives ()
{
  DetachAllCacheSources ();

  impl->descs.clear ();
  impl->primitives.clear ();

  InvalidateCacheDependencies ();
}

/*
    ������ ���������� ����������
*/

size_t DynamicPrimitiveEntityStorage::RendererPrimitiveGroupsCount ()
{
  UpdateCache ();

  return impl->groups.size ();
}

RendererPrimitiveGroup* DynamicPrimitiveEntityStorage::RendererPrimitiveGroups ()
{
  UpdateCache ();

  if (impl->groups.empty ())
    return 0;

  return &impl->groups [0];
}

/*
    ���������� ������������
*/
      
void DynamicPrimitiveEntityStorage::UpdateCacheCore ()
{
  try
  {
      //������� �����

    impl->groups.clear ();

      //���������� �����

    impl->groups.reserve (impl->primitives.size ());

    for (DynamicPrimitiveArray::iterator iter=impl->primitives.begin (), end=impl->primitives.end (); iter!=end; ++iter)
      impl->groups.push_back ((*iter)->RendererPrimitiveGroup ());

      //���������� ������������

    InvalidateCacheDependencies ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::DynamicPrimitiveEntityStorage::UpdateCacheCore");
    throw;
  }
}

void DynamicPrimitiveEntityStorage::ResetCacheCore ()
{
  impl->groups.clear ();
}
