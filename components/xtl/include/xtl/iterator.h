#ifndef XTL_ITERATOR_HEADER
#define XTL_ITERATOR_HEADER

#include <typeinfo>
#include <stl/iterator_traits.h>

namespace stl
{

//forward declaration
template <class T1, class T2> struct pair;

}

namespace xtl
{

namespace detail
{

//implementation forwards
template <class T> struct iterator_interface;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������:
///  iterator_exception - ������� ����� ���������� ���������
///  bad_iterator_operation - �������� �������� (��������� ����������������� ��������� �� ��������� ��������� ��������� ��������)
///  bad_iterator_dereference - ������� ������ �������� � ������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct iterator_exception: public std::exception
{
  const char* what () const throw () { return "xtl::iterator_exception"; }
};

struct bad_iterator_operation: public iterator_exception
{
  const char* what () const throw () { return "xtl::bad_iterator_operation"; }
};

struct bad_iterator_dereference: public bad_iterator_operation
{
  const char* what () const throw () { return "xtl::bad_iterator_dereference"; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct iterator_value_type          { typedef T type; };
template <class T> struct iterator_value_type<const T> { typedef T type; };

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class iterator
{
  template <class T1> friend class iterator;
  typedef bool (iterator::*safe_bool)() const;
  public:
    typedef typename iterator_value_type<T>::type value_type;
    typedef size_t                                size_type;
    typedef ptrdiff_t                             difference_type;
    typedef T*                                    pointer;
    typedef T&                                    reference;
    typedef stl::bidirectional_iterator_tag       iterator_category;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    iterator  ();
    iterator  (const iterator&);
    ~iterator ();
    
    template <class Iter>           iterator (Iter iter);
    template <class Iter, class Fn> iterator (Iter iter, Fn selector);
    template <class Iter>           iterator (Iter iter, Iter first, Iter last);
    template <class Iter, class Fn> iterator (Iter iter, Iter first, Iter last, Fn selector);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    iterator& operator = (const iterator&);

    template <class Iter> iterator& operator = (Iter);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void clear ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool empty () const;

    operator safe_bool () const { return empty () ? 0 : &iterator::empty; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    reference operator  * () const;
    pointer   operator -> () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� / ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    iterator& operator ++ ();
    iterator& operator -- ();
    iterator  operator ++ (int);
    iterator  operator -- (int);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ���� ��������� ��������� � ����������� ��������� �� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const std::type_info& target_type () const;

    template <class Iter>       Iter* target ();
    template <class Iter> const Iter* target () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� �� �������� ������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Iter> bool contains (const Iter&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const iterator&) const;
    bool operator != (const iterator&) const;    

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void swap (iterator&);
    
  private:
    typedef detail::iterator_interface<value_type> iterator_interface;
    
    template <class Iter> static iterator_interface* create_dispatch (const Iter&, const Iter*);
                          static iterator_interface* create_dispatch (const iterator<value_type>&, const iterator<value_type>*);

  private:
    iterator_interface* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Iter, class T> bool operator == (const iterator<T>&, const Iter&);
template <class Iter, class T> bool operator != (const iterator<T>&, const Iter&);
template <class Iter, class T> bool operator == (const Iter&, const iterator<T>&);
template <class Iter, class T> bool operator != (const Iter&, const iterator<T>&);
template <class T>             bool operator == (const iterator<T>&, const iterator<const T>&);
template <class T>             bool operator == (const iterator<const T>&, const iterator<T>&);
template <class T>             bool operator != (const iterator<T>&, const iterator<const T>&);
template <class T>             bool operator != (const iterator<const T>&, const iterator<T>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> void swap (iterator<T>&, iterator<T>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Iter>          iterator<typename stl::iterator_traits<Iter>::value_type> make_iterator (Iter iter);
template <class T, class Iter> iterator<T>                                               make_iterator (Iter iter);

template <class Iter, class Fn>
iterator<typename stl::iterator_traits<Iter>::value_type> make_iterator (Iter iter, Fn selector);

template <class T, class Iter, class Fn>
iterator<T> make_iterator (Iter iter, Fn selector);

template <class Iter>
iterator<typename stl::iterator_traits<Iter>::value_type> make_iterator (Iter iter, Iter first, Iter last);

template <class T, class Iter>
iterator<T> make_iterator (Iter iter, Iter first, Iter last);

template <class Iter, class Fn>
iterator<typename stl::iterator_traits<Iter>::value_type> make_iterator (Iter iter, Iter first, Iter last, Fn selector);

template <class T, class Iter, class Fn>
iterator<T> make_iterator (Iter iter, Iter first, Iter last, Fn selector);

#include <xtl/detail/iterator.inl>

}

#endif
