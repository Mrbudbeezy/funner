#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    �����������
*/

DeviceObject::DeviceObject (Device* in_device)
  : device (in_device)
  { }

/*
    ��������� �� ���������� ���������, �������� ����������� ������
*/

IDevice* DeviceObject::GetDevice ()
{
  common::RaiseNotImplemented ("render::low_level::opengl::DeviceObject::GetDevice");
  return 0;
}

/*
    �������� ������������� �������
*/

bool DeviceObject::IsCompatible (IObject* object)
{
  return IsCompatible (dynamic_cast<DeviceObject*> (object));
}

bool DeviceObject::IsCompatible (DeviceObject* object)
{
  return object && device == object->device;
}
