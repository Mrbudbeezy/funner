#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    �������� ���������� ������ ��������
*/

typedef xtl::uninitialized_storage<SpriteModel::SpriteDesc> SpriteDescArray;

struct SpriteList::Impl
{
  SpriteDescArray items;    //������ ��������  
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
    ������ ������� ��������
*/

size_t SpriteList::SpritesCount () const
{
  return impl->items.size ();
}

size_t SpriteList::SpriteDescsCountCore ()
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

const SpriteModel::SpriteDesc* SpriteList::Sprites () const
{
  return impl->items.data ();
}

const SpriteModel::SpriteDesc* SpriteList::SpriteDescsCore ()
{
  return impl->items.data ();
}

SpriteModel::SpriteDesc* SpriteList::Sprites ()
{
  return const_cast<SpriteDesc*> (const_cast<const SpriteList&> (*this).Sprites ());
}

/*
    ���������� �������� � ������
*/

void SpriteList::Insert (const SpriteDesc& SpriteDesc)
{
  Insert (1, &SpriteDesc);  
}

void SpriteList::Insert (size_t items_count, const SpriteDesc* items)
{
  if (items_count && !items)
    throw xtl::make_null_argument_exception ("scene_graph::SpriteList::Insert", "items");
    
  if (!items_count)
    return;

  size_t old_size = impl->items.size ();

  impl->items.resize (impl->items.size () + items_count);

  memcpy (impl->items.data () + old_size, items, items_count * sizeof (SpriteDesc));

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
    SpriteModel::AcceptCore (visitor);
}

/*
    ���������� �� ���������� ������
*/

void SpriteList::InvalidateData ()
{
  UpdateNotify ();
}
