/*
    ������� ��� ��������������������
*/

#ifndef MPL_UTILITY_HEADER
#define MPL_UTILITY_HEADER

#include <stddef.h>

namespace mpl
{

/*
    ����-���������
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ����������� ������ ����� � compile-time �� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef char                       no_type;
typedef struct { char dummy [2]; } yes_type;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� ��������� ����� ����������� ������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, T v> struct integral_constant
{
  static  const T                 value = v;
  typedef T                       value_type;
  typedef integral_constant<T, v> type; 
};

//��������������� ������ ��� �������� ��������������� ������������� �������
template <bool value> struct bool_constant:    public integral_constant<bool, value> {};
template <int value>  struct integer_constant: public integral_constant<int, value> {};

typedef bool_constant<true>  true_type;
typedef bool_constant<false> false_type;

/*
    ����������� �������
*/

template <bool condition, class TrueType, class FalseType> struct select;              //����� �� ���� ����� �� ������� condition
template <bool condition>                                  struct compile_time_assert; //������ assert �� ����� ����������

#include <mpl/detail/utility.inl>

}

#endif
