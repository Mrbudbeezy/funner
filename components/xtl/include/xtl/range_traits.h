#ifndef XTL_RANGE_TRAITS_HEADER
#define XTL_RANGE_TRAITS_HEADER

#include <stl/reverse_iterator.h>
#include <xtl/utility>

namespace xtl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Метафункции для однопроходных интервалов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct range_value;          //тип значения элементов интервала
template <class T> struct range_iterator;       //тип итератора интервала
template <class T> struct range_const_iterator; //тип константного итератора интервала

///////////////////////////////////////////////////////////////////////////////////////////////////
///Метафункции для интервалов прямого перебора
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct range_difference; //тип разности между двумя итераторами интервала
template <class T> struct range_size;       //тип размерности интервала

///////////////////////////////////////////////////////////////////////////////////////////////////
///Метафункции для интервалов двунаправленного перебора
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct range_reverse_iterator;       //определение типа итератора обратного перебора
template <class T> struct range_const_reverse_iterator; //определение типа константного итератора обратного перебора

///////////////////////////////////////////////////////////////////////////////////////////////////
///Дополнительные метафункции
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct range_result_iterator;
template <class T> struct range_reverse_result_iterator;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Функции для однопроходных интервалов перебора
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> typename range_iterator<T>::type       begin (T&);       //получение итератора на начало интервала
template <class T> typename range_const_iterator<T>::type begin (const T&); //получение константного итератора на начало интервала
template <class T> typename range_iterator<T>::type       end   (T&);       //получение итератора на конец интервала
template <class T> typename range_const_iterator<T>::type end   (const T&); //получение константного итератора на конец интервала
template <class T> bool                                   empty (const T&); //проверка на пустоту интервала

///////////////////////////////////////////////////////////////////////////////////////////////////
///Функции для интервалов прямого перебора
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> typename range_size<T>::type size (const T&); //определение количества элементов в интервале

///////////////////////////////////////////////////////////////////////////////////////////////////
///Функции для интервалов двунаправленного перебора
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> typename range_reverse_result_iterator<T>::type rbegin (T&);       //получение обратного итератора на начало интервала
template <class T> typename range_reverse_result_iterator<T>::type rend   (T&);       //получение обратного итератора на конец интервала           

///////////////////////////////////////////////////////////////////////////////////////////////////
///Дополнительные функции для интервалов
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
