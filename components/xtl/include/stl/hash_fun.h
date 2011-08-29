#ifndef __MYSTL__HASH_FUN__
#define __MYSTL__HASH_FUN__

#include <string.h>
#include <wchar.h> //cwchar ����� �������� ���������� �� mingw 3.4.5

#include <stl/config.h>

#ifndef __MYSTL_STANDALONE__
  #include <common/hash.h>
#endif

namespace stl
{

//forward declaration
template <class T1, class T2> struct pair;

//////////////////////////////////////////////////////////////////////////////////////////////////
///���� �����������
//////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct hash_key
{
  public:
    typedef T value_type;

//////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
//////////////////////////////////////////////////////////////////////////////////////////////////
    hash_key (const value_type& value);

//////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ����
//////////////////////////////////////////////////////////////////////////////////////////////////
    size_t get_hash () const;

//////////////////////////////////////////////////////////////////////////////////////////////////
///���������
//////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const hash_key&) const;
    bool operator != (const hash_key&) const;

  private:
    size_t hash;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///������� �����������
//////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> size_t hash (const T*);
template <class T> size_t hash (const hash_key<T>&);

size_t hash (int x);
size_t hash (long x);
size_t hash (unsigned int x);
size_t hash (unsigned long x);
size_t hash (const char*);
size_t hash (const unsigned char*);
size_t hash (const wchar_t*);

//��������� ����������������� ����
template <class T> size_t hash (const T&, size_t previous_hash);

//��������� ���� ����
template <class T1, class T2> size_t hash (const pair<T1, T2>&);

//////////////////////////////////////////////////////////////////////////////////////////////////
///������� �����������
//////////////////////////////////////////////////////////////////////////////////////////////////
template <class Key> struct hash_fun
{
  size_t operator () (const Key& x) const { return hash (x); }
};

#include <stl/detail/hash.inl>

}

#endif
