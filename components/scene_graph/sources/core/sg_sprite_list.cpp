#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    �������� ���������� ������ ��������
*/

typedef xtl::uninitialized_storage<SpriteList::Item> ItemArray;

struct SpriteList::Impl
{
  stl::string material; //��� ���������
  ItemArray   items;    //������ ��������  
};

/*
    ����������� / ����������
*/

SpriteList::SpriteList ()
  : impl (new Impl)
{
}

SpriteList::~SpriteList ()
{
}

/*
    �������� ������ ��������
*/

SpriteList::Pointer SpriteList::Create ()
{
  return Pointer (new SpriteList, false);
}

/*
    ��������
*/

void SpriteList::SetMaterial (const char* material)
{
  if (!material)
    throw xtl::make_null_argument_exception ("scene_graph::SpriteList::SetMaterial", "material");
    
  impl->material = material;
  
  UpdateNotify ();
}

const char* SpriteList::Material () const
{
  return impl->material.c_str ();
}

/*
    ������ ������� ��������
*/

size_t SpriteList::Size () const
{
  return impl->items.size ();
}

/*
    ��������� ������� ������ �������� / �������������� ����� ��� �������� ��������
*/

void SpriteList::Resize (size_t count)
{
  impl->items.resize (count);
  
  UpdateNotify ();
}

void SpriteList::Reserve (size_t count)
{
  impl->items.reserve (count);
}

/*
    ������ ��������
*/

const SpriteList::Item* SpriteList::Data () const
{
  return impl->items.data ();
}

SpriteList::Item* SpriteList::Data ()
{
  return const_cast<Item*> (const_cast<const SpriteList&> (*this).Data ());
}

/*
    ���������� �������� � ������
*/

void SpriteList::Insert (const Item& item)
{
  Insert (1, &item);  
}

void SpriteList::Insert (size_t items_count, const Item* items)
{
  if (items_count && !items)
    throw xtl::make_null_argument_exception ("scene_graph::SpriteList::Insert", "items");
    
  if (!items_count)
    return;

  size_t old_size = impl->items.size ();

  impl->items.resize (impl->items.size () + items_count);

  memcpy (impl->items.data () + old_size, items, items_count * sizeof (Item));

  UpdateNotify ();
}

/*
    ������� ������ ��������
*/

void SpriteList::RemoveAll ()
{
  impl->items.resize (0);
}

/*
    ������������ ���������������
*/

void SpriteList::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}

/*
    ���������� �� ���������� ������
*/

void SpriteList::InvalidateData ()
{
  UpdateNotify ();
}
