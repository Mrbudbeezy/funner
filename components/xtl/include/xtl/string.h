#ifndef XTL_STRING_HEADER
#define XTL_STRING_HEADER

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

namespace xtl
{

/*
    �ࠢ����� ��ப
*/

int xstrcmp   (const char* s1, const char* s2);
int xstricmp  (const char* s1, const char* s2);
int xstrncmp  (const char* s1, const char* s2, size_t max_count);
int xstrnicmp (const char* s1, const char* s2, size_t max_count);

/*
    ����� � ��ப�:
      - �᫨ ࠧ��� ���� �������祭 ��� ���� ��ப� �����頥��� -1;
      - count - ���ᨬ��쭮� ������⢮ �����뢠���� ᨬ����� ������ '\0';
      - �᫨ buffer == NULL && count == 0, � �����頥��� ࠧ��� �ଠ�஢����.
*/

int xsnprintf   (char* buffer, size_t count, const char* format, ...);
int xvsnprintf  (char* buffer, size_t count, const char* format, va_list list);

#include <xtl/detail/string.inl>

}

#endif
