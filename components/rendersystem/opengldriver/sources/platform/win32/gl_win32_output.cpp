#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

const size_t MODE_ARRAY_RESERVE = 32; //������������� ���������� �����-�������

namespace
{

/*
    ��������� ���������� �� �����-������
*/

bool get_mode_desc (const char* device_name, int mode_index, OutputModeDesc& mode_desc)
{
  DEVMODE dev_mode_desc;
  
  memset (&dev_mode_desc, 0, sizeof (dev_mode_desc));
  
  dev_mode_desc.dmSize = sizeof (dev_mode_desc);
  
  if (!EnumDisplaySettings (device_name, mode_index, &dev_mode_desc))
    return false;

  mode_desc.width        = dev_mode_desc.dmPelsWidth;
  mode_desc.height       = dev_mode_desc.dmPelsHeight;
  mode_desc.color_bits   = dev_mode_desc.dmBitsPerPel;
  mode_desc.refresh_rate = dev_mode_desc.dmDisplayFrequency;
  
  return true;
}

}

/*
    ����������� / ����������
*/

Output::Output (const DISPLAY_DEVICE& device_info)
{
    //��������� ����� ����������

  name     = device_info.DeviceString;
  win_name = device_info.DeviceName;
  
    //��������� ����������� �������� � ����� ����������

  stl::string::reverse_iterator iter;

  for (iter=name.rbegin (); iter!=name.rend () && *iter == ' '; ++iter);

  name.erase (iter.base (), name.end ());
  
    //������������ ������ ������ ����������
    
  stl::string flags_string;
  
  struct Tag2Flag
  {
    const char* tag;
    size_t      flag;
  };
  
  static const Tag2Flag flags [] = {
    {"ATTACHED_TO_DESKTOP", DISPLAY_DEVICE_ATTACHED_TO_DESKTOP},
    {"MIRRORING_DRIVER",    DISPLAY_DEVICE_MIRRORING_DRIVER},
    {"MODESPRUNED",         DISPLAY_DEVICE_MODESPRUNED},
    {"PRIMARY_DEVICE",      DISPLAY_DEVICE_PRIMARY_DEVICE},
    {"REMOVABLE",           DISPLAY_DEVICE_REMOVABLE},
    {"VGA_COMPATIBLE",      DISPLAY_DEVICE_VGA_COMPATIBLE},
    {0, 0}
  };
  
  for (const Tag2Flag* i=flags; i->tag; ++i)
    if (i->flag & device_info.StateFlags)
    {
      if (!flags_string.empty ())
        flags_string += " | ";
        
      flags_string += i->tag;
    }

    //����������� �������

  properties.AddProperty ("win-name",     device_info.DeviceName);
  properties.AddProperty ("flags",        flags_string.c_str ());
  properties.AddProperty ("pci-id",       device_info.DeviceID);
  properties.AddProperty ("registry-key", device_info.DeviceKey);
  
    //���������� ������ �����-�������
    
  modes.reserve (MODE_ARRAY_RESERVE);

  OutputModeDesc mode_desc;

  for (int mode_index=0; get_mode_desc (win_name.c_str (), mode_index, mode_desc); mode_index++)
    modes.push_back (mode_desc);
}

Output::~Output ()
{
}

/*
    ��������� ����� / �������� ����������
*/

const char* Output::GetName ()
{
  return name.c_str ();
}

/*
    ��������� ������ �����-�������
*/

size_t Output::GetModesCount ()
{
  return modes.size ();
}

void Output::GetModeDesc (size_t mode_index, OutputModeDesc& mode_desc)
{
  if (mode_index >= modes.size ())
    RaiseOutOfRange ("render::low_level::opengl::Output::GetModeDesc", "mode_index", mode_index, modes.size ());
    
  mode_desc = modes [mode_index];
}

/*
    ��������� �������� �����-������
*/

void Output::SetCurrentMode (const OutputModeDesc& mode_desc)
{
  DEVMODE dev_mode_desc;

  memset (&dev_mode_desc, 0, sizeof (dev_mode_desc));

  dev_mode_desc.dmSize             = sizeof (DEVMODE);
  dev_mode_desc.dmPelsWidth        = mode_desc.width;
  dev_mode_desc.dmPelsHeight       = mode_desc.height;
  dev_mode_desc.dmBitsPerPel       = mode_desc.color_bits;
  dev_mode_desc.dmDisplayFrequency = mode_desc.refresh_rate;
  dev_mode_desc.dmFields           = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | (mode_desc.refresh_rate ? DM_DISPLAYFREQUENCY : 0);

  LONG result = ChangeDisplaySettings (&dev_mode_desc, CDS_FULLSCREEN);

  if (result != DISP_CHANGE_SUCCESSFUL)
    RaiseInvalidOperation ("render::low_level::opengl::Output::SetCurrentMode",
    "ChangeDisplaySettings fail: name='%s', win-name='%s', mode='%ux%ux%u (%uHz)'",
    name.c_str (), win_name.c_str (), mode_desc.width, mode_desc.height, mode_desc.color_bits, mode_desc.refresh_rate);      
}

void Output::GetCurrentMode (OutputModeDesc& mode_desc)
{
  get_mode_desc (win_name.c_str (), ENUM_CURRENT_SETTINGS, mode_desc);    
}

/*
    ���������� �����-����������
*/

void Output::SetGammaRamp (const Color3f table [256])
{
  RaiseNotImplemented ("render::low_level::opengl::Output::SetGammaRamp");
}

void Output::GetGammaRamp (Color3f table [256])
{
  RaiseNotImplemented ("render::low_level::opengl::Output::GetGammaRamp");
}

/*
    ������ ������� ���������� ������
*/

IPropertyList* Output::GetProperties ()
{
  return &properties;
}
