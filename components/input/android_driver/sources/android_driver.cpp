#include "shared.h"

using namespace input::low_level::android_driver;

namespace
{

const char*  COMPONENT_NAME         = "input.low_level.android"; //��� ����������
const char*  DRIVER_NAME            = "Android";                 //��� ��������
const size_t RESERVED_DEVICES_COUNT = 8;                         //������������� ���������� ���������

}

namespace input
{

namespace low_level
{

namespace android_driver
{

///Android ������� �����
class Driver: virtual public IDriver, public xtl::reference_counter
{
  public:
///�����������/����������
    Driver ()
    {
      devices.reserve (RESERVED_DEVICES_COUNT);
      
      SensorManagerSingleton::Instance manager;
      
      for (size_t i=0, count=manager->SensorsCount (); i<count; i++)
      {
        DevicePtr device (new DeviceDesc, false);
        
        device->name      = manager->SensorName (i);
        device->full_name = manager->SensorFullName (i);
        device->creator   = xtl::bind (&Driver::CreateSensor, i);
        
        devices.push_back (device);
      }
    }

    ~Driver ()
    {
    }
    
///�������� �������
    static IDevice* CreateSensor (size_t index)
    {
      return SensorManagerSingleton::Instance ()->CreateSensor (index);
    }

///�������� ��������
    const char* GetDescription () { return "input::low_level::android_driver::Driver"; }

///������������ ��������� ��������� �����
    size_t GetDevicesCount () 
    { 
      return devices.size ();
    }

    const char* GetDeviceName (size_t index)
    {
      if (index >= devices.size ())
        throw xtl::make_range_exception ("input::low_level::android_driver::Driver::GetDeviceName", "index", index, 0u, devices.size ());

      return devices [index]->name.c_str ();
    }

    const char* GetDeviceFullName (size_t index)
    {
      if (index >= devices.size ())
        throw xtl::make_range_exception ("input::low_level::android_driver::Driver::GetDeviceFullName", "index", index, 0u, devices.size ());

      return devices [index]->full_name.c_str ();
    }

///��������� ���������� �����
    IDevice* CreateDevice (const char* name, const char* init_string = "") 
    {
      try
      {
        if (!name)
          throw xtl::make_null_argument_exception ("", "name");

        if (!init_string)
          init_string = "";

        for (size_t i = 0, count = devices.size (); i < count; i++)
        {
          if (!xtl::xstrcmp (devices [i]->name.c_str (), name))
            return devices [i]->creator (name, init_string);
        }

        throw xtl::make_argument_exception ("", "name", name);
      }
      catch (xtl::exception& e)
      {
        e.touch ("input::low_level::android_driver::Driver::CreateDevice");
        throw;
      }
    }

///��������� ������� ����������� ���������������� ��������
    void SetDebugLog (const LogHandler& in_log)
    {
      log_fn = in_log;
    }
    
    const LogHandler& GetDebugLog ()
    {
      return log_fn;
    }
    
///������� ������
    void AddRef () {}  
    void Release () {}

  private:
    Driver (const Driver& source);              //no impl
    Driver& operator = (const Driver& source);  //no impl
    
  private:
    typedef xtl::function<IDevice* (const char*, const char*)> DeviceCreator;
  
    struct DeviceDesc: public xtl::reference_counter
    {
      stl::string   name;
      stl::string   full_name;
      DeviceCreator creator;
    };
    
    typedef xtl::intrusive_ptr<DeviceDesc> DevicePtr;
    typedef stl::vector<DevicePtr>         DeviceList;

  private:
    LogHandler log_fn;
    DeviceList devices;
};

}

}

}

/*
    ��������� ��������
*/

namespace components
{

namespace android_input_driver
{

class AndroidDriverComponent
{
  public:
    AndroidDriverComponent ()
    {
      input::low_level::DriverManager::RegisterDriver (DRIVER_NAME, xtl::com_ptr<Driver> (new Driver, false).get ());
    }
};

extern "C"
{

common::ComponentRegistrator<AndroidDriverComponent> AndroidInputDriver (COMPONENT_NAME);

}

}

}
