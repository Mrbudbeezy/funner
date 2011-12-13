#include "shared.h"

using namespace syslib::iphone;
using namespace input::low_level::iphone_driver;

namespace
{

const size_t MESSAGE_BUFFER_SIZE = 64;

}

struct IPhoneApplication::Impl : public IApplicationListener
{
  typedef xtl::signal<void (const char*)> DeviceSignal;

  stl::string          name;                          //��� ����������
  stl::string          full_name;                     //������ ��� ����������
  DeviceSignal         signals;                       //����������� �������
  stl::wstring         control_name;                  //��� ��������
  char                 message [MESSAGE_BUFFER_SIZE]; //���������

  ///����������� / ����������
  Impl (const char* in_name, const char* in_full_name)
    : name (in_name)
    , full_name (in_full_name)
  {
    ApplicationManager::AttachApplicationListener (this);
  }

  ~Impl ()
  {
    ApplicationManager::DetachApplicationListener (this);
  }

  ///������� ����������
  void OnMemoryWarning ()
  {
    signals ("MemoryWarning");
  }

  void OnActive ()
  {
    signals ("Application active");
  }

  void OnInactive ()
  {
    signals ("Application inactive");
  }
};

/*
   �����������/����������
*/

IPhoneApplication::IPhoneApplication (const char* name, const char* full_name)
  : impl (new Impl (name, full_name))
{
}

IPhoneApplication::~IPhoneApplication ()
{
  delete impl;
}

/*
   ��������� ����� ��������
*/

const wchar_t* IPhoneApplication::GetControlName (const char* control_id)
{
  impl->control_name = common::towstring (control_id);

  return impl->control_name.c_str ();
}

/*
   �������� �� ������� ����������
*/

xtl::connection IPhoneApplication::RegisterEventHandler (const input::low_level::IDevice::EventHandler& handler)
{
  return impl->signals.connect (handler);
}

/*
   ��������� ����������
*/

const char* IPhoneApplication::GetProperties ()
{
  return "";
}

void IPhoneApplication::SetProperty (const char* name, float value)
{
  throw xtl::make_argument_exception ("input::low_level::iphone_driver::IPhoneApplication::SetProperty", "name", name);
}

float IPhoneApplication::GetProperty (const char* name)
{
  throw xtl::make_argument_exception ("input::low_level::iphone_driver::IPhoneApplication::GetProperty", "name", name);
}

/*
   ��������� ����� ����������
*/

const char* IPhoneApplication::GetName ()
{
  return impl->name.c_str ();
}

/*
   ������ ��� ���������� (���.���.�������������)
*/

const char* IPhoneApplication::GetFullName ()
{
  return impl->full_name.c_str ();
}
