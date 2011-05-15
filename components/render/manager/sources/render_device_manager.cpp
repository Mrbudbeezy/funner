#include "shared.h"

using namespace render;

/*
    �������� ���������� ��������� ����������
*/

struct DeviceManager::Impl
{
  LowLevelDevicePtr                device;                     //���������� ������������
  LowLevelDriverPtr                driver;                     //������� ���������� ������������
  render::InputLayoutManager       input_layout_manager;       //�������� �������� ���������
  render::ProgramParametersManager program_parameters_manager; //�������� ���������� �������� ��������
  SettingsPtr                      settings;                   //��������� ��������� ����������
  
  Impl (const LowLevelDevicePtr& in_device, const LowLevelDriverPtr& in_driver, const SettingsPtr& in_settings)
    : device (in_device)
    , driver (in_driver)
    , input_layout_manager (in_device, in_settings)
    , program_parameters_manager (in_device, in_settings)
    , settings (in_settings)
  {
  }
};

/*
    ����������� / ����������
*/

DeviceManager::DeviceManager (const LowLevelDevicePtr& device, const LowLevelDriverPtr& driver, const SettingsPtr& settings)
{
  try
  {
    if (!device)
      throw xtl::make_null_argument_exception ("", "device");

    if (!driver)
      throw xtl::make_null_argument_exception ("", "driver");
      
    if (!settings)
      throw xtl::make_null_argument_exception ("", "settings");

    impl = new Impl (device, driver, settings);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::DeviceManager::DeviceManager");
    throw;
  }
}

DeviceManager::~DeviceManager ()
{
}

/*
    ��������� ���������� / ��������
*/

render::low_level::IDevice& DeviceManager::Device ()
{
  return *impl->device;
}

render::low_level::IDriver& DeviceManager::Driver ()
{
  return *impl->driver;
}

/*
    �������� �������� ���������
*/

InputLayoutManager& DeviceManager::InputLayoutManager ()
{
  return impl->input_layout_manager;
}

/*
    �������� ���������� �������� ��������
*/

ProgramParametersManager& DeviceManager::ProgramParametersManager ()
{
  return impl->program_parameters_manager;
}

/*
    ��������� ��������� ����������
*/

Settings& DeviceManager::Settings ()
{
  return *impl->settings;
}
