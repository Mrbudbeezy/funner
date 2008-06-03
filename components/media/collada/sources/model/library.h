#ifndef MEDIALIB_COLLADA_LIBRARY_IMPL_HEADER
#define MEDIALIB_COLLADA_LIBRARY_IMPL_HEADER

namespace media
{

namespace collada
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Item> class LibraryImpl: public ILibrary<Item>
{
  typedef ILibrary<Item> Base;
  public:
    typedef typename Base::Iterator      Iterator;
    typedef typename Base::ConstIterator ConstIterator;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� � ���������� / �������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size    () const { return impl.Size (); }
    bool   IsEmpty () const { return impl.IsEmpty (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������� �� ����� (� ������ ���������� - ����������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    const Item& operator [] (const char* id) const
    {
      if (!id)
        common::raise_null_argument ("media::collada::ILibrary::operator []", "id");
        
      const Item* item = impl.Find (id);
      
      if (!item)
        common::raise_invalid_argument ("media::collada::ILibrary::operator []", "id", id);
        
      return *item;
    }

    Item& operator [] (const char* id)
    { 
      return const_cast<Item&> (const_cast<const LibraryImpl&> (*this).LibraryImpl::operator [] (id));
    }    

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������� �� ����� (� ������ ���������� ���������� 0)
///////////////////////////////////////////////////////////////////////////////////////////////////
    Item*       Find (const char* id)       { return impl.Find (id); }
    const Item* Find (const char* id) const { return impl.Find (id); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Iterator      CreateIterator ()       { return impl.CreateIterator (); }
    ConstIterator CreateIterator () const { return impl.CreateConstIterator (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* ItemId (const ConstIterator& i) const { return ResourceLibrary<Item>::ItemId (i); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� / �������� ��������� � ���������� / ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Insert (const char* id, Item& item) { impl.Insert (id, item); }
    void Remove (const char* id)             { impl.Remove (id); }
    void Clear  ()                           { impl.Clear (); }

  private:
    typedef ResourceLibrary<Item> Impl;
    
    Impl impl;
};

}

}

#endif
