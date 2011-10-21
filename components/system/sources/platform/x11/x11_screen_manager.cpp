#if 0

#include "../shared.h"

using namespace syslib;

namespace
{

/*
    ���������
*/

const size_t DEFAULT_SCREEN_ARRAY_RESERVE = 8;            //������������� ���������� �������
const size_t MODE_ARRAY_RESERVE           = 32;           //������������� ���������� �����-�������
const char*  LOG_NAME                     = "system.x11"; //��� ��������� ������ ���������

/*
    ��������� ������
*/

typedef stl::vector<ScreenModeDesc> ScreenModeArray;

struct ScreenDesc: public xtl::reference_counter
{
  stl::string     name;           //��� ����������
  ScreenModeArray modes;          //������ ������
  
///�����������
  ScreenDesc (const char* in_name, const char* in_win_name)
    : device_context ()
    , name (in_name)
    , win_name (in_win_name)
  {
      //��������� ����������� �������� � ����� ����������

    stl::string::reverse_iterator iter;

    for (iter=name.rbegin (); iter!=name.rend () && *iter == ' '; ++iter);

    name.erase (iter.base (), name.end ());    
    
      //���������� ������ �����-�������
      
    modes.reserve (MODE_ARRAY_RESERVE);

    ScreenModeDesc mode_desc;

    for (int mode_index=0; get_mode_desc (win_name.c_str (), mode_index, mode_desc); mode_index++)
      modes.push_back (mode_desc);
  }
  
///����������
  ~ScreenDesc ()
  {

  }
};

typedef xtl::intrusive_ptr<ScreenDesc> ScreenDescPtr;
typedef stl::vector<ScreenDescPtr>     ScreenDescArray;

/*
    �������� �������
*/

struct ScreenManagerImpl
{
  ScreenDescArray screens; //������
  
///�����������
  ScreenManagerImpl ()
  {
    screens.reserve (DEFAULT_SCREEN_ARRAY_RESERVE);  

      //���������� ������� �������

  }
  
///��������� ������
  static ScreenDescPtr GetScreen (screen_t handle)
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("syslib::x11::ScreenManagerImpl::GetScreen", "handle");
    
    return (ScreenDesc*)handle;
  }
};

typedef common::Singleton<ScreenManagerImpl> ScreenManagerSingleton;

}

/*
    ������������ �������
*/

size_t XlibScreenManager::GetScreensCount ()
{
  return ScreenManagerSingleton::Instance ()->screens.size ();
}

/*
    �������� / �������� ������
*/

screen_t XlibScreenManager::CreateScreen (size_t screen_index)
{  
  ScreenManagerSingleton::Instance instance;

  size_t screens_count = instance->screens.size ();
  
  if (screen_index >= screens_count)
    throw xtl::make_range_exception ("syslib::XlibScreenManager::CreateScreen", "screen_index", screen_index, screens_count);
    
  ScreenDescPtr screen = instance->screens [screen_index];
  
  addref (&*screen);

  return (screen_t)screen.get ();
}

void XlibScreenManager::DestroyScreen (screen_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::XlibScreenManager::DestroyScreen", "handle");
    
  ScreenDescPtr screen ((ScreenDesc*)handle);
  
  release (&*screen);
}

/*
    ��� ������
*/

const char* XlibScreenManager::GetScreenName (screen_t handle)
{
  try
  {
    ScreenDescPtr screen = ScreenManagerImpl::GetScreen (handle);

  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibScreenManager::GetScreenName");
    throw;
  }
}

/*
    ��������� ������ �����-������� ������
*/

size_t XlibScreenManager::GetScreenModesCount (screen_t handle)
{
  try
  {
    ScreenDescPtr screen = ScreenManagerImpl::GetScreen (handle);

  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibScreenManager::GetScreenModesCount");
    throw;
  }
}

void XlibScreenManager::GetScreenMode (screen_t handle, size_t mode_index, ScreenModeDesc& mode_desc)
{
  try
  {
    ScreenDescPtr screen = ScreenManagerImpl::GetScreen (handle);
    

  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibScreenManager::GetScreenMode");
    throw;
  }
}

/*
    ��������� �������� �����-������ ������
*/

void XlibScreenManager::SetScreenCurrentMode (screen_t handle, const ScreenModeDesc& mode_desc)
{
  const char* device_name = "null";

  try
  {    
    ScreenDescPtr screen = ScreenManagerImpl::GetScreen (handle);
    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibScreenManager::SetCurrentMode(device-name='%s', mode='%ux%ux%u @ %uHz')", device_name, mode_desc.width, mode_desc.height, mode_desc.color_bits, mode_desc.refresh_rate);
    throw;
  }
}

void XlibScreenManager::GetScreenCurrentMode (screen_t handle, ScreenModeDesc& mode_desc)
{
  try
  {
    ScreenDescPtr screen = ScreenManagerImpl::GetScreen (handle);    
    

  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibScreenManager::GetScreenCurrentMode");
    throw;
  }
}

void XlibScreenManager::GetScreenDefaultMode (screen_t handle, ScreenModeDesc& mode_desc)
{
  try
  {
    ScreenDescPtr screen = ScreenManagerImpl::GetScreen (handle);
    

  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibScreenManager::GetScreenDefaultMode");
    throw;
  }
}

/*
    ���������� �����-���������� ������
*/

void XlibScreenManager::SetScreenGammaRamp (screen_t handle, const Color3f table [256])
{
  try
  {
    ScreenDescPtr screen = ScreenManagerImpl::GetScreen (handle);
    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibScreenManager::SetScreenGammaRamp");
    throw;
  }
}

void XlibScreenManager::GetScreenGammaRamp (screen_t handle, Color3f table [256])
{
  try
  {
    ScreenDescPtr screen = ScreenManagerImpl::GetScreen (handle);

  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibScreenManager::GetScreenGammaRamp");
    throw;
  }
}

/*
    ����� ������ ���������� ����
*/

screen_t XlibScreenManager::FindContainingScreen (const void* native_window_handle)
{
  try
  {
    ScreenManagerSingleton::Instance instance;
    

  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibScreenManager::FindContainingScreen");
    throw;
  }
}

/*
    ��������� ���������-���������� ����������� ������
*/

const void* XlibScreenManager::GetNativeScreenHandle (screen_t handle)
{  
  try
  {
    ScreenDescPtr screen = ScreenManagerImpl::GetScreen (handle);    
    

  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibScreenManager::GetNativeScreenHandle");
    throw;
  }
}

/*
    ��������� ���������-��������� ������� ������
*/

void XlibScreenManager::GetScreenProperties (screen_t handle, common::PropertyMap& properties)
{
  try
  {
    ScreenDescPtr screen = ScreenManagerImpl::GetScreen (handle);    
    

  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibScreenManager::GetScreenProperties");
    throw;
  }
}

#endif
