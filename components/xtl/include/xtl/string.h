#ifndef XTL_STRING_HEADER
#define XTL_STRING_HEADER

#include <string.h>
#include <stdio.h>
#include <wchar.h>

namespace xtl
{

/*
    �ࠢ����� ��ப
*/

int xtl_strcmp   (const char* s1, const char* s2);
int xtl_stricmp  (const char* s1, const char* s2);
int xtl_strncmp  (const char* s1, const char* s2, size_t max_count);
int xtl_strnicmp (const char* s1, const char* s2, size_t max_count);

/*
    ����� � ��ப�:
      - �᫨ ࠧ��� ���� �������祭 ��� ���� ��ப� �����頥��� -1;
      - count - ���ᨬ��쭮� ������⢮ �����뢠���� ᨬ����� ������ '\0';
      - �᫨ buffer == NULL && count == 0, � �����頥��� ࠧ��� �ଠ�஢����.
*/

int xtl_snprintf   (char* buffer, size_t count, const char* format, ...);
int xtl_vsnprintf  (char* buffer, size_t count, const char* format, va_list list);
int xtl_snwprintf  (wchar_t* buffer, size_t count, const wchar_t* format, ...);
int xtl_vsnwprintf (wchar_t* buffer, size_t count, const wchar_t* format, va_list list);

#include <xtl/detail/string.inl>

}

#endif
