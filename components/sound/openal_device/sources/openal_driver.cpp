#include "shared.h"

using namespace sound::low_level;

namespace components
{

namespace openal_driver
{

/*
    ���������
*/

const char* DRIVER_NAME    = "OpenAL";                 //��� ��������
const char* COMPONENT_NAME = "sound.low_level.openal"; //��� ����������
const char* LOG_NAME       = COMPONENT_NAME;           //����� ����������������

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������� ���������������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Driver : virtual public IDriver, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Driver ()
    {
      try
      {
        const char* devices_names = 0;

#ifdef ALC_ALL_DEVICES_SPECIFIER
        if (alcIsExtensionPresent (0, "ALC_ENUMERATE_ALL_EXT"))
          devices_names = alcGetString (0, ALC_ALL_DEVICES_SPECIFIER);
        else
#endif
        if (alcIsExtensionPresent (0, "ALC_ENUMERATION_EXT"))
          devices_names = alcGetString (0, ALC_DEVICE_SPECIFIER);

        if (devices_names)
        {
          if (!strlen(devices_names))
            devices.push_back ("default");

          for (; strlen (devices_names); devices_names += strlen (devices_names) + 1)
            devices.push_back (devices_names);
        }
        else
          devices.push_back ("default");

        common::Log log (LOG_NAME);

        log.Printf ("OpenAL driver has found %u possible devices:", devices.size ());

        for (DeviceArray::iterator iter = devices.begin (), end = devices.end (); iter != end; ++iter)
          log.Printf ("...OpenAL device '%s'", iter->c_str ());
      }
      catch (...)
      {
        DriverManager::UnregisterDriver (DRIVER_NAME);
        throw;
      }
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription ()
    {
      return "sound::low_level::openal::Driver";
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetDevicesCount ()
    {
      return devices.size ();
    }

    const char* GetDeviceName (size_t index)
    {
      if (index >= GetDevicesCount ())
        throw xtl::make_range_exception ("sound::low_level::openal::Driver::GetDeviceName", "index", index, 0u, GetDevicesCount ());

      return devices[index].c_str ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* CreateDevice (const char* name, const char* init_string)
    {
      static const char* METHOD_NAME = "sound::low_level::openal::Driver::CreateDevice";

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");

      if (!init_string)
        init_string = "";

      for (DeviceArray::iterator iter = devices.begin (), end = devices.end (); iter != end; ++iter)
        if (!strcmp (iter->c_str (), name))
        {
          common::Log log (LOG_NAME);

          log.Printf ("Attempt to create OpenAL device '%s'", name);

          return new sound::low_level::openal::OpenALDevice (DRIVER_NAME, name, init_string);
        }

      throw xtl::make_argument_exception (METHOD_NAME, "name", name);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  () { addref (this); }
    void Release () { release (this); }

  private:
    typedef stl::vector<stl::string> DeviceArray;

  private:
    DeviceArray devices;
};

/*
    ��������� �������� ������������ ����� OpenAL
*/

class OpenALComponent
{
  public:
    OpenALComponent ()
    {
      DriverManager::RegisterDriver (DRIVER_NAME, xtl::com_ptr<Driver> (new Driver, false).get ());
    }
};

extern "C"
{

common::ComponentRegistrator<OpenALComponent> OpenAL (COMPONENT_NAME);

}

}

}
