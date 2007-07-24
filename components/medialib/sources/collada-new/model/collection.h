#include <media/collada/utility.h>
#include <common/strlib.h>
#include <stl/vector>
#include <xtl/functional>

namespace medialib
{

namespace collada
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Item> const char* get_collection_subname ();

template <> const char* get_collection_subname<Surface> () { return "surfaces"; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Item, class StorableItem=Item, bool NeedDestroy=false>
class Collection: public ICollection<Item>
{
  public:
    enum { DEFAULT_ITEMS_RESERVE = 8 }; //���������� ��������� ������������� �� ���������
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Collection (Entity& in_owner)
      : owner (in_owner), name (common::format ("%s#%s", owner.EntityId (), get_collection_subname<Item> ()))
    {
      items.reserve (DEFAULT_ITEMS_RESERVE);
    }

    ~Collection ()
    {
      Clear ();
    }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ModelImpl* Owner () const { return owner.Owner (); }
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* EntityId () const { return name.c_str (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� / �������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size    () const { return items.size (); }
    bool   IsEmpty () const { return items.empty (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const Item& operator [] (size_t index) const
    {
      if (index >= items.size ())
        common::RaiseOutOfRange ("medialib::collada::Collection::operator []", "index", index, items.size ());

      return *items [index];
    }
    
    Item& operator [] (size_t index)
    {
      return const_cast<Item&> (const_cast<const Collection&> (*this) [index]);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������� � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual int Find (Item& item) const
    {
      for (ItemArray::const_iterator i=items.begin (); i!=items.end (); ++i)
        if (*i == &item)
          return i - items.begin ();
          
      return -1;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t InsertCore (StorableItem& item)
    {
      items.push_back (&item);
      
      return items.size () - 1;
    }

    size_t Insert (Item& item)
    {
      if (item.Owner () != Owner ())
        raise_incompatible ("medialib::collada::Collection::Insert", item, *this);

      InsertCore (item);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
   void Remove (size_t index)
   {
     if (index >= items.size ())
       common::RaiseOutOfRange ("medialib::collada::Collection::Remove", "index", index, items.size ());

     if (NeedDestroy)
       delete items [index];

     items.erase (items.begin () + index);
   }
   
   void Remove (Item& item)
   {
     int index = Find (item);

     if (index != -1)
       Remove (index);
   }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear ()
    {
      if (NeedDestroy)
      {
        for (ItemArray::iterator i=items.begin (); i!=items.end (); ++i)
          delete *i;
      }

      items.clear ();
    }    

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ForEach (const xtl::function<void (Item&)>& fn)
    {
      for (ItemArray::iterator i=items.begin (); i!=items.end (); ++i)
        fn (**i);
    }

    void ForEach (const xtl::function<void (const Item&)>& fn) const
    {
      for (ItemArray::const_iterator i=items.begin (); i!=items.end (); ++i)
        fn (**i);
    }    

  private:
    typedef stl::vector<StorableItem*> ItemArray;

  private:
    Entity&     owner; //��������
    stl::string name;  //��� ���������
    ItemArray   items; //��������
};

}

}
