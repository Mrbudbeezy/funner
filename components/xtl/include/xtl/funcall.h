#ifndef XTL_FUNCALL_HEADER
#define XTL_FUNCALL_HEADER

#include <xtl/functional_traits.h>
#include <xtl/utility>

namespace xtl
{

//implementation forwards
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
class tuple;

template <size_t Index, class Tuple>
struct tuple_element;

/*
    ��������������� ������� �������������� ��������
*/

//���������� ��� ��������������� ������� ��� ����������
template <class Ret, class Fn>
typename Ret funcall (Fn&);

//���������� ��� ��������������� ������� � 1-� ����������
template <class Ret, class Fn, class T1>
typename Ret funcall (Fn&, T1&);

//���������� ��� ��������������� ������� � 2-�� �����������
template <class Ret, class Fn, class T1, class T2>
typename Ret funcall (Fn&, T1&, T2&);

//���������� ��� ��������������� ������� � 3-�� �����������
template <class Ret, class Fn, class T1, class T2, class T3>
typename Ret funcall (Fn&, T1&, T2&, T3&);

//���������� ��� ��������������� ������� � 4-�� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4>
typename Ret funcall (Fn&, T1&, T2&, T3&, T4&);

//���������� ��� ��������������� ������� � 5-� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5>
typename Ret funcall (Fn&, T1&, T2&, T3&, T4&, T5&);

//���������� ��� ��������������� ������� � 6-� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6>
typename Ret funcall (Fn&, T1&, T2&, T3&, T4&, T5&, T6&);

//���������� ��� ��������������� ������� � 7-� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
typename Ret funcall (Fn&, T1&, T2&, T3&, T4&, T5&, T6&, T7&);

//���������� ��� ��������������� ������� � 8-� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
typename Ret funcall (Fn&, T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&);

//���������� ��� ��������������� ������� � 9-� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
typename Ret funcall (Fn&, T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&, T9&);

/*
    ����� �������� � ����������� ��������� ������ ��������
*/

//��������� ����������� ��� ������ �������� evalutor.eval<ArgumentIndex> (args)
template <class Ret, size_t ArgumentsCount, class Fn, class Args, class ArgumentEvalutor>
Ret apply (Fn& fn, Args& args, ArgumentEvalutor& evalutor);

//��������� ������������ ��������, ����������� �������� ��������� get<Index> (args)
template <class Ret, class Fn, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
Ret apply (Fn& fn, tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>& args);

template <class Ret, class Fn, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
Ret apply (Fn& fn, const tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>& args);

#include <xtl/detail/unwrap.inl>
#include <xtl/detail/funcall.inl>
#include <xtl/detail/apply.inl>

}

namespace tr1
{

using xtl::tuple;
using xtl::tuple_element;

}

#endif
