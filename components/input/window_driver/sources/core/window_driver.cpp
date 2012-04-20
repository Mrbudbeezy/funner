#include "shared.h"

using namespace common;
using namespace stl;
using namespace syslib;

namespace input
{

namespace low_level
{

namespace window
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Window ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Driver: virtual public IDriver, public xtl::reference_counter
{
  public:
    Driver () {}

    ~Driver ()
    {
      UnregisterDriver ();
      UnregisterAllDevices ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription () { return "Window driver"; }

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
        throw xtl::make_range_exception ("input::low_level::window::Driver::GetDeviceName", "index", index, 0u, GetDevicesCount ());

      return device_entries[index]->device_name.c_str ();
    }

    const char* GetDeviceFullName (size_t index)
    {
      if (index >= GetDevicesCount ())
        throw xtl::make_range_exception ("input::low_level::window::Driver::GetDeviceFullName", "index", index, 0u, GetDevicesCount ());

      return device_entries[index]->device_full_name.c_str ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* CreateDevice (const char* name, const char*)
    {
      for (DeviceEntries::iterator iter = device_entries.begin (), end = device_entries.end (); iter != end; ++iter)
        if (!xtl::xstrcmp ((*iter)->device_full_name.c_str (), name))
          return new Device ((*iter)->window, name, (*iter)->device_full_name.c_str ());

      throw xtl::make_argument_exception ("input::low_level::window::Driver::CreateDevice", "name", name);
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
///����������� ���� ��� ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterDevice (const char* device_name, Window& window)
    {
      for (DeviceEntries::iterator iter = device_entries.begin (), end = device_entries.end (); iter != end; ++iter)
        if (!strcmp ((*iter)->device_name.c_str (), device_name))
          throw xtl::make_argument_exception ("input::low_level::window::Driver::RegisterDevice", "device_name", device_name, "Name already registered");

      xtl::auto_connection c = window.RegisterEventHandler (WindowEvent_OnDestroy, xtl::bind (&Driver::DestroyWindowHandler, this, _1, _2, _3));

      string device_full_name = "window";

      if (xtl::xstrlen (device_name))
      {
        device_full_name += '.';
        device_full_name += device_name;
      }

      device_entries.push_back (DeviceEntryPtr (new DeviceEntry (device_name, device_full_name.c_str (), &window, c), false));

      if (device_entries.size () == 1)
        DriverManager::RegisterDriver (GetDescription (), this);
    }

    void UnregisterDevice (const char* device_name)
    {
      for (DeviceEntries::iterator iter = device_entries.begin (), end = device_entries.end (); iter != end; ++iter)
        if (!strcmp ((*iter)->device_name.c_str (), device_name))
        {
          device_entries.erase (iter);

          if (device_entries.empty ())
            UnregisterDriver ();

          return;
        }
    }

    void UnregisterAllDevices (Window& window)
    {
      for (DeviceEntries::iterator iter = device_entries.begin (); iter != device_entries.end (); ++iter)
        if ((*iter)->window == &window)
          device_entries.erase (iter--);

      if (device_entries.empty ())
        UnregisterDriver ();
    }

    void UnregisterAllDevices ()
    {
      device_entries.clear ();
      UnregisterDriver ();
    }

  private:
    Driver (const Driver& source);              //no impl
    Driver& operator = (const Driver& source);  //no impl

  private:
    void UnregisterDriver ()
    {
      DriverManager::UnregisterDriver (GetDescription ());
    }

    void DestroyWindowHandler (Window& window, WindowEvent, const WindowEventContext&)
    {
      UnregisterAllDevices (window);
    }

  private:
    struct DeviceEntry: public xtl::reference_counter
    {
      string               device_name;
      string               device_full_name;
      Window*              window;
      xtl::auto_connection on_window_destroy_connection;

      DeviceEntry (const char* in_device_name, const char* in_device_full_name, Window* in_window, xtl::connection& in_connection)
        : device_name (in_device_name), device_full_name (in_device_full_name), window (in_window)
      {
        on_window_destroy_connection.swap (in_connection);
      }
    };

    typedef xtl::intrusive_ptr<DeviceEntry> DeviceEntryPtr;
    typedef vector<DeviceEntryPtr>          DeviceEntries;

  private:
    DeviceEntries   device_entries;
    LogHandler      log_fn;
};


}

}

}

namespace
{

typedef Singleton<input::low_level::window::Driver> WindowDriverSingleton;

}

namespace input
{

namespace low_level
{

/*
   Window-������� �����
*/

/*
   ��� ��������
*/

const char* WindowDriver::Name ()
{
  return WindowDriverSingleton::Instance ()->GetDescription ();
}

/*
   ��������� ��������
*/

IDriver* WindowDriver::Driver ()
{
  return &*WindowDriverSingleton::Instance ();
}

/*
   ����������� ���� ��� ���������� �����
*/

void WindowDriver::RegisterDevice (const char* device_name, syslib::Window& window)
{
  if (!device_name)
    throw xtl::make_null_argument_exception ("input::low_level::WindowDriver::RegisterDevice", "device_name");

  WindowDriverSingleton::Instance ()->RegisterDevice (device_name, window);
}

void WindowDriver::RegisterDevice (syslib::Window& window)
{
  RegisterDevice (window.Title (), window);
}

void WindowDriver::UnregisterDevice (const char* device_name)
{
  if (!device_name)
    throw xtl::make_null_argument_exception ("input::low_level::WindowDriver::UnregisterDevice", "device_name");

  WindowDriverSingleton::Instance ()->UnregisterDevice (device_name);
}

void WindowDriver::UnregisterAllDevices (syslib::Window& window)
{
  WindowDriverSingleton::Instance ()->UnregisterAllDevices (window);
}

void WindowDriver::UnregisterAllDevices ()
{
  WindowDriverSingleton::Instance ()->UnregisterAllDevices ();
}

}

}
