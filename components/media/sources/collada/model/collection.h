#ifndef MEDIALIB_COLLADA_COLLECTION_IMPL_HEADER
#define MEDIALIB_COLLADA_COLLECTION_IMPL_HEADER

namespace media
{

namespace collada
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ItemSelector
{
  template <class T> T&       operator () (ResourceHolder<T>& value) const       { return value.Resource (); }
  template <class T> const T& operator () (const ResourceHolder<T>& value) const { return value.Resource (); }  
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Item>
class CollectionImpl: public ICollection<Item>
{
  enum { DEFAULT_ITEMS_RESERVE = 8 }; //���������� ��������� ������������� �� ���������  
  typedef ICollection<Item> Base;  
  public:
    typedef typename Base::Iterator      Iterator;
    typedef typename Base::ConstIterator ConstIterator;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    CollectionImpl ()
    {
      items.reserve (DEFAULT_ITEMS_RESERVE);
    }
    
    CollectionImpl (const CollectionImpl& collection)
    {
      items.reserve (collection.items.size ());
    
      for (ItemArray::const_iterator i=collection.items.begin (), end=collection.items.end (); i!=end; ++i)
        items.push_back (ResourceHolder<Item> (*i, ForceClone));
    }
    
    CollectionImpl& operator = (const CollectionImpl& collection)
    {
      CollectionImpl (collection).items.swap (items);
      return *this;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� / �������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size    () const { return items.size ();  }
    bool   IsEmpty () const { return items.empty (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const Item& operator [] (size_t index) const
    {
      if (index >= items.size ())
        common::RaiseOutOfRange ("media::collada::ICollection::operator []", "index", index, items.size ());

      return items [index].Resource ();
    }
    
    Item& operator [] (size_t index)
    {
      return const_cast<Item&> (const_cast<const CollectionImpl&> (*this) [index]);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Iterator CreateIterator ()
    {
      return Iterator (items.begin (), items.begin (), items.end (), ItemSelector ());
    }
    
    ConstIterator CreateIterator () const
    {
      return ConstIterator (items.begin (), items.begin (), items.end (), ItemSelector ());
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Insert (Item& item, media::CloneMode mode)
    {
      items.push_back (ResourceHolder<Item> (item, mode));
      
      return items.size () - 1;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
   void Remove (size_t index)
   {
     if (index >= items.size ())
       return;

     items.erase (items.begin () + index);
   }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear ()
    {
      items.clear ();
    }

  private:
    typedef stl::vector<ResourceHolder<Item> > ItemArray;

  private:
    ItemArray items; //��������
};

}

}

#endif
