#ifndef XTL_RANGE_TRAITS_HEADER
#define XTL_RANGE_TRAITS_HEADER

#include <stl/reverse_iterator.h>
#include <xtl/utility>

namespace xtl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ��� ������������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct range_value;          //��� �������� ��������� ���������
template <class T> struct range_iterator;       //��� ��������� ���������
template <class T> struct range_const_iterator; //��� ������������ ��������� ���������

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ��� ���������� ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct range_difference; //��� �������� ����� ����� ����������� ���������
template <class T> struct range_size;       //��� ����������� ���������

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ��� ���������� ���������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct range_reverse_iterator;       //����������� ���� ��������� ��������� ��������
template <class T> struct range_const_reverse_iterator; //����������� ���� ������������ ��������� ��������� ��������

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct range_result_iterator;
template <class T> struct range_reverse_result_iterator;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��� ������������� ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> typename range_iterator<T>::type       begin (T&);       //��������� ��������� �� ������ ���������
template <class T> typename range_const_iterator<T>::type begin (const T&); //��������� ������������ ��������� �� ������ ���������
template <class T> typename range_iterator<T>::type       end   (T&);       //��������� ��������� �� ����� ���������
template <class T> typename range_const_iterator<T>::type end   (const T&); //��������� ������������ ��������� �� ����� ���������
template <class T> bool                                   empty (const T&); //�������� �� ������� ���������

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��� ���������� ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> typename range_size<T>::type size (const T&); //����������� ���������� ��������� � ���������

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��� ���������� ���������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> typename range_reverse_result_iterator<T>::type rbegin (T&);       //��������� ��������� ��������� �� ������ ���������
template <class T> typename range_reverse_result_iterator<T>::type rend   (T&);       //��������� ��������� ��������� �� ����� ���������           

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ������� ��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> typename range_const_iterator<T>::type         const_begin  (const T&);
template <class T> typename range_const_iterator<T>::type         const_end    (const T&);
template <class T> typename range_const_reverse_iterator<T>::type const_rbegin (const T&);
template <class T> typename range_const_reverse_iterator<T>::type const_rend   (const T&);

namespace type_traits
{

#include <xtl/detail/type_traits/cv.inl>

}

#include <xtl/detail/range_traits.inl>

}

#endif
