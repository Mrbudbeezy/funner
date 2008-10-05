#ifndef INPUT_LOW_LEVEL_DIRECT_INPUT_DRIVER_SHARED_HEADER
#define INPUT_LOW_LEVEL_DIRECT_INPUT_DRIVER_SHARED_HEADER

#include <stl/hash_map>
#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/connection.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/component.h>
#include <common/singleton.h>
#include <common/strlib.h>
#include <common/utf_converter.h>

#include <syslib/timer.h>
#include <syslib/window.h>

#include <input/low_level/device.h>
#include <input/low_level/driver.h>

#define DIRECTINPUT_VERSION 0x0800

#include <Dinput.h>

#pragma pack (1)

namespace input
{

namespace low_level
{

namespace direct_input_driver
{

enum ObjectType
{
  ObjectType_AbsoluteAxis,
  ObjectType_RelativeAxis,
  ObjectType_Button,
  ObjectType_POV,
  ObjectType_Unknown
};

class OtherDevice: virtual public input::low_level::IDevice, public xtl::reference_counter
{
  public:
    typedef input::low_level::IDriver::LogHandler DebugLogHandler;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    OtherDevice (syslib::Window* window, const char* name, IDirectInputDevice8* direct_input_device_interface, REFGUID rguid, 
                 const DebugLogHandler& debug_log_handler, const char* device_type, const char* init_string = "");
    ~OtherDevice ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetName () { return name.c_str (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��� ���������� (���.���.�������������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetFullName () { return full_name.c_str (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const wchar_t* GetControlName (const char* control_id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetEventHandler (const input::low_level::IDevice::EventHandler& handler)
    {
      event_handler = handler;
    }
    const input::low_level::IDevice::EventHandler& GetEventHandler ()
    {
      return event_handler;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetProperties () { return properties.c_str (); }
    void        SetProperty   (const char* name, float value);
    float       GetProperty   (const char* name);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef () { addref (this); }  
    void Release () { release (this); }

  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterObject (const char* name, size_t offset, ObjectType type);
    void RegisterObject (const wchar_t* name, size_t offset, ObjectType type);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RenameObject (size_t object_offset, const char* new_name);

  private:
    enum ObjectPropertyType
    {
      ObjectPropertyType_Sensitivity,
      ObjectPropertyType_DeadZone,
      ObjectPropertyType_Saturation,

      ObjectPropertyType_Num
    };

    struct ObjectPropertyMapElement
    {
      float              value;
      size_t             object_offset;
      ObjectPropertyType property_type;

      ObjectPropertyMapElement ();
      ObjectPropertyMapElement (float in_value, size_t in_object_offset, ObjectPropertyType in_property_type) 
        : value (in_value), object_offset (in_object_offset), property_type (in_property_type)
        {}
    };

    typedef stl::hash_map<stl::hash_key<const char*>, ObjectPropertyMapElement> ObjectPropertyMap;

    struct ObjectData
    {
      stl::string                 name;
      stl::wstring                unicode_name;
      size_t                      offset;
      ObjectType                  type;
      DWORD                       min_value;                          //����������� �������� (��� �������� ���� ���)
      DWORD                       max_value;                          //������������ �������� (��� �������� ���� ���)
      bool                        bad_object;                         //true ��� ������� ���� ���, ���� ������ �������� ������� ��������
      ObjectPropertyMap::iterator properties[ObjectPropertyType_Num]; //��������
      DWORD                       last_value;                         //��������� ��������

      ObjectData () {}
      ObjectData (const char* in_name, const wchar_t* in_unicode_name, size_t in_offset, ObjectType in_type) 
        : name (in_name), unicode_name (in_unicode_name), offset (in_offset), type (in_type), min_value (-65536), max_value (65535), bad_object (0)
        {}
    };

    typedef xtl::com_ptr<IDirectInputDevice8>                               DirectInputDeviceInterfacePtr;
    typedef stl::hash_map<size_t, ObjectData>                               ObjectsMap;
    typedef stl::hash_map<stl::hash_key<const char*>, ObjectsMap::iterator> ObjectsNamesMap;
    typedef xtl::uninitialized_storage<char>                                DeviceDataBuffer;
    typedef xtl::uninitialized_storage<char>                                EventStringBuffer;
    typedef xtl::uninitialized_storage<DIDEVICEOBJECTDATA>                  EventsBuffer;

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void PollDevice ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddProperty (const char* property_name, ObjectsMap::iterator object_iter, ObjectPropertyType property_type, float default_value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetDwordProperty (REFGUID property, DWORD value, DWORD object_offset);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool ReAcquireDevice ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���� init_string
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ProcessInitStringProperty (const char* property, const char* value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ProcessEvent (const char* event);

  private:
    OtherDevice (const OtherDevice& source);             //no impl
    OtherDevice& operator = (const OtherDevice& source); //no impl
    OtherDevice ();                                      //no impl

  private:
    stl::string                             name;
    stl::string                             full_name;
    DirectInputDeviceInterfacePtr           device_interface;
    input::low_level::IDevice::EventHandler event_handler;
    syslib::Timer                           poll_timer;
    ObjectsMap                              objects;
    ObjectsNamesMap                         objects_names;
    bool                                    device_lost;
    ObjectPropertyMap                       objects_properties_map;
    stl::string                             properties;
    DebugLogHandler                         debug_log_handler;
    size_t                                  events_buffer_size;     //������ ������ ������� (���� 0 - ������������ ����� ����������� ���������)
    EventsBuffer                            events_buffer;
    DeviceDataBuffer                        last_device_data;
    DeviceDataBuffer                        current_device_data;
    EventStringBuffer                       event_string_buffer;
    size_t                                  current_axis;
    size_t                                  current_button;
    size_t                                  current_pov;
    size_t                                  current_unknown;
};

const char* get_direct_input_error_name (HRESULT error);

}

}

}

#endif
