#ifndef XTL_STRING_HEADER
#define XTL_STRING_HEADER

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cctype>

namespace xtl
{

/*
    ��������� �����
*/

int xstrcmp   (const char* s1, const char* s2);
int xstricmp  (const char* s1, const char* s2);
int xstrncmp  (const char* s1, const char* s2, size_t max_count);
int xstrnicmp (const char* s1, const char* s2, size_t max_count);

/*
    ��������� ����� ������
*/

size_t xstrlen (const char* s);
size_t xstrlen (const wchar_t* s);

/*
    ����������� �����
*/

char*    xstrncpy (char* dst, const char* src, int length);
wchar_t* xstrncpy (wchar_t* dst, const wchar_t* src, int length);

/*
    ������ � ������:
      - ���� ������ ������ ������������ ��� ������ ������ ������������ -1;
      - count - ������������ ���������� ������������ �������� ������� '\0';
      - ���� buffer == NULL && count == 0, �� ������������ ������ ��������������.
*/

int xsnprintf  (char* buffer, size_t count, const char* format, ...);
int xvsnprintf (char* buffer, size_t count, const char* format, va_list list);

#include <xtl/detail/string.inl>

}

#endif
