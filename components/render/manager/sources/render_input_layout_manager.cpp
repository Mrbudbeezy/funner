#include "shared.h"

using namespace render;

/*
    �������� ���������� ��������� �������� ���������
*/

typedef stl::hash_map<size_t, LowLevelInputLayoutPtr> InputLayoutMap;

struct InputLayoutManager::Impl
{
  LowLevelDevicePtr device;  //���������� ���������
  InputLayoutMap    layouts; //�������������� �������
  
  Impl (const LowLevelDevicePtr& in_device) : device (in_device) {}
};

/*
    ������������ / ���������� / ������������
*/

InputLayoutManager::InputLayoutManager (const LowLevelDevicePtr& in_device)
  : impl (new Impl (in_device))
{
}

InputLayoutManager::~InputLayoutManager ()
{
}

/*
    �������� �������
*/

LowLevelInputLayoutPtr InputLayoutManager::CreateInputLayout (const render::low_level::InputLayoutDesc& desc)
{
  try
  {
    size_t hash = common::crc32 (&desc, sizeof (desc));
    
    InputLayoutMap::iterator iter = impl->layouts.find (hash);
    
    if (iter != impl->layouts.end ())
      return iter->second;
      
    LowLevelInputLayoutPtr layout (impl->device->CreateInputLayout (desc), false);
    
    impl->layouts [hash] = layout;
    
    return layout;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::InputLayoutManager::CreateInputLayout");
    throw;
  }
}
