/*
    ������ �� ������ � ������ ������ (�������� �� ���� ���������� Loki)
*/

#ifndef DESIGN_TYPE_MANIP_HEADER
#define DESIGN_TYPE_MANIP_HEADER

#ifdef _MSC_VER
  #pragma warning (push)
  #pragma warning (disable : 4244) //'argument' : conversion from 'double' to 'int', possible loss of data //????
#endif

#include <stl/type_traits.h>

namespace design
{

using stl::false_type;
using stl::true_type;

namespace type_manip
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� �����, ������������ ��� ���������� �� ����� � ������������ ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct identity
{
  typedef T result;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �� ���� ������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <bool condition,class A,class B> struct select
{
  typedef A result;
};

template <class A,class B> struct select<false,A,B>
{
  typedef B result;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������ ���� �� ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class A,class B> struct conversion
{
  private:
    typedef char small_type;
    struct big_type { char dummy [2]; };
    
    static big_type   test  (...);
    static small_type test  (B);
    static A          makeA ();
    
  public:  
    enum {
      exists          = sizeof (small_type) == sizeof (test (makeA ())),
      exists_symmetry = exists && conversion<B,A>::exists,
      same_type       = false
    };  
};

template <class T> struct conversion<T,T>       { enum { exists = 1, exists_symmetry = 1, same_type = 1 }; };
template <class T> struct conversion<void,T>    { enum { exists = 1, exists_symmetry = 0, same_type = 0 }; };
template <class T> struct conversion<T,void>    { enum { exists = 1, exists_symmetry = 0, same_type = 0 }; };
template <>        struct conversion<void,void> { enum { exists = 1, exists_symmetry = 1, same_type = 1 }; };

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////

//�������� ������������ A->B
template <class A,class B>
struct conversion_exists
{ 
  enum { result = conversion<A,B>::exists };
};

//�������� ������������ A->B, B->A
template <class A,class B>
struct conversion_symmetry_exists
{ 
  enum { result = conversion<A,B>::exists_symmetry };
}; 

//�������� �������� �� A � B ����� � ��� �� �����
template <class A,class B>
struct equals
{ 
  enum { result = conversion<A,B>::same_type };
}; 

//�������� ��������� ������������ (B ������� ����������� �� A)
template <class A,class B>
struct supersubclass
{
  enum { result = conversion<const B*,const A*>::exists && !conversion<const A*,const void*>::same_type };
};

}

}

#ifdef  _MSC_VER
  #pragma warning (pop)
#endif

#endif
