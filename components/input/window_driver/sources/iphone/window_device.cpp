#include <syslib/platform/iphone.h>

#include <shared/shared.h>

using namespace syslib::iphone;
using namespace input::low_level::window;

namespace
{

const size_t MESSAGE_BUFFER_SIZE = 64;
const double DEFAULT_ACCELEROMETER_UPDATE_INTERVAL = 1.f / 30.f;

const char* TOUCH_NAME             = "Touch";
const char* MULTITOUCH_BEGAN_EVENT = "MultiTouch began";
const char* MULTITOUCH_ENDED_EVENT = "MultiTouch ended";

const char* ACCELEROMETER_UPDATE_INTERVAL = "AccelerometerUpdateInterval";

const char* PROPERTIES [] = {
  ACCELEROMETER_UPDATE_INTERVAL
};

const size_t PROPERTIES_COUNT = sizeof (PROPERTIES) / sizeof (*PROPERTIES);

}

struct Device::Impl : public IWindowListener, public IApplicationListener
{
  typedef xtl::signal<void (const char*)> DeviceSignal;

  stl::string                  name;                          //��� ����������
  stl::string                  full_name;                     //������ ��� ����������
  stl::string                  properties;                    //���������
  DeviceSignal                 signals;                       //����������� �������
  stl::wstring                 control_name;                  //��� ��������
  xtl::auto_connection         window_destroy_connection;     //���������� �������� ����
  double                       accelerometer_update_interval; //������� ������� ������� �� �������������
  syslib::Window               *window;                       //����
  char                         message [MESSAGE_BUFFER_SIZE]; //���������

  ///����������� / ����������
  Impl (syslib::Window* in_window, const char* in_name)
    : name (in_name)
    , full_name ("window")
    , accelerometer_update_interval (DEFAULT_ACCELEROMETER_UPDATE_INTERVAL)
    , window (in_window)
  {
    try
    {
      window_destroy_connection = window->RegisterEventHandler (syslib::WindowEvent_OnDestroy, xtl::bind (&Device::Impl::OnWindowDestroy, this));

      for (size_t i = 0; i < PROPERTIES_COUNT; i++)
      {
        properties.append (PROPERTIES[i]);
        properties.append (" ");
      }

      if (!properties.empty ())
        properties.pop_back ();
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("input::low_level::window::Device::Device");
      throw;
    }

    if (!name.empty ())
    {
      full_name += '.';
      full_name += name;
    }

    attach_window_listener      (*window, this);
    attach_application_listener (this);

    set_accelerometer_update_interval (accelerometer_update_interval);
  }

  ~Impl ()
  {
    detach_application_listener (this);
    detach_window_listener      (*window, this);
  }

  ///������� �������
  void OnTouchesBegan (size_t touches_count, const TouchDescription* touches)
  {
    signals (MULTITOUCH_BEGAN_EVENT);

    for (size_t i = 0; i < touches_count; i++, touches++)
    {
      xtl::xsnprintf (message, MESSAGE_BUFFER_SIZE, "%s began %p %f %f", TOUCH_NAME, touches->touch,
                 touches->current_x, touches->current_y);
      signals (message);

      if (!(touches->tap_count % 2))
      {
        xtl::xsnprintf (message, MESSAGE_BUFFER_SIZE, "%s doubletap %p %f %f", TOUCH_NAME, touches->touch,
                   touches->current_x, touches->current_y);
        signals (message);
      }
    }

    signals (MULTITOUCH_ENDED_EVENT);
  }

  void OnTouchesMoved (size_t touches_count, const TouchDescription* touches)
  {
    signals (MULTITOUCH_BEGAN_EVENT);

    for (size_t i = 0; i < touches_count; i++, touches++)
    {
      xtl::xsnprintf (message, MESSAGE_BUFFER_SIZE, "%s moved %p %f %f %f %f", TOUCH_NAME, touches->touch,
                 touches->current_x, touches->current_y, touches->previous_x, touches->previous_y);
      signals (message);
    }

    signals (MULTITOUCH_ENDED_EVENT);
  }

  void OnTouchesEnded (size_t touches_count, const TouchDescription* touches)
  {
    signals (MULTITOUCH_BEGAN_EVENT);

    for (size_t i = 0; i < touches_count; i++, touches++)
    {
      xtl::xsnprintf (message, MESSAGE_BUFFER_SIZE, "%s ended %p %f %f %f %f", TOUCH_NAME, touches->touch,
                 touches->current_x, touches->current_y, touches->previous_x, touches->previous_y);
      signals (message);
    }

    signals (MULTITOUCH_ENDED_EVENT);
  }

  ///������� ��������
  void OnShakeMotionBegan ()
  {
    signals ("ShakeMotion began");
  }

  void OnShakeMotionEnded ()
  {
    signals ("ShakeMotion ended");
  }

  void OnWindowDestroy ()
  {
    detach_window_listener (*window, this);
  }

  ///������� ����������
  void OnAcceleration (double x, double y, double z)
  {
    xtl::xsnprintf (message, MESSAGE_BUFFER_SIZE, "Acceleration %f %f %f", x, y, z);
    signals (message);
  }

  void OnMemoryWarning ()
  {
    signals ("MemoryWarning");
  }

  void OnActive ()
  {
    signals ("Application active");
  }

  void OnInactive ()
  {
    signals ("Application inactive");
  }
};

/*
   �����������/����������
*/

Device::Device (syslib::Window* window, const char* name)
  : impl (new Impl (window, name))
{
}

Device::~Device ()
{
  delete impl;
}

/*
   ��������� ����� ��������
*/

const wchar_t* Device::GetControlName (const char* control_id)
{
  impl->control_name = common::towstring (control_id);

  return impl->control_name.c_str ();
}

/*
   �������� �� ������� ����������
*/

xtl::connection Device::RegisterEventHandler (const input::low_level::IDevice::EventHandler& handler)
{
  return impl->signals.connect (handler);
}

/*
   ��������� ����������
*/

const char* Device::GetProperties ()
{
  return impl->properties.c_str ();
}

void Device::SetProperty (const char* name, float value)
{
  if (!xtl::xstrcmp (ACCELEROMETER_UPDATE_INTERVAL, name))
  {
    impl->accelerometer_update_interval = value;

    set_accelerometer_update_interval (impl->accelerometer_update_interval);

    return;
  }

  throw xtl::make_argument_exception ("input::low_level::window::Device::SetProperty", "name", name);
}

float Device::GetProperty (const char* name)
{
  if (!xtl::xstrcmp (ACCELEROMETER_UPDATE_INTERVAL, name))
    return impl->accelerometer_update_interval;

  throw xtl::make_argument_exception ("input::low_level::window::Device::GetProperty", "name", name);
}

/*
   ��������� ����� ����������
*/

const char* Device::GetName ()
{
  return impl->name.c_str ();
}

/*
   ������ ��� ���������� (���.���.�������������)
*/

const char* Device::GetFullName ()
{
  return impl->full_name.c_str ();
}
