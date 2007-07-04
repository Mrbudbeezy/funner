#ifndef __COMMONLIB_STRING_WRAPPERS__
#define __COMMONLIB_STRING_WRAPPERS__

#include <stddef.h>
#include <stdarg.h>

/*
    �ਢ������ ���� �㭪樨 ����� �����窠�� ��� �� �������㥬묨 ���������
*/

namespace common
{

/*
    �ࠢ����� ��ப
*/

int strcmp   (const char* s1,const char* s2);
int stricmp  (const char* s1,const char* s2);
int strncmp  (const char* s1,const char* s2,size_t max_count);
int strnicmp (const char* s1,const char* s2,size_t max_count);

/*
    ����� � ��ப�:
      - �᫨ ࠧ��� ���� �������祭 ��� ���� ��ப� �����頥��� -1;
      - count - ���ᨬ��쭮� ������⢮ �����뢠���� ᨬ����� ������ '\0';
      - �᫨ buffer == NULL && count == 0, � �����頥��� ࠧ��� �ଠ�஢����.
*/

int snprintf   (char* buffer,size_t count,const char* format,...);
int vsnprintf  (char* buffer,size_t count,const char* format,va_list list);
int snwprintf  (wchar_t* buffer,size_t count,const wchar_t* format,...);
int vsnwprintf (wchar_t* buffer,size_t count,const wchar_t* format,va_list list);

/*
    ���஢����
*/

size_t strhash  (const char*,size_t init_hash=0xFFFFFFFF);
size_t strihash (const char*,size_t init_hash=0xFFFFFFFF);

}

#endif
