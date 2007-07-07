/*
    ������� ��� ��������������������
*/

#ifndef XTL_MPL_UTILITY_HEADER
#define XTL_MPL_UTILITY_HEADER

#include <stddef.h>

namespace xtl
{

namespace mpl
{

/*
    ����������� �������
*/

template <bool condition, class TrueType, class FalseType> struct select;              //����� �� ���� ����� �� ������� condition
template <bool condition>                                  struct compile_time_assert; //������ assert �� ����� ����������

#include <stl/detail/mpl/utility.inl>

}

}

#endif
