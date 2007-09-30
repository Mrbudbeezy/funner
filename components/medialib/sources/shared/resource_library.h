#ifndef MEDIALIB_RESOURCE_LIBRARY_HEADER
#define MEDIALIB_RESOURCE_LIBRARY_HEADER

#include <xtl/function.h>
#include <xtl/iterator.h>
#include <stl/hash_map>
#include <media/clone.h>
#include <shared/resource_holder.h>
#include <common/exception.h>

namespace media
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class ResourceLibrary
{
  public:
    typedef T                         Item;
    typedef xtl::iterator<Item>       Iterator;
    typedef xtl::iterator<const Item> ConstIterator;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ResourceLibrary  ();
    ResourceLibrary  (const ResourceLibrary&);
    ~ResourceLibrary ();

    ResourceLibrary& operator = (const ResourceLibrary&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� / �������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size    () const;
    bool   IsEmpty () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Iterator      CreateIterator ();
    ConstIterator CreateIterator () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������� � ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
          Item* Find (const char* name);
    const Item* Find (const char* name) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������/�������� ��������� �� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Insert (const char* name, Item& item, CloneMode mode = CloneMode_Instance);
    void Remove (const char* name);
    void Remove (Item&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (ResourceLibrary&);

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, ResourceHolder<Item> > ItemMap;
   
  private:
    ItemMap items; //�������� ������ ����������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
void swap (ResourceLibrary<T>&, ResourceLibrary<T>&);

#include <shared/detail/resource_library.inl>

}

#endif
