#ifndef SYSLIB_APPLICATION_DELEGATE_HEADER
#define SYSLIB_APPLICATION_DELEGATE_HEADER

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IApplicationListener
{
  public:
///������� ��������� �����
    virtual void OnIdle () {}    
    
///���������� �������� ������ �����������
    virtual void OnExit (int code) {}

  protected:
    virtual ~IApplicationListener () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IApplicationDelegate
{
  public:
///������ ����� ��������� ���������
    virtual void Run () = 0;

///����� �� ����������
    virtual void Exit (int code) = 0;
    
///��������� ������������� ������ ������� idle
    virtual void SetIdleState (bool state) = 0;

///��������� ��������� ������� ����������
    virtual void SetListener (IApplicationListener* listener) = 0;
    
///������� ������
    virtual void AddRef  () = 0;
    virtual void Release () = 0;

  protected:
    virtual ~IApplicationDelegate () {}
};

}

#endif
