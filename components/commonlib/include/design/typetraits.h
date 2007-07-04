#ifndef DESIGN_TYPE_TRAITS_HEADER
#define DESIGN_TYPE_TRAITS_HEADER

namespace design
{

namespace type_manip
{

//implementation forwards
template <class T> struct clear_cv;
template <class T> struct clear_ref;
template <class T> struct clear_ptr;
template <class T> struct builtin_type_traits;
template <class T> struct parameter_type;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������� ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct type_traits
{
  typedef typename clear_ref<T>::result          value_type;       //��� ����� ������ ������
  typedef typename clear_ptr<value_type>::result pointee_type;     //��� ����� ������ ���������
  typedef typename clear_cv<value_type>::result  unqualified_type; //��� ����� ������ cv-�������������
  typedef value_type&                            reference;        //������
  typedef const value_type&                      const_reference;  //����������� ������
  typedef value_type*                            pointer;          //���������
  typedef const value_type*                      const_pointer;    //����������� ���������
  typedef typename parameter_type<T>::result     parameter_type;   //��� ���������

  enum {
    is_const     = clear_cv<T>::is_const,
    is_volatile  = clear_cv<T>::is_volatile,
    is_builtin   = builtin_type_traits<T>::is_builtin,
    is_reference = builtin_type_traits<T>::is_reference,
    is_pointer   = builtin_type_traits<T>::is_pointer,
    is_integer   = builtin_type_traits<T>::is_integer,
    is_float     = builtin_type_traits<T>::is_float,
    is_bool      = builtin_type_traits<T>::is_bool
  };
};

#include <design/impl/typetraits.inl>

}

using type_manip::type_traits;

}

#endif
