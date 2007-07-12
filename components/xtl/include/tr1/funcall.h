#ifndef MYTR1_FUNCALL_HEADER
#define MYTR1_FUNCALL_HEADER

#include <tr1/functional_traits.h>
#include <mpl/utility.h>

namespace tr1
{

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

#include <tr1/detail/funcall.inl>

}

#endif
