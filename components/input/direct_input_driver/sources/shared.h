#ifndef INPUT_LOW_LEVEL_DIRECT_INPUT_DRIVER_SHARED_HEADER
#define INPUT_LOW_LEVEL_DIRECT_INPUT_DRIVER_SHARED_HEADER

#include <stl/hash_map>
#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/signal.h>

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
///Конструктор/деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    OtherDevice (syslib::Window* window, const char* name, const char* full_name, IDirectInputDevice8* direct_input_device_interface,
                 const DebugLogHandler& debug_log_handler, const char* device_type, const char* init_string = "");
    ~OtherDevice ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение имени устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetName () { return name.c_str (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Полное имя устройства (тип.имя.идентификатор)
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetFullName () { return full_name.c_str (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение имени контрола
///////////////////////////////////////////////////////////////////////////////////////////////////
    const wchar_t* GetControlName (const char* control_id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подписка на события устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::connection RegisterEventHandler (const input::low_level::IDevice::EventHandler& handler);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Настройки устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetProperties () { return properties.c_str (); }
    void        SetProperty   (const char* name, float value);
    float       GetProperty   (const char* name);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef () { addref (this); }  
    void Release () { release (this); }

  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация объекта
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterObject (const char* name, unsigned int offset, ObjectType type);
    void RegisterObject (const wchar_t* name, unsigned int offset, ObjectType type);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Переименование объекта
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
      unsigned int       object_offset;
      ObjectPropertyType property_type;

      ObjectPropertyMapElement ();
      ObjectPropertyMapElement (float in_value, unsigned int in_object_offset, ObjectPropertyType in_property_type)
        : value (in_value), object_offset (in_object_offset), property_type (in_property_type)
        {}
    };

    typedef stl::hash_map<stl::hash_key<const char*>, ObjectPropertyMapElement> ObjectPropertyMap;

    struct ObjectData
    {
      stl::string                 name;
      stl::wstring                unicode_name;
      unsigned int                offset;
      ObjectType                  type;
      int                         min_value;                          //минимальное значение (для объектов типа ось)
      int                         max_value;                          //максимальное значение (для объектов типа ось)
      bool                        bad_object;                         //true для объекта типа ось, если нельзя получить пределы значения
      ObjectPropertyMap::iterator properties[ObjectPropertyType_Num]; //свойства
      DWORD                       last_value;                         //последнее значение

      ObjectData () {}
      ObjectData (const char* in_name, const wchar_t* in_unicode_name, unsigned int in_offset, ObjectType in_type)
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
///Получение данных
///////////////////////////////////////////////////////////////////////////////////////////////////
    void PollDevice ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление свойства объекта
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddProperty (const char* property_name, ObjectsMap::iterator object_iter, ObjectPropertyType property_type, float default_value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка свойства объекта
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetDwordProperty (REFGUID property, DWORD value, DWORD object_offset);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Попытка переполучения устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool ReAcquireDevice ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработка поля init_string
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ProcessInitStringProperty (const char* property, const char* value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработка событий
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ProcessEvent (const char* event);

  private:
    OtherDevice (const OtherDevice& source);             //no impl
    OtherDevice& operator = (const OtherDevice& source); //no impl
    OtherDevice ();                                      //no impl

  private:
    typedef xtl::signal<void (const char*)> DeviceSignal;

  private:
    stl::string                   name;
    stl::string                   full_name;
    DirectInputDeviceInterfacePtr device_interface;
    DeviceSignal                  signals;
    syslib::Timer                 poll_timer;
    ObjectsMap                    objects;
    ObjectsNamesMap               objects_names;
    bool                          device_lost;
    ObjectPropertyMap             objects_properties_map;
    stl::string                   properties;
    DebugLogHandler               debug_log_handler;
    unsigned int                  events_buffer_size;     //размер буфера событий (если 0 - используется опрос мгновенного состояния)
    EventsBuffer                  events_buffer;
    DeviceDataBuffer              last_device_data;
    DeviceDataBuffer              current_device_data;
    EventStringBuffer             event_string_buffer;
    unsigned int                  current_axis;
    unsigned int                  current_button;
    unsigned int                  current_pov;
    unsigned int                  current_unknown;
};

const char* get_direct_input_error_name (HRESULT error);

}

}

}

#endif
