#include <stl/auto_ptr.h>

#include <xtl/common_exceptions.h>
#include <xtl/reference_counter.h>

#include <common/lockable.h>

using namespace common;

/*
    �������� ���������� ������������ �������
*/

typedef stl::auto_ptr<ILockable> LockablePtr;

struct Lockable::Impl
{
  LockablePtr lockable; //����������� ������������ �������

///�����������
  Impl ()
  {
    lockable = create_lockable ();

    if (!lockable)
      throw xtl::format_operation_exception ("", "Lockable not created");
  }

///����������
  void Lock ()
  {
    lockable->Lock ();
  }
  
///������ ����������
  void Unlock ()
  {
    try
    {
      lockable->Unlock ();
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
