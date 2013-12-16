#include "shared.h"

using namespace render::manager;

/*
    �������� ���������� ��������� ������������ ����������
*/

namespace
{

struct DynamicPrimitiveDesc
{
  DynamicPrimitive*    primitive;
  xtl::auto_connection trackable_connection;

  DynamicPrimitiveDesc () : primitive () { }
};

}

typedef stl::vector<DynamicPrimitiveDesc>   DynamicPrimitiveArray;
typedef stl::vector<RendererPrimitiveGroup> GroupArray;

struct DynamicPrimitiveEntityStorage::Impl
{
  EntityImpl&           entity;             //������ �� ������
  DynamicPrimitiveArray primitives;         //������ ������������ ����������
  GroupArray            groups;             //������ ���������� ����������
  bool                  need_update_groups; //��������� �� ���������� �����

/// �����������
  Impl (EntityImpl& in_entity)
    : entity (in_entity)
    , need_update_groups ()
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
    ����������
*/

void DynamicPrimitiveEntityStorage::UpdateOnPrerender ()
{
  try
  {
      //������� �����

    impl->groups.clear ();

    impl->need_update_groups = false;

      //���������� ���������� � �����

    impl->groups.reserve (impl->primitives.size ());

    for (DynamicPrimitiveArray::iterator iter=impl->primitives.begin (), end=impl->primitives.end (); iter!=end; ++iter)
    {
      iter->primitive->UpdateOnPrerender (impl->entity);

      impl->groups.push_back (iter->primitive->RendererPrimitiveGroup ());
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::DynamicPrimitiveEntityStorage::UpdateOnPrerender");
    throw;
  }
}

void DynamicPrimitiveEntityStorage::UpdateOnRender (FrameImpl& frame)
{
  try
  {
      //���������� ����������

    for (DynamicPrimitiveArray::iterator iter=impl->primitives.begin (), end=impl->primitives.end (); iter!=end; ++iter)
      iter->primitive->UpdateOnRender (impl->entity, frame);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::DynamicPrimitiveEntityStorage::UpdateOnRender");
    throw;
  }
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

    impl->primitives.push_back ();

    DynamicPrimitiveDesc& desc = impl->primitives.back ();

    try
    {
      desc.primitive            = primitive;
      desc.trackable_connection = primitive->connect_tracker (xtl::bind (&DynamicPrimitiveEntityStorage::RemovePrimitive, this, primitive));

      AttachCacheSource (*primitive);

      impl->need_update_groups = true;
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
    if (iter->primitive == primitive)
    {
      DetachCacheSource (*primitive);

      impl->primitives.erase (iter);

      impl->need_update_groups = true;

      return;
    }
}

void DynamicPrimitiveEntityStorage::RemoveAllPrimitives ()
{
  DetachAllCacheSources ();

  impl->primitives.clear ();
  impl->groups.clear ();

  impl->need_update_groups = true;
}

/*
    ������ ���������� ����������
*/

size_t DynamicPrimitiveEntityStorage::RendererPrimitiveGroupsCount ()
{
  if (impl->need_update_groups)
  {
    impl->groups.clear ();

    impl->need_update_groups = true;
  }

  return impl->groups.size ();
}

RendererPrimitiveGroup* DynamicPrimitiveEntityStorage::RendererPrimitiveGroups ()
{
  if (impl->need_update_groups)
  {
    impl->groups.clear ();

    impl->need_update_groups = true;
  }

  if (impl->groups.empty ())
    return 0;

  return &impl->groups [0];
}
