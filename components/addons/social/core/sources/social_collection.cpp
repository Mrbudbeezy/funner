#include "shared.h"

namespace
{

const size_t DEFAULT_ITEMS_RESERVE = 8;

}

namespace social
{

template <class Item>
struct Collection<Item>::Impl : public xtl::reference_counter
{
  stl::vector<Item> items;

  Impl ()
  {
    items.reserve (DEFAULT_ITEMS_RESERVE);
  }
};

/*
   ����������� / ���������� / �����������
*/

template <class Item>
Collection<Item>::Collection ()
  : impl (new Impl)
  {}
    
template <class Item>
Collection<Item>::Collection (const Collection<Item>& source)
  : impl (source.impl)
{
  addref (impl);
}
    
template <class Item>
Collection<Item>::~Collection ()
{
  release (impl);
}

template <class Item>
Collection<Item>& Collection<Item>::operator = (const Collection<Item>& source)
{
  Collection<Item> (source).Swap (*this);

  return *this;
}

/*
   ���������� ��������� / �������� �� �������
*/

template <class Item>
size_t Collection<Item>::Size () const
{
  return impl->items.size ();
}

template <class Item>
bool Collection<Item>::IsEmpty () const
{
  return impl->items.empty ();
}

/*
   ������ � �������� �� �������
*/

template <class Item>
const Item& Collection<Item>::operator [] (size_t index) const
{
  return const_cast<Collection&> (*this) [index];
}

template <class Item>
Item& Collection<Item>::operator [] (size_t index)
{
  if (index >= impl->items.size ())
    throw xtl::make_range_exception ("social::Collection::operator []", "index", index, impl->items.size ());

  return impl->items [index];
}

/*
   �������������� ���������
*/

template <class Item>
size_t Collection<Item>::Capacity () const
{
  return impl->items.capacity ();
}

template <class Item>
void Collection<Item>::Reserve (size_t count)
{
  impl->items.reserve (count);
}

/*
   ���������� �������� � ���������
*/

template <class Item>
size_t Collection<Item>::Add (const Item& item)
{
  impl->items.push_back (item);

  return impl->items.size () - 1;
}

/*
   �������� �������� �� ���������
*/

template <class Item>
void Collection<Item>::Remove (size_t index)
{
  if (index >= impl->items.size ())
    return;

  impl->items.erase (impl->items.begin () + index);
}

/*
   ������� ���������
*/

template <class Item>
void Collection<Item>::Clear ()
{
  impl->items.clear ();
}

/*
   �����
*/

template <class Item>
void Collection<Item>::Swap (Collection<Item>& source)
{
  stl::swap (impl, source.impl);
}

template <class Item>
void swap (Collection<Item>& collection1, Collection<Item>& collection2)
{
  collection1.Swap (collection2);
}

}

namespace social
{

//classes
template class Collection<Achievement>;
template class Collection<Leaderboard>;
template class Collection<Score>;
template class Collection<User>;

//functions
template void swap (Collection<Achievement>&, Collection<Achievement>&);
template void swap (Collection<Leaderboard>&, Collection<Leaderboard>&);
template void swap (Collection<Score>&,       Collection<Score>&);
template void swap (Collection<User>&,        Collection<User>&);

}
