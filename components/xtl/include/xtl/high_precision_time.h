#ifndef XTL_HIGH_PRECISION_TIME_HEADER
#define XTL_HIGH_PRECISION_TIME_HEADER

#include <cstddef>

#ifndef _WIN32
  #include <time.h>
#endif

namespace xtl
{

typedef unsigned long long high_precision_time_t;

//��������� ������� � ������� ���������
high_precision_time_t high_precision_time ();

//��������� ������� �������
high_precision_time_t high_precision_time_frequency ();

//�������������� � ������������ �������������
void convert_high_precision_time (high_precision_time_t span, size_t& out_seconds, size_t& out_nanoseconds);

#include <xtl/detail/high_precision_time.inl>

}

#endif
