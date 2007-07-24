#include <media/collada/utility.h>
#include <common/strlib.h>
#include <stl/hash_map>
#include <xtl/functional>

namespace medialib
{

namespace collada
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Item> const char* get_library_name ();

template <> const char* get_library_name<Effect>   () { return "library_effects"; }
template <> const char* get_library_name<Material> () { return "library_materials"; }
template <> const char* get_library_name<Mesh>     () { return "library_meshes"; }
template <> const char* get_library_name<Skin>     () { return "library_skines"; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Item> class ItemLibrary: public ILibrary<Item>
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ItemLibrary (ModelImpl* in_owner) : owner (in_owner), gen_id (0) {}

    ~ItemLibrary ()
    {
      for (ItemMap::iterator i=items.begin (); i!=items.end (); ++i)
        delete i->second;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ModelImpl* Owner () const { return owner; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* EntityId () const { return get_library_name<Item> (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� / �������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size    () const { return items.size (); }
    bool   IsEmpty () const { return items.empty (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������� �� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const Item& operator [] (const char* id) const
    {
      if (!id)
        RaiseNullArgument ("medialib::collada::Library::operator []", "id");
        
      ItemMap::const_iterator iter = items.find (id);
      
      if (iter == items.end ())
        RaiseInvalidArgument ("medialib::collada::Library::operator []", "id", id, "No item with this name in library");
        
      return *iter->second;
    }
    
    Item& operator [] (const char* id)
    {
      return const_cast<Item&> (const_cast<const ItemLibrary&> (*this) [id]);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������� �� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const Item* Find (const char* id) const
    {
      if (!id)
        RaiseNullArgument ("medialib::collada::Library::Find", "id");
        
      ItemMap::const_iterator iter = items.find (id);

      if (iter == items.end ())
        return 0;

      return iter->second;
    }
    
    Item* Find (const char* id)
    {
      return const_cast<Item*> (const_cast<const ItemLibrary&> (*this).Find (id));
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ForEach (const xtl::function<void (Item&)>& fn)
    {
      for (ItemMap::iterator i=items.begin (); i!=items.end (); ++i)
        fn (*i->second);
    }

    void ForEach (const xtl::function<void (const Item&)>& fn) const
    {
      for (ItemMap::const_iterator i=items.begin (); i!=items.end (); ++i)
        fn (*i->second);
    }        

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Flush ()
    {
      RaiseNotImplemented ("medialib::collada::Library::Flush");
    }

  protected:
    class ContructableItem: public Item
    {
      public:
        template <class T1> ContructableItem  (T1& arg1, ItemLibrary& library, const char* id) : Item (arg1, library, id) {}
                            ContructableItem  (ItemLibrary& library, const char* id) : Item (library, id) {}
                            ~ContructableItem () {}
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Insert (ContructableItem* item)
    {
      try
      {
        items [item->EntityId ()] = item;
      }
      catch (...)
      {
        delete item;
        throw;
      }
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Item* CreateCore (const char* id)
    {
      ContructableItem* item = new ContructableItem (*this, id ? id : format ("%s.item%u", get_library_name<Item> (), gen_id++).c_str ());
    
      Insert (item);
      
      return item;
    }
    
    template <class T1> Item* CreateCore (T1& arg1, const char* id)
    {
      ContructableItem* item = new ContructableItem (arg1, *this, id ? id : format ("%s.item%u", get_library_name<Item> (), gen_id++).c_str ());

      Insert (item);

      return item;
    }    
    
  private:
    typedef stl::hash_map<stl::hash_key<const char*>, ContructableItem*> ItemMap;
  
  private:
    ModelImpl* owner;   //��������
    ItemMap    items;   //��������
    size_t     gen_id;  //��������� ����� ��� ������������� ���
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Item> class Library: public ItemLibrary<Item>
{
  public:
    Library (ModelImpl* owner) : ItemLibrary<Item> (owner) {}
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Item& Create (const char* id) { return *CreateCore (id); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <> class Library<Material>: public ItemLibrary<Material>
{
  public:
    Library (ModelImpl* owner) : ItemLibrary<Material> (owner) {}
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Material& Create (Effect& effect, const char* id) { return *CreateCore (effect, id); }
};

}

}
