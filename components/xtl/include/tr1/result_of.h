#ifndef MYTR1_RESULT_OF_HEADER
#define MYTR1_RESULT_OF_HEADER

#include <tr1/functional_traits.h>

namespace tr1
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ���� ������������� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class FunctionCallType> struct result_of;

#include <tr1/detail/result_of.inl>

}

#endif
