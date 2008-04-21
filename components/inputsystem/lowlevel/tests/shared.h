#ifndef INPUT_LOW_LEVEL_TESTS_SHARED_HEADER
#define INPUT_LOW_LEVEL_TESTS_SHARED_HEADER

#include <stdio.h>
#include <string.h>

#include <xtl/intrusive_ptr.h>
#include <xtl/function.h>
#include <xtl/reference_counter.h>

#include <common/exception.h>

#include <input/low_level/driver.h>
#include <input/low_level/device.h>

using namespace input::low_level;
using namespace common;

#ifdef _MSC_VER
  #pragma warning (disable : 4250)
#endif

const char* DEVICE_PROPERTY1 = "Axis_X_dead_zone";
const char* DEVICE_PROPERTY2 = "Axis_X_saturation_zone";

//�������� ���������� �����
class TestInput: virtual public IDevice, public xtl::reference_counter
{
  public:
    TestInput () : dead_zone (0), saturation_zone (0) {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetName () { return "TestInput"; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetEventHandler (const EventHandler& handler)
    {
      RaiseNotImplemented ("TestInput::SetEventHandler");
    }
    void GetEventHandler ()
    {
      RaiseNotImplemented ("TestInput::GetEventHandler");
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetProperties () {return "Axis_X_dead_zone Axis_X_saturation_zone";}
    void        SetProperty   (const char* name, float value) 
    {
      if (!strcmp (DEVICE_PROPERTY1, name))
        dead_zone = value;
      else if (!strcmp (DEVICE_PROPERTY2, name))
        saturation_zone = value;
      else
        RaiseInvalidArgument ("TestInput::GetProperty", "name", name);    
    }
    float GetProperty   (const char* name)
    {
      if (!strcmp (DEVICE_PROPERTY1, name))
        return dead_zone;
      else if (!strcmp (DEVICE_PROPERTY2, name))
        return saturation_zone;
      else
        RaiseInvalidArgument ("TestInput::GetProperty", "name", name);

      return 0;
    }    
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef () { addref (this); }  
    void Release () { release (this); }

  private:
    float dead_zone;
    float saturation_zone;
};

//�������� �������
class TestDriver: virtual public IDriver, public xtl::reference_counter
{
  public:
    TestDriver () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription () { return "TestDriver"; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t      GetDevicesCount ()
    {
      RaiseNotImplemented ("TestDriver::GetDevicesCount");
      return 0;
    }
    const char* GetDeviceName (size_t index)
    {
      RaiseNotImplemented ("TestDriver::GetDeviceName");
      return 0;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* CreateDevice (const char* name_mask) 
    {
      RaiseNotImplemented ("TestDriver::CreateDevice");
      return 0;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ����������� ���������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////   
    void SetDebugLog (const LogHandler& in_log)
    {
      log_fn = in_log;
    }
    
    const LogHandler& GetDebugLog ()
    {
      return log_fn;
    }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef () { addref (this); }  
    void Release () { release (this); }

  private:
    LogHandler log_fn;
};

#endif
