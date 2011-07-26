#include "shared.h"

using namespace input::low_level::iphone_driver;

namespace
{

const char* COMPONENT_NAME = "input.low_level.iphone"; //��� ����������
const char* DRIVER_NAME    = "IPhone";                 //��� ��������

struct DeviceDesc
{
  const char* name;
  const char* full_name;
};

const DeviceDesc ACCELEROMETER_DEVICE_DESC                  = { "iphone_accelerometer", "accelerometer.iphone_accelerometer" };
const DeviceDesc APPLICATION_DEVICE_DESC                    = { "iphone_application", "application.iphone_application" };
const DeviceDesc ASCII_KEYBOARD_DEVICE_DESC                 = { "ascii_keyboard", "keyboard.ascii_keyboard" };
const DeviceDesc ASCII_AUTOCAPITALIZED_KEYBOARD_DEVICE_DESC = { "ascii_autocapitalized_keyboard", "keyboard.ascii_autocapitalized_keyboard" };
const DeviceDesc NUMBER_KEYBOARD_DEVICE_DESC                = { "number_keyboard", "keyboard.number_keyboard" };
const DeviceDesc NUMBER_PUNCTUATION_KEYBOARD_DEVICE_DESC    = { "number_punctuation_keyboard", "keyboard.number_punctuation_keyboard" };

const DeviceDesc SUPPORTED_DEVICES [] = { ACCELEROMETER_DEVICE_DESC, APPLICATION_DEVICE_DESC, ASCII_KEYBOARD_DEVICE_DESC,
                                          ASCII_AUTOCAPITALIZED_KEYBOARD_DEVICE_DESC, NUMBER_KEYBOARD_DEVICE_DESC,
                                          NUMBER_PUNCTUATION_KEYBOARD_DEVICE_DESC };

}

namespace input
{

namespace low_level
{

namespace iphone_driver
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///iPhone ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Driver: virtual public IDriver, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Driver ()
    {
    }

    ~Driver ()
    {
      DriverManager::UnregisterDriver (GetDescription ());
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription () { return "input::low_level::iphone_driver::Driver"; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetDevicesCount () 
    { 
      return sizeof (SUPPORTED_DEVICES) / sizeof (SUPPORTED_DEVICES [0]);
    }

    const char* GetDeviceName (size_t index)
    {
      if (index >= GetDevicesCount ())
        throw xtl::make_range_exception ("input::low_level::iphone_driver::Driver::GetDeviceName", "index", index, 0u, GetDevicesCount ());

      return SUPPORTED_DEVICES [index].name;
    }

    const char* GetDeviceFullName (size_t index)
    {
      if (index >= GetDevicesCount ())
        throw xtl::make_range_exception ("input::low_level::iphone_driver::Driver::GetDeviceFullName", "index", index, 0u, GetDevicesCount ());

      return SUPPORTED_DEVICES [index].full_name;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* CreateDevice (const char* name, const char* init_string = "") 
    {
      static const char* METHOD_NAME = "input::low_level::iphone_driver::Driver::CreateDevice";

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");

      if (!init_string)
        init_string = "";

      for (size_t i = 0, count = GetDevicesCount (); i < count; i++)
      {
        const char* device_name = GetDeviceName (i);

        if (!xtl::xstrcmp (device_name, name))
        {
          if (!xtl::xstrcmp (device_name, ASCII_KEYBOARD_DEVICE_DESC.name))
            return new IPhoneKeyboard (ASCII_KEYBOARD_DEVICE_DESC.name, ASCII_KEYBOARD_DEVICE_DESC.full_name, KeyboardType_ASCII, AutocapitalizationType_None);
          else if (!xtl::xstrcmp (device_name, ASCII_AUTOCAPITALIZED_KEYBOARD_DEVICE_DESC.name))
            return new IPhoneKeyboard (ASCII_AUTOCAPITALIZED_KEYBOARD_DEVICE_DESC.name, ASCII_AUTOCAPITALIZED_KEYBOARD_DEVICE_DESC.full_name, KeyboardType_ASCII, AutocapitalizationType_Sentences);
          else if (!xtl::xstrcmp (device_name, NUMBER_KEYBOARD_DEVICE_DESC.name))
            return new IPhoneKeyboard (NUMBER_KEYBOARD_DEVICE_DESC.name, NUMBER_KEYBOARD_DEVICE_DESC.full_name, KeyboardType_NumberPad, AutocapitalizationType_None);
          else if (!xtl::xstrcmp (device_name, NUMBER_PUNCTUATION_KEYBOARD_DEVICE_DESC.name))
            return new IPhoneKeyboard (NUMBER_PUNCTUATION_KEYBOARD_DEVICE_DESC.name, NUMBER_PUNCTUATION_KEYBOARD_DEVICE_DESC.full_name, KeyboardType_NumbersAndPunctuation, AutocapitalizationType_None);
          else if (!xtl::xstrcmp (device_name, APPLICATION_DEVICE_DESC.name))
            return new IPhoneApplication (APPLICATION_DEVICE_DESC.name, APPLICATION_DEVICE_DESC.full_name);
          else if (!xtl::xstrcmp (device_name, ACCELEROMETER_DEVICE_DESC.name))
            return new IPhoneAccelerometer (ACCELEROMETER_DEVICE_DESC.name, ACCELEROMETER_DEVICE_DESC.full_name);
        }
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

  private:
    Driver (const Driver& source);              //no impl
    Driver& operator = (const Driver& source);  //no impl

  private:
    LogHandler log_fn;
};

}

}

}

/*
    ��������� ��������
*/

namespace
{

class IPhoneDriverComponent
{
  public:
    IPhoneDriverComponent ()
    {
      input::low_level::DriverManager::RegisterDriver (DRIVER_NAME, xtl::com_ptr<Driver> (new Driver, false).get ());
    }
};

}

extern "C"
{

common::ComponentRegistrator<IPhoneDriverComponent> IPhoneInputDriver (COMPONENT_NAME);

}
