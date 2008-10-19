#ifndef COMMLIB_DEFAULT_PLATFORM_HEADER
#define COMMLIB_DEFAULT_PLATFORM_HEADER

#include <stdio.h>
#include <ctime>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>

#ifdef _WIN32
  #include <io.h>
  #include <direct.h>

  #ifdef _MSC_VER
    #pragma warning (disable : 4996) //declare deprecated
  #endif

#elif __GNUC__
  #include <unistd.h>
#else
  #error Unknown compiler
#endif

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/string.h>

#include <common/heap.h>
#include <common/file.h>
#include <common/strlib.h>
#include <common/singleton.h>

#include <platform/platform.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class MallocAllocator: public ICustomAllocator
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� / ������������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void* Allocate   (size_t size);
    void  Deallocate (void* ptr);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  () {}
    void Release () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class StdioFileSystem: public ICustomFileSystem
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    file_t     FileOpen     (const char* name,filemode_t mode_flags,size_t buffer_size);
    void       FileClose    (file_t);
    size_t     FileRead     (file_t,void* buf,size_t size);
    size_t     FileWrite    (file_t,const void* buf,size_t size);
    void       FileRewind   (file_t);
    filepos_t  FileSeek     (file_t,filepos_t pos);
    filepos_t  FileTell     (file_t);
    filesize_t FileSize     (file_t);
    void       FileResize   (file_t,filesize_t new_size);
    bool       FileEof      (file_t);
    void       FileFlush    (file_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Remove (const char* file_name);
    void Rename (const char* file_name,const char* new_name);
    void Mkdir  (const char* dir_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� � �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsFileExist (const char* file_name);
    bool GetFileInfo (const char* file_name,FileInfo& info);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Search (const char* wc_mask,const FileSearchHandler& handler);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  () {}
    void Release () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� / ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class StdioIOSystem: public StdioFileSystem
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    file_t FileOpen       (const char* name,filemode_t mode_flags,size_t buffer_size);
    size_t FileBufferSize (file_t) { return (size_t)-1; } //����������� �� �����

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Remove (const char* file_name);
    void Rename (const char* file_name,const char* new_name);
    void Mkdir  (const char* dir_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� � �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsFileExist (const char* file_name);
    bool GetFileInfo (const char* file_name,FileInfo& info);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Search (const char* wc_mask,const FileSearchHandler& handler);
};

}

#endif
