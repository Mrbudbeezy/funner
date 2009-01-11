#include "shared.h"

using namespace syslib;
using namespace common;

namespace
{

/*
    ��������� ������㥬��� ��ꥪ� ��� ��������筮� ������
*/

class MultiThreadedLockable: public ILockable
{
  public:
///�����஢��
    void Lock ()
    {
      mutex.Lock ();
    }
    
///�⬥�� �����஢��
    void Unlock ()
    {
      mutex.Unlock ();
    }
    
  private:
    Mutex mutex;
};

}

namespace common
{

//ᮧ����� ������㥬��� ��ꥪ�
ILockable* create_lockable ()
{
  return new MultiThreadedLockable;
}

}
