#ifndef COMMONLIB_LOCKABLE_HEADER
#define COMMONLIB_LOCKABLE_HEADER

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///  - ������������ ������ ��� �������� ������� �� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Lockable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Lockable  ();
    ~Lockable ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� / ����� � ����������� ������ ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Lock   () const;
    void Lock   ();
    void Unlock () const;
    void Unlock ();

  private:
    Lockable (const Lockable&); //no impl
    Lockable& operator = (const Lockable&); //no impl

  private:
    typedef void* handle_t;

  private:  
    handle_t handle;    
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � �������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Lock
{
  public:
    Lock  (const Lockable& in_lockable);
    ~Lock ();

  private:
    Lock (const Lock&); //no impl
    Lock& operator = (const Lock&); //no impl
    
  private:
    const Lockable& lockable;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��� �������������� � XTL
///////////////////////////////////////////////////////////////////////////////////////////////////
void lock   (Lockable&);
void unlock (Lockable&);

#include <common/detail/lockable.inl>

}

#endif
