#include "shared.h"

using namespace common;
using namespace syslib;
using namespace input::low_level;
using namespace input::low_level::sensor_input_driver;

namespace
{

const char* COMPONENT_NAME = "input.low_level.SensorInputDriver"; //��� ����������
const char* DRIVER_NAME    = "SensorInputDriver";                 //��� ��������

void default_log_handler (const char*)
{
}

}

/*
    ����������� / ����������
*/

Driver::Driver ()
  : debug_log (&default_log_handler)
{
}

Driver::~Driver ()
{
}

/*
    �������� ��������
*/

const char* Driver::GetDescription ()
{
  return "input::low_level::sensor_input_driver";
}

/*
    ������������ ��������� ��������� �����
*/

size_t Driver::GetDevicesCount ()
{
  return SensorManager::SensorsCount ();
}

const char* Driver::GetDeviceName (size_t index)
{
  return Driver::GetDeviceFullName (index);
}

const char* Driver::GetDeviceFullName (size_t index)
{
  try
  {
    Sensor sensor (index);
    
    return sensor.Name ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::low_level::sensor_input_driver::Driver::GetDeviceFullName");
    throw;
  }
}

/*
    ��������� ���������� �����
*/

IDevice* Driver::CreateDevice (const char* full_name, const char* init_string)
{
  throw xtl::make_not_implemented_exception ("input::low_level::sensor_input_driver::Driver::CreateDevice");
}

/*
    ���������� ����������������
*/

void Driver::SetDebugLog (const LogHandler& log)
{
  debug_log = log;
}

const Driver::LogHandler& Driver::GetDebugLog ()
{
  return debug_log;
}

/*
    ������� ������
*/

void Driver::AddRef ()
{
  addref (this);
}

void Driver::Release ()
{
  release (this);
}

/*
    ��������� ��������
*/

namespace components
{

namespace sensor_input_driver
{

class SensorInputDriverComponent
{
  public:
    SensorInputDriverComponent ()
    {
      input::low_level::DriverManager::RegisterDriver (DRIVER_NAME, xtl::com_ptr<Driver> (new Driver, false).get ());
    }
};

extern "C" common::ComponentRegistrator<SensorInputDriverComponent> SensorInputDriver (COMPONENT_NAME);

}

}
