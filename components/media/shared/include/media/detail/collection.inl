namespace
{

const size_t DEFAULT_ITEMS_RESERVE = 8;

}

/*
   ������������ / ������������
*/

template <class Item, class BaseClass>
CollectionImpl<Item, BaseClass>::CollectionImpl ()
{
  items.reserve (DEFAULT_ITEMS_RESERVE);
}
    
template <class Item, class BaseClass>
CollectionImpl<Item, BaseClass>::CollectionImpl (const CollectionImpl& collection)
  : items (collection.items)
  {}
    
template <class Item, class BaseClass>
CollectionImpl<Item, BaseClass>& CollectionImpl<Item, BaseClass>::operator = (const CollectionImpl& collection)
{
  CollectionImpl (collection).items.swap (items);
  return *this;
}

/*
   ���������� ��������� / �������� �� �������
*/

template <class Item, class BaseClass>
size_t CollectionImpl<Item, BaseClass>::Size () const
{
  return items.size ();
}

template <class Item, class BaseClass>
bool CollectionImpl<Item, BaseClass>::IsEmpty () const
{
  return items.empty ();
}

/*
   ������ � �������� �� �������
*/

template <class Item, class BaseClass>
const Item& CollectionImpl<Item, BaseClass>::operator [] (size_t index) const
{
  if (index >= items.size ())
    throw xtl::make_range_exception ("media::collada::ICollection::operator []", "index", index, items.size ());

  return items [index];
}
    
template <class Item, class BaseClass>
Item& CollectionImpl<Item, BaseClass>::operator [] (size_t index)
{
  return const_cast<Item&> (const_cast<const CollectionImpl&> (*this) [index]);
}

/*
   ������ � �������� �� ����� (� ������ ���������� - ����������)
*/

template <class Item, class BaseClass>
Item& CollectionImpl<Item, BaseClass>::operator [] (const char* name)
{
  return const_cast<Item&> (const_cast<const CollectionImpl<Item, BaseClass>&> (*this).CollectionImpl::operator [] (name));
}

template <class Item, class BaseClass>
const Item& CollectionImpl<Item, BaseClass>::operator [] (const char* name) const
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::ICollection::operator []", "name");

  const Item* item = Find (name);

  if (!item)
    throw xtl::make_argument_exception ("media::ICollection::operator []", "name", name);

  return *item;
}

/*
   ����� �������� �� ����� (� ������ ���������� ���������� 0)
*/

template <class Item, class BaseClass>
Item* CollectionImpl<Item, BaseClass>::Find (const char* name)
{
  return const_cast<Item*> (const_cast<const CollectionImpl<Item, BaseClass>&> (*this).Find (name));
}

namespace
{

template <class Item>
bool isNameEqual (const Item& item, const char* name)
{
  return !xtl::xstrcmp (get_name (item), name);
}

}

template <class Item, class BaseClass>
const Item* CollectionImpl<Item, BaseClass>::Find (const char* name) const
{
  if (!name)
    return 0;

  typename CollectionImpl<Item, BaseClass>::ItemArray::const_iterator item = stl::find_if (items.begin (), items.end (), xtl::bind (isNameEqual<Item>, _1, name));

  if (item == items.end ())
    return 0;

  return &(*item);
}

/*
   ��������� ���������
*/

template <class Item, class BaseClass>
typename CollectionImpl<Item, BaseClass>::Iterator CollectionImpl<Item, BaseClass>::CreateIterator ()
{
  return Iterator (items.begin (), items.begin (), items.end ());
}

template <class Item, class BaseClass>
typename CollectionImpl<Item, BaseClass>::ConstIterator CollectionImpl<Item, BaseClass>::CreateIterator () const
{
  return ConstIterator (items.begin (), items.begin (), items.end ());
}

/*
   �������������� ���������
*/

template <class Item, class BaseClass>
size_t CollectionImpl<Item, BaseClass>::Capacity () const
{
  return items.capacity ();
}

template <class Item, class BaseClass>
void CollectionImpl<Item, BaseClass>::Reserve (size_t count)
{
  items.reserve (count);
}

/*
   ���������� �������� � ���������
*/

template <class Item, class BaseClass>
size_t CollectionImpl<Item, BaseClass>::Add (Item& item)
{
  items.push_back (item);

  return items.size () - 1;
}

/*
   �������� �������� �� ���������
*/

template <class Item, class BaseClass>
void CollectionImpl<Item, BaseClass>::Remove (size_t index)
{
  if (index >= items.size ())
    return;

  items.erase (items.begin () + index);
}

/*
   ������� ���������
*/

template <class Item, class BaseClass>
void CollectionImpl<Item, BaseClass>::Clear ()
{
  items.clear ();
}
