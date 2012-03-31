#include "shared.h"

using namespace input::low_level::android_driver;

/*
    ���������
*/

namespace
{

const char* LOG_NAME = "input.low_level.android";

}

/*
    �������� ���������� �������
*/

typedef xtl::signal<void (const char*)> DeviceSignal;

struct Sensor::Impl
{
  ASensorRef   handle;       //���������� �������
  stl::string  name;         //����������� ��� ����������
  stl::string  full_name;    //������ ���
  DeviceSignal signal;       //������
  common::Log  log;          //�������� ������ ���������� ���������
  stl::wstring control_name; //��� ��������
  
///�����������
  Impl (ASensorRef in_handle, const char* in_name)
    : handle (in_handle)
    , name (in_name)
    , log (LOG_NAME)
  {
    const char* in_full_name = ASensor_getName (handle);

    if (!in_full_name)
      throw xtl::format_operation_exception ("", "::ASensor_getName failed");

    full_name = in_full_name;
  }
  
///��������� �������
  void ProcessEvents ()
  {
    try
    {
      
    }
    catch (std::exception& e)
    {
      log.Printf ("%s\n    at input::low_level::android_driver::Sensor::Impl::ProcessEvents", e.what ());
    }
    catch (...)
    {
      log.Printf ("unknown exception\n    at input::low_level::android_driver::Sensor::Impl::ProcessEvents");
    }
  }
};

/*
    �����������/����������
*/

Sensor::Sensor (ASensorRef handle, const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
    
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");      

    impl = new Impl (handle, name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::low_level::android_driver::Sensor::Sensor");
    throw;
  }
}

Sensor::~Sensor ()
{
}

/*
    ��������� ����� ����������
*/

const char* Sensor::GetName ()
{
  return impl->name.c_str ();
}

/*
    ������ ��� ���������� (���.���.�������������)
*/

const char* Sensor::GetFullName ()
{
  return impl->full_name.c_str ();
}

/*
    ��������� ����� ��������
*/

const wchar_t* Sensor::GetControlName (const char* control_id)
{
  impl->control_name = common::towstring (control_id);

  return impl->control_name.c_str ();
}

/*
    �������� �� ������� ����������
*/

xtl::connection Sensor::RegisterEventHandler (const input::low_level::IDevice::EventHandler& handler)
{
  return impl->signal.connect (handler);
}

/*
    ��������� ����������
*/

const char* Sensor::GetProperties ()
{
//?????????
  return "";
}

void Sensor::SetProperty (const char* name, float value)
{
//?????????
}

float Sensor::GetProperty (const char* name)
{
//?????????
  return 0.0f;
}
