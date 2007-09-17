#ifndef COMMONLIB_STREAMS_TESTS_HEADER
#define COMMONLIB_STREAMS_TESTS_HEADER

#include <common/streams.h>
#include <common/wxf_writer.h>
#include <xtl/function.h>
#include <stl/string>
#include <stl/set>
#include <mathlib.h>
#include <stdio.h>
#include <memory.h>

using namespace common;
using namespace math;

/*
    ����� ������ � ����������� ����� ������
*/

size_t dump (const void* buffer, size_t size)
{
  size_t result = size;

  const char* s = (const char*)buffer;

  for (;size--; s++)
    printf ("%c", *s);

  return result;
}

#endif
