#include "shared.h"

using namespace render;
using namespace render::manager;

/*
    �������� ���������� ��������� ����������
*/

struct DeviceManager::Impl
{
  LowLevelDeviceContextPtr                  context;                    //���������������� �������� ���������
  LowLevelDevicePtr                         device;                     //���������� ������������
  LowLevelDriverPtr                         driver;                     //������� ���������� ������������
  CacheManagerPtr                           cache_manager;              //�������� �����������
  render::manager::InputLayoutManager       input_layout_manager;       //�������� �������� ���������
  render::manager::ProgramParametersManager program_parameters_manager; //�������� ���������� �������� ��������  
  SettingsPtr                               settings;                   //��������� ��������� ����������
  low_level::DeviceCaps                     device_caps;                //����������� ���������� ���������
  
  Impl (const LowLevelDevicePtr& in_device, const LowLevelDriverPtr& in_driver, const SettingsPtr& in_settings, const CacheManagerPtr& in_cache_manager)
    : context (in_device->GetImmediateContext ())
    , device (in_device)
    , driver (in_driver)
    , cache_manager (in_cache_manager)
    , input_layout_manager (in_device, in_settings)
    , program_parameters_manager (in_device, in_settings, in_cache_manager)
    , settings (in_settings)
  {
    device->GetCaps (device_caps);    
  }
};

/*
    ����������� / ����������
*/

DeviceManager::DeviceManager (const LowLevelDevicePtr& device, const LowLevelDriverPtr& driver, const SettingsPtr& settings, const CacheManagerPtr& cache_manager)
{
  try
  {
    if (!device)
      throw xtl::make_null_argument_exception ("", "device");

    if (!driver)
      throw xtl::make_null_argument_exception ("", "driver");
      
    if (!settings)
      throw xtl::make_null_argument_exception ("", "settings");
      
    if (!cache_manager)
      throw xtl::make_null_argument_exception ("", "cache_manager");

    impl = new Impl (device, driver, settings, cache_manager);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::DeviceManager::DeviceManager");
    throw;
  }
}

DeviceManager::~DeviceManager ()
{
}

/*
    ��������� ���������� / ����������������� ��������� / ��������
*/

render::low_level::IDevice& DeviceManager::Device ()
{
  return *impl->device;
}

render::low_level::IDeviceContext& DeviceManager::ImmediateContext ()
{
  return *impl->context;
}

render::low_level::IDriver& DeviceManager::Driver ()
{
  return *impl->driver;
}

/*
    ����������� ���������� ���������
*/

const low_level::DeviceCaps& DeviceManager::DeviceCaps ()
{
  return impl->device_caps;
}

/*
    �������� �����������
*/

render::manager::CacheManager& DeviceManager::CacheManager ()
{
  return *impl->cache_manager;
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
