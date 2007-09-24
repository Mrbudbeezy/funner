#ifndef XTL_STRIDE_POINTER_HEADER
#define XTL_STRIDE_POINTER_HEADER

#include <stl/iterator_traits.h>

namespace xtl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� � �����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class stride_ptr
{
  typedef T* (stride_ptr::*unspecified_bool_type)() const;
  public:
    typedef stl::bidirectional_iterator_tag iterator_category;
    typedef ptrdiff_t                       difference_type;
    typedef T                               value_type;
    typedef T*                              pointer;
    typedef T&                              reference;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    stride_ptr ();
    stride_ptr (const pointer, difference_type stride=sizeof (value_type));
    stride_ptr (const stride_ptr&);

    template <class T1> stride_ptr (const stride_ptr<T1>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
                        stride_ptr& operator = (const stride_ptr&);
    template <class T1> stride_ptr& operator = (const stride_ptr<T1>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    pointer   get         () const;
    reference operator *  () const;
    pointer   operator -> () const;
    reference operator [] (difference_type i) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    operator unspecified_bool_type () const; //������� ���������� ���� ��� �������� if (p)

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    stride_ptr& operator ++ ();
    stride_ptr& operator -- ();
    stride_ptr& operator += (difference_type);
    stride_ptr& operator -= (difference_type);
    stride_ptr  operator ++ (int);
    stride_ptr  operator -- (int);
    stride_ptr  operator +  (difference_type) const;
    stride_ptr  operator -  (difference_type) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ���������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool equal (const stride_ptr&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T1> bool operator == (const stride_ptr<T1>&) const;
    template <class T1> bool operator != (const stride_ptr<T1>&) const;
    template <class T1> bool operator <  (const stride_ptr<T1>&) const;
    template <class T1> bool operator >  (const stride_ptr<T1>&) const;
    template <class T1> bool operator <= (const stride_ptr<T1>&) const;
    template <class T1> bool operator >= (const stride_ptr<T1>&) const;
    template <class T1> bool operator == (const T1*) const;
    template <class T1> bool operator != (const T1*) const;
    template <class T1> bool operator <  (const T1*) const;
    template <class T1> bool operator >  (const T1*) const;
    template <class T1> bool operator <= (const T1*) const;
    template <class T1> bool operator >= (const T1*) const;

  private:
    char*           ptr;
    difference_type stride;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T1, class T2> bool operator == (const T1*, const stride_ptr<T2>&);
template <class T1, class T2> bool operator != (const T1*, const stride_ptr<T2>&);
template <class T1, class T2> bool operator <  (const T1*, const stride_ptr<T2>&);
template <class T1, class T2> bool operator >  (const T1*, const stride_ptr<T2>&);
template <class T1, class T2> bool operator <= (const T1*, const stride_ptr<T2>&);
template <class T1, class T2> bool operator >= (const T1*, const stride_ptr<T2>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� plain-���������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> T* get_pointer (const stride_ptr<T>&);

#include <xtl/detail/stride_ptr.inl>

}

#endif
