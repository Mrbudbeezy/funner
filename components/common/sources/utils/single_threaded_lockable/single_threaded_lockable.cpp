#include <xtl/common_exceptions.h>

#include <common/lockable.h>

using namespace common;

namespace
{

/*
    ���������� ������������ ������� ��� ������������ ������
*/

class SingleThreadedLockable: public ILockable
{
  public:
///�����������
    SingleThreadedLockable () : locked (false) {}    

///����������
    void Lock ()
    {
      if (locked)
        throw xtl::format_operation_exception ("common::SingleThreadedLockable::Lock", "Recursive lock not supported");
        
      locked = true;
    }
    
///������ ����������
    void Unlock ()
    {
      locked = false;
    }
    
  private:
    bool locked; //���� ���������� �������
};

}

namespace common
{

//�������� ������������ �������
ILockable* create_lockable ()
{
  return new SingleThreadedLockable;
}

}
