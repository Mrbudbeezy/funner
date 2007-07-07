/*
    ������� ��� ��������������������
*/

#ifndef MPL_UTILITY_HEADER
#define MPL_UTILITY_HEADER

#include <stddef.h>

namespace mpl
{

/*
    ����������� �������
*/

template <bool condition, class TrueType, class FalseType> struct select;              //����� �� ���� ����� �� ������� condition
template <bool condition>                                  struct compile_time_assert; //������ assert �� ����� ����������

#include <mpl/detail/utility.inl>

}

#endif
