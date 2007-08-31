#include "shared.h"

using namespace sound::low_level;

namespace
{

//������� �������� OpenAL ����������
ISoundDevice* create_device (const char*, const char* device_name, const void*, const char*)
{
  return new OpenALDevice (device_name);
}

}

namespace sound
{

namespace low_level
{

/*
    ����������� �������� ��������������� ����� �� ���� OpenAL
*/

void register_openal_driver (const char* name)
{ 
  SoundSystem::RegisterDriver (name, &create_device);

  try
  {
     //����� ����� ���������������� ������������
     
     SoundSystem::RegisterConfiguration (name, "default");
  }
  catch (...)
  {
    SoundSystem::UnregisterAllConfigurations (name, "*");
    SoundSystem::UnregisterDriver (name);
    throw;
  }
}

}

}
