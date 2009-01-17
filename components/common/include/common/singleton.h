#ifndef COMMONLIB_SINGLEON_HEADER
#define COMMONLIB_SINGLEON_HEADER

#include <new>
#include <typeinfo>
#include <stl/stdexcept>
#include <stdlib.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ������ ��� ��������� ��� ������ new/delete
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct SingletonCreateUsingNew
{
  static T*   Create  ();
  static void Destroy (T*);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ������ ��� ��������� ��� ������ malloc/free
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct SingletonCreateUsingMalloc
{
  static T*   Create  ();
  static void Destroy (T*);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� � ����������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct SingletonStatic
{
  static T*   Create  ();
  static void Destroy (T*);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SingletonListNode
{
  template <class T,template <class> class CreateaionPolicy> friend class Singleton;
  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterSingleton   (const std::type_info&, void (*destroy_function)());
    void UnregisterSingleton ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void DestroyAll ();

  private:
    SingletonListNode*        prev;
    SingletonListNode*        next;
    void                      (*destroy_function)();
    const std::type_info*     type;
    static SingletonListNode* first;
    static bool               atexit_registered;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������� ������� - ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T,template <class> class CreateaionPolicy=SingletonStatic>
class Singleton
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static T& Instance ();
    static T* InstancePtr () { return &Instance (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������������� � ����������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void Init    ();
    static void Destroy ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static bool IsInitialized ();

  private:
    Singleton ();

  private:
    static T*                instance;
    static SingletonListNode node;
    static bool              is_in_init;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T,template <class> class CreateaionPolicy>
T* Singleton<T,CreateaionPolicy>::instance = 0;

template <class T,template <class> class CreateaionPolicy>
SingletonListNode Singleton<T,CreateaionPolicy>::node;

template <class T,template <class> class CreateaionPolicy>
bool Singleton<T,CreateaionPolicy>::is_in_init = false;

#include <common/detail/singleton.inl>

}

#endif
