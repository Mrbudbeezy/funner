#include "shared.h"

using namespace xtl;
using namespace input;
using namespace input::low_level;
using namespace common;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct EventsSource::Impl : public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Impl  ();
    ~Impl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool        IsConnected () const;
    const char* DriverName  () const;
    const char* DeviceName  () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� � ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Connect    (const char* driver_name_mask, const char* device_name_mask, const xtl::function<void (const char*)>& log_function);
    void Disconnect ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    connection RegisterHandler (const EventHandler&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� �� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void DeviceEventHandler (const char* event);
  
  private:
    typedef signal<void (const char*)> DeviceSignal;
    typedef com_ptr<IDevice>           DevicePtr;

    DeviceSignal signals;           //������� ����������
    DevicePtr    connected_device;  //������������ ����������
    stl::string  driver_name;       //��� ��������
};

/*
   ������������� / ���������� / ������������
*/

EventsSource::Impl::Impl () {}

EventsSource::Impl::~Impl ()
{
}

/*
   ���������� � �����������
*/

bool EventsSource::Impl::IsConnected () const
{
  return connected_device;
}

const char* EventsSource::Impl::DriverName () const
{
  return driver_name.c_str ();
}

const char* EventsSource::Impl::DeviceName () const
{
  return connected_device ? connected_device->GetName () : "";
}

/*
   ����������� � ���������� �����
*/

void EventsSource::Impl::Connect (const char* driver_name_mask, const char* device_name_mask, const xtl::function<void (const char*)>& log_function)
{
  for (size_t i = 0; i < DriverManager::DriversCount (); i++)
    if (wcimatch (DriverManager::DriverName (i), driver_name_mask))
    {
      xtl::com_ptr<IDriver> driver = DriverManager::Driver (i);

      for (size_t j = 0; j < driver->GetDevicesCount (); j++)
        if (wcimatch (driver->GetDeviceName (j), device_name_mask))
        {
          try
          {
            DevicePtr new_connected_device (driver->CreateDevice (driver->GetDeviceName (j)), false);

            new_connected_device->SetEventHandler (xtl::bind (&EventsSource::Impl::DeviceEventHandler, this, _1));

            driver_name      = DriverManager::DriverName (i);
            connected_device = new_connected_device;

            return;
          }
          catch (std::exception& exception)
          {
            log_function (exception.what ());
          }
          catch (...)
          {
            log_function ("Unknown exception caught when connecting to device");
          }
        }
    }

  raise<Exception> ("input::EventSource::Impl::Connect", "Can't connect to any device with driver mask '%s' and device mask '%s'", driver_name_mask, device_name_mask);
}

void EventsSource::Impl::Disconnect ()
{
  connected_device = 0;
  driver_name.clear ();
}

/*
   �������� ������� �����
*/

xtl::connection EventsSource::Impl::RegisterHandler (const EventHandler& handler)
{
  return signals.connect (handler);
}

/*
   ���������� ��������� �� ����������
*/

void EventsSource::Impl::DeviceEventHandler (const char* event)
{
  try
  {
    signals (event);
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}  

/*
   ������������� / ���������� / ������������
*/

EventsSource::EventsSource ()
  : impl (new Impl)
  {}

EventsSource::EventsSource (const char* driver_name_mask, const char* device_name_mask)
  : impl (new Impl)
{
  Connect (driver_name_mask, device_name_mask);
}

EventsSource::EventsSource (const EventsSource& source)
  : impl (source.impl)
{
  addref (impl);
}

EventsSource::~EventsSource ()
{
  release (impl);
}

EventsSource& EventsSource::operator = (const EventsSource& source)
{
  EventsSource (source).Swap (*this);

  return *this;
}

/*
   ���������� � �����������
*/

bool EventsSource::IsConnected () const
{
  return impl->IsConnected ();
}

const char* EventsSource::DriverName () const
{
  return impl->DriverName ();
}

const char* EventsSource::DeviceName () const
{
  return impl->DeviceName ();
}

/*
   ����������� � ���������� �����
*/

namespace
{

void dummy_print (const char*)
{
}

}

void EventsSource::Connect (const char* driver_name_mask, const char* device_name_mask)
{
  Connect (driver_name_mask, device_name_mask, &dummy_print);
}

void EventsSource::Connect (const char* driver_name_mask, const char* device_name_mask, const xtl::function<void (const char*)>& log_function)
{
  if (!driver_name_mask)
    driver_name_mask = "*";

  if (!device_name_mask)
    device_name_mask = "*";

  impl->Connect (driver_name_mask, device_name_mask, log_function ? log_function : &dummy_print);
}

void EventsSource::Disconnect ()
{
  impl->Disconnect ();
}

/*
   �������� ������� �����
*/

xtl::connection EventsSource::RegisterHandler (const EventHandler& handler) const
{
  return impl->RegisterHandler (handler);
}

/*
   �����
*/

void EventsSource::Swap (EventsSource& source)
{
  stl::swap (impl, source.impl);
}

namespace input
{

/*
   �����
*/

void swap (EventsSource& source1, EventsSource& source2)
{
  source1.Swap (source2);
}

}
