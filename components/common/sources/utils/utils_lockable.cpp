#include <xtl/common_exceptions.h>

#include <common/lockable.h>

#include <platform/platform.h>

using namespace common;

/*
    �������� ���������� ������������ �������
*/

struct Lockable::Impl
{
  Platform::lockable_t lockable; //����������� ������������ �������

///�����������
  Impl ()
  {
    Platform::InitLockable (lockable);
  }
  
///����������
  ~Impl ()
  {
    try
    {
      Platform::DestroyLockable (lockable);
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }

///����������
  void Lock ()
  {
    Platform::Lock (lockable);
  }
  
///������ ����������
  void Unlock ()
  {
    try
    {
      Platform::Unlock (lockable);
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }
};

/*
    ����������� / ����������
*/

Lockable::Lockable ()
{
  try
  {
    impl = new Impl;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Lockable::Lockable");
    throw;
  }
}

Lockable::~Lockable ()
{
  delete impl;
}

/*
    ����������
*/

void Lockable::Lock ()
{
  try
  {
    impl->Lock ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Lockable::Lock");
    throw;
  }
}

void Lockable::Unlock ()
{
  impl->Unlock ();
}
