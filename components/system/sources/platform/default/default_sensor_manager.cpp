#include "shared.h"

using namespace syslib;

/*
    ��������� ����������: ������ � ��������� ����������
*/

namespace
{

void raise (const char* method_name)
{
  throw xtl::format_not_supported_exception (method_name, "No sensors support for this platform");
}

}

/*
    ���������� ��������
*/

size_t DefaultSensorManager::GetSensorsCount ()
{
  return 0;
}

/*
    �������� / �������� �������
*/

sensor_t DefaultSensorManager::CreateSensor (size_t sensor_index)
{
  raise ("syslib::DefaultSensorManager::CreateSensor");
  return 0;
}

void DefaultSensorManager::DestroySensor (sensor_t)
{
  raise ("syslib::DefaultSensorManager::DestroySensor");
}

/*
    ��� �������
*/

stl::string DefaultSensorManager::GetSensorName (sensor_t)
{
  raise ("syslib::DefaultSensorManager::GetSensorName");
  return "";
}

/*
    ������������� �������
*/

stl::string DefaultSensorManager::GetSensorVendor (sensor_t)
{
  raise ("syslib::DefaultSensorManager::GetSensorVendor");
  return "";
}

/*
    ��� ����������
*/

stl::string DefaultSensorManager::GetSensorType (sensor_t)
{
  raise ("syslib::DefaultSensorManager::GetSensorType");
  return "";
}

/*
    ������������ ��������
*/

float DefaultSensorManager::GetSensorMaxRange (sensor_t)
{
  raise ("syslib::DefaultSensorManager::GetSensorMaxRange");
  return 0;
}

/*
    ������� ����������
*/

void DefaultSensorManager::SetSensorUpdateRate (sensor_t, float rate)
{
  raise ("syslib::DefaultSensorManager::SetSensorUpdateRate");
}

float DefaultSensorManager::GetSensorUpdateRate (sensor_t)
{
  raise ("syslib::DefaultSensorManager::GetSensorUpdateRate");
  return 0;
}

/*
    ��������� ���������-���������� ����������� ������
*/

const void* DefaultSensorManager::GetNativeSensorHandle (sensor_t)
{
  raise ("syslib::DefaultSensorManager::GetNativeSensorHandle");
  return 0;
}

/*
    ��������� ���������-��������� ������� ������
*/

void DefaultSensorManager::GetSensorProperties (sensor_t, common::PropertyMap& properties)
{
  raise ("syslib::DefaultSensorManager::GetSensorProperties");
}

/*
    ������ ������� �������
*/

void DefaultSensorManager::StartSensorPolling (sensor_t)
{
  raise ("syslib::DefaultSensorManager::StartSensorPolling");
}

void DefaultSensorManager::StopSensorPolling (sensor_t)
{
  raise ("syslib::DefaultSensorManager::StopSensorPolling");
}

bool DefaultSensorManager::PollSensorEvent (sensor_t, SensorEvent&)
{
  return false;
}
