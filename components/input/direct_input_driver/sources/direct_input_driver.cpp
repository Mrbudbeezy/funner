#include "shared.h"

using namespace common;

namespace
{

const char* COMPONENT_NAME = "input.low_level.direct_input8"; //��� ����������
const char* DRIVER_NAME    = "DirectInput8";                  //��� ��������

void default_log_handler (const char*)
{
}

BOOL FAR PASCAL enum_devices_callback (LPCDIDEVICEINSTANCE device_instance, LPVOID direct_input_driver);

const char* get_com_error_name (HRESULT error)
{
  switch (error)
  {
    case CLASS_E_NOAGGREGATION: return "CLASS_E_NOAGGREGATION";
    case E_INVALIDARG:          return "E_INVALIDARG";
    case E_NOINTERFACE:         return "E_NOINTERFACE";
    case E_OUTOFMEMORY:         return "E_OUTOFMEMORY";
    case E_UNEXPECTED:          return "E_UNEXPECTED";
    case REGDB_E_CLASSNOTREG:   return "REGDB_E_CLASSNOTREG";
    case RPC_E_CHANGED_MODE:    return "RPC_E_CHANGED_MODE";
    default:                    return "Unknown";
  }
}

enum DirectInputDeviceType
{
  DirectInputDeviceType_GameControl,
  DirectInputDeviceType_Keyboard,
  DirectInputDeviceType_Pointer,
  DirectInputDeviceType_Other
};

struct ComInitializer
{
  ComInitializer ()
  {
    HRESULT create_result;

    create_result = CoInitialize (0);

    switch (create_result)
    {
      case S_OK:
      case S_FALSE:
        break;
      default:
        throw xtl::format_operation_exception ("ComInitializer::ComInitializer", "Can't initialize COM, error '%s'.", get_com_error_name (create_result));
    }
  }

  ~ComInitializer ()
  {
    CoUninitialize ();
  }
};

typedef common::Singleton<ComInitializer> ComInitializerSingleton;

}

namespace input
{

namespace low_level
{

namespace direct_input_driver
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Window ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Driver: virtual public IDriver, public xtl::reference_counter
{
  public:
    Driver () 
      : log_fn (&default_log_handler)
    {
      static const char* METHOD_NAME = "input::low_level::direct_input_driver::Driver::Driver";

      try
      {
        ComInitializerSingleton::Instance ();
      }
      catch (xtl::exception& exception)
      {
        exception.touch (METHOD_NAME);
      }

      HRESULT create_result;

      create_result = CoCreateInstance (CLSID_DirectInput8, 0, CLSCTX_INPROC_SERVER, IID_IDirectInput8A, (LPVOID *)&direct_input_interface);

      if (create_result != S_OK)
        throw xtl::format_operation_exception (METHOD_NAME, "Can't create direct input 8 interface, error '%s'", get_com_error_name (create_result));

      create_result = direct_input_interface->Initialize (GetModuleHandle (0), DIRECTINPUT_VERSION);

      if (create_result != DI_OK)
        throw xtl::format_operation_exception (METHOD_NAME, "Can't initialize direct input interface, error '%s'", get_direct_input_error_name (create_result));

      RegisterDevices ();
    }

    ~Driver () 
    { 
      UnregisterDriver ();

      direct_input_interface->Release ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription () { return "input::low_level::direct_input_driver::Driver"; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetDevicesCount () 
    { 
      return device_entries.size (); 
    }

    const char* GetDeviceName (size_t index)
    {
      if (index >= GetDevicesCount ())
        throw xtl::make_range_exception ("input::low_level::direct_input_driver::Driver::GetDeviceName", "index", index, 0u, GetDevicesCount ());

      return device_entries[index]->device_name.c_str ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* CreateDevice (const char* name, const char* init_string = "") 
    {
      static const char* METHOD_NAME = "input::low_level::direct_input_driver::Driver::CreateDevice";

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");

      if (!init_string)
        init_string = "";

      for (DeviceEntries::iterator iter = device_entries.begin (), end = device_entries.end (); iter != end; ++iter)
        if (!strcmp ((*iter)->device_name.c_str (), name))
        {
          HRESULT create_result;

          IDirectInputDevice8 *device_interface;

          create_result = direct_input_interface->CreateDevice ((*iter)->device_guid, &device_interface, NULL);

          if (create_result != DI_OK)
            throw xtl::format_operation_exception (METHOD_NAME, "Can't create direct input device, error '%s'", get_direct_input_error_name (create_result));

          return new OtherDevice (&dummy_window, name, device_interface, (*iter)->device_guid, log_fn, init_string);
        }

      throw xtl::make_argument_exception (METHOD_NAME, "name", name);
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
    void AddRef () {}  
    void Release () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ���������� ����� �� ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterDevice (const char* device_name, REFGUID device_guid)
    {
      direct_input_device_entries.push_back (DirectInputDeviceEntry (device_name, device_guid, current_device_type));
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ���� ��� ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterDevices ()
    {
      static const char* METHOD_NAME = "input::low_level::direct_input_driver::Driver::RegisterDevices";

      current_device_type = DirectInputDeviceType_GameControl;
      direct_input_interface->EnumDevices (DI8DEVCLASS_GAMECTRL, &enum_devices_callback, this, DIEDFL_ATTACHEDONLY);

      current_device_type = DirectInputDeviceType_Keyboard;
      direct_input_interface->EnumDevices (DI8DEVCLASS_KEYBOARD, &enum_devices_callback, this, DIEDFL_ATTACHEDONLY);

      current_device_type = DirectInputDeviceType_Pointer;
      direct_input_interface->EnumDevices (DI8DEVCLASS_POINTER, &enum_devices_callback, this, DIEDFL_ATTACHEDONLY);

      current_device_type = DirectInputDeviceType_Other;
      direct_input_interface->EnumDevices (DI8DEVCLASS_DEVICE, &enum_devices_callback, this, DIEDFL_ATTACHEDONLY);

      if (device_entries.empty () && !direct_input_device_entries.empty ())
        DriverManager::RegisterDriver (GetDescription (), this);

      for (DirectInputDeviceEntries::iterator iter = direct_input_device_entries.begin (), end = direct_input_device_entries.end (); iter != end; ++iter)
        device_entries.push_back (DeviceEntryPtr (new DeviceEntry (iter->device_name.c_str (), iter->device_guid, iter->device_type)));

      direct_input_device_entries.clear ();
    }

  private:
    Driver (const Driver& source);              //no impl
    Driver& operator = (const Driver& source);  //no impl

  private:
    void UnregisterDriver ()
    {
      DriverManager::UnregisterDriver (GetDescription ());
    }

  private:
    struct DirectInputDeviceEntry
    {
      stl::string           device_name;
      GUID                  device_guid;
      DirectInputDeviceType device_type;

      DirectInputDeviceEntry (const char* in_device_name, const GUID& in_device_guid, DirectInputDeviceType in_device_type)
        : device_name (in_device_name), device_guid (in_device_guid), device_type (in_device_type)
        {}
    };

    struct DeviceEntry : public xtl::reference_counter
    {
      stl::string           device_name;
      GUID                  device_guid;
      DirectInputDeviceType device_type;

      DeviceEntry (const char* in_device_name, const GUID& in_device_guid, DirectInputDeviceType in_device_type)
        : device_name (in_device_name), device_guid (in_device_guid), device_type (in_device_type)
        {}
    };

    typedef xtl::intrusive_ptr<DeviceEntry>     DeviceEntryPtr;
    typedef stl::vector<DeviceEntryPtr>         DeviceEntries;
    typedef stl::vector<DirectInputDeviceEntry> DirectInputDeviceEntries;

  private:
    DirectInputDeviceEntries direct_input_device_entries;
    DeviceEntries            device_entries;
    LogHandler               log_fn;
    IDirectInput8            *direct_input_interface;  
    DirectInputDeviceType    current_device_type;
    syslib::Window           dummy_window;
};

}

}

}

namespace
{

using input::low_level::direct_input_driver::Driver;

BOOL FAR PASCAL enum_devices_callback (LPCDIDEVICEINSTANCE device_instance, LPVOID direct_input_driver)
{
#ifdef DIRECT_INPUT_KEYBOARD_DISABLED
  if (device_instance->guidInstance != GUID_SysKeyboard)
#endif
    ((Driver*)direct_input_driver)->RegisterDevice (device_instance->tszInstanceName, device_instance->guidInstance);
  
  return DIENUM_CONTINUE;
}

}

/*
    ��������� ��������
*/

namespace
{

class DirectInputDriverComponent
{
  public:
    DirectInputDriverComponent ()
    {
      input::low_level::DriverManager::RegisterDriver (DRIVER_NAME, xtl::com_ptr<Driver> (new Driver, false).get ());
    }
};

}

extern "C"
{

common::ComponentRegistrator<DirectInputDriverComponent> DirectInput8Driver (COMPONENT_NAME);

}
