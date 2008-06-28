#include "shared.h"

using namespace common;

/*
    �����������
*/

MountPoint::MountPoint (const char* in_name, ICustomVarRegistry* in_registry)
  : name (in_name), registry (in_registry)
{
  registry->SetEventHandler (xtl::bind (&MountPoint::Notify, this, _1, _2));
}

/*
    �������� �� ������� ������� / ���������� � ������������� �������
*/

xtl::connection MountPoint::RegisterEventHandler (const EventHandler& handler)
{
  return signal.connect (handler);
}

void MountPoint::Notify (const char* var_name, VarRegistryEvent event)
{
  try
  {
    signal (name.c_str (), var_name, event);
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}
