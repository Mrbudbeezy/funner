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
        common::RaiseNullArgument ("media::collada::ILibrary::operator []", "id");
        
      const Item* item = impl.Find (id);
      
      if (!item)
        common::RaiseInvalidArgument ("media::collada::ILibrary::operator []", "id", id);
        
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
    ConstIterator CreateIterator () const { return impl.CreateIterator (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� / �������� ��������� � ���������� / ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Insert (Item& item, media::CloneMode mode) { impl.Insert (item.Id (), item, mode); }
    void Remove (const char* item_id)               { impl.Remove (item_id); }
    void Clear  ()                                  { impl.Clear (); }

  private:
    typedef ResourceLibrary<Item> Impl;
    
    Impl impl;
};

}

}

#endif
