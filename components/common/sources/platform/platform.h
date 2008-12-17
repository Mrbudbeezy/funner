#ifndef COMMONLIB_PLATFORM_HEADER
#define COMMONLIB_PLATFORM_HEADER

#include <cstddef>

#include <common/thread.h>

namespace common
{

//implementation forwards
class ICustomAllocator;
class ICustomFileSystem;

/*
    �����ମ-������ᨬ� ����䥩��
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///����䥩� ���⭮� �裡 ���
///////////////////////////////////////////////////////////////////////////////////////////////////
class IThreadCallback
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �� �믮������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Run () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��뫮�
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AddRef  () = 0;
    virtual void Release () = 0;

  protected:
    virtual ~IThreadCallback () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����䥩� �ࠢ����� ���ﬨ
///////////////////////////////////////////////////////////////////////////////////////////////////
class ICustomThreadSystem
{
  public:
    typedef void* thread_t;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� / 㤠����� ���
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual thread_t CreateThread (IThreadCallback*, ThreadState) = 0;
    virtual void     DeleteThread (thread_t) = 0;    
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��ࠢ����� ���ﭨ�� ࠡ��� ���
///////////////////////////////////////////////////////////////////////////////////////////////////    
    virtual void        SetThreadState (thread_t, ThreadState) = 0;    
    virtual ThreadState GetThreadState (thread_t) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����襭�� ���
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void JoinThread (thread_t) = 0;

  protected:
    virtual ~ICustomThreadSystem () {}
};

/*
    �����ମ-����ᨬ� ����䥩��
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����ଠ �� 㬮�砭��
///////////////////////////////////////////////////////////////////////////////////////////////////
class DefaultPlatform
{
  public:
    static ICustomAllocator*    GetSystemAllocator ();
    static ICustomFileSystem*   GetFileSystem ();
    static ICustomFileSystem*   GetIOSystem ();
    static ICustomThreadSystem* GetThreadSystem ();
    static size_t               GetMilliseconds ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����ଠ Win32
///////////////////////////////////////////////////////////////////////////////////////////////////
class Win32Platform: public DefaultPlatform
{
  public:
    static ICustomThreadSystem* GetThreadSystem ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Unistd
///////////////////////////////////////////////////////////////////////////////////////////////////
class UnistdPlatform: public DefaultPlatform
{
  public:
    static ICustomFileSystem*   GetFileSystem   ();
    static ICustomThreadSystem* GetThreadSystem ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Darwin
///////////////////////////////////////////////////////////////////////////////////////////////////
class MacOsXPlatform : public UnistdPlatform
{
  public:
    static size_t GetMilliseconds ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ⥪�饩 �������
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
  typedef Win32Platform Platform;
#elif defined (__APPLE__)
  typedef MacOsXPlatform Platform;
#elif defined __GNUC__
  typedef UnistdPlatform Platform;
#else
  #error Unknown compiler
#endif

}

#endif
