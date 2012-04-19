#include "shared.h"

using namespace syslib;

namespace
{

/*
    ���������
*/

const float DEFAULT_SENSOR_MANAGER_UPDATE_RATE = 10.0f; //������� ���������� �������� �� ���������

}

/*
===================================================================================================
    Sensor
===================================================================================================
*/

namespace syslib
{

class SensorImpl: public ISensorEventListener, public xtl::reference_counter, public xtl::trackable
{
  public:
///�����������
    SensorImpl (size_t index)
      : handle ()
      , paused (false)
    {
      try
      {
        handle = Platform::CreateSensor (index);
        
        if (!handle)
          throw xtl::format_operation_exception ("", "Null sensor handle returned from Platform::CreateSensor");
          
        name   = Platform::GetSensorName (handle);
        type   = Platform::GetSensorType (handle);
        vendor = Platform::GetSensorVendor (handle);
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::SensorImpl::SensorImpl");
        throw;
      }
    }
    
///����������
    ~SensorImpl ()
    {
      try
      {
        mutex.Lock ();
        
        disconnect_all_trackers ();        
        
        Platform::DestroySensor (handle);
      }
      catch (...)
      {
      }
    }
    
///���������� �������
    sensor_t Handle () { return handle; }
    
///��� �������
    const char* Name () { return name.c_str (); }
    
///��� �������
    const char* Type () { return type.c_str (); }
    
///������������� �������
    const char* Vendor () { return vendor.c_str (); }
    
///����������� ������������ �������
    xtl::connection RegisterEventHandler (const Sensor::EventHandler& handler)
    {
      xtl::connection connection;
      
      try
      {
        Lock lock (mutex);
        
        Sensor::EventHandler wrapped_handler = EventHandlerWrapper (handler);
        
        connection = signal.connect (wrapped_handler);         
        
        connect_tracker (connection);
        
        if (signal.num_slots () == 1 && !paused)
        {
          Platform::StartSensorPolling (handle, *this);
        }
        
        wrapped_handler.target<EventHandlerWrapper> ()->impl = this;
        
        return connection;
      }
      catch (...)
      {
        connection.disconnect ();
        throw;
      }
    }
    
///����� �������
    void Poll ()
    {
      try
      {
        if (paused)
          return;
          
        Platform::PollSensorEvents (handle);

        while (!events.empty ())
        {                    
          SensorEvent event;
          
          {
            Lock lock (mutex);
          
            event = events.front ();          
          
            events.pop_front ();
          }

          signal (event);
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::SensorImpl::Poll");
        throw;
      }
    }
    
///������� �� ����� ����������
    void Pause ()
    {
      try
      {        
        Lock lock (mutex);
        
        if (paused)
          return;
          
        if (signal.num_slots () > 0)
          Platform::StopSensorPolling (handle);
          
        paused = true;
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::SensorImpl::Pause");
        throw;
      }
    }
    
///������� �� �������������� ����������
    void Resume ()
    {
      try
      {
        Lock lock (mutex);

        if (!paused)
          return;                
          
        if (signal.num_slots () > 0)
          Platform::StartSensorPolling (handle, *this);
          
        paused = false;
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::SensorImpl::Resume");
        throw;
      }      
    }

///��������� ������� �������
    void OnSensorChanged (const SensorEvent& event)
    {
      Lock lock (mutex);
      
      events.push_back (event);
    }

  private:
    typedef xtl::signal<Sensor::EventHandler::signature_type> Signal;
    typedef stl::deque<SensorEvent>                           EventQueue;
    
    struct EventHandlerWrapper
    {
      Sensor::EventHandler handler;
      SensorImpl*          impl;
      
      EventHandlerWrapper (const Sensor::EventHandler& in_handler)
        : handler (in_handler)
        , impl ()
      {
      }
      
      ~EventHandlerWrapper ()
      {
        try
        {
          if (impl)
            impl->CheckPolling ();
        }
        catch (...)
        {
        }
      }
      
      void operator () (SensorEvent& event) const
      {
        handler (event);
      }
    };
    
    void CheckPolling ()
    {
      Lock lock (mutex);      
      
      if (signal.num_slots () == 1 && !paused)
        Platform::StopSensorPolling (handle);
    }
  
  private:
    Mutex       mutex;  //������� ������� � �������
    sensor_t    handle; //���������� �������
    Signal      signal; //������ ������� �������
    bool        paused; //���� ����� �������� ������� �������
    stl::string name;   //��� �������
    stl::string type;   //��� �������
    stl::string vendor; //������������� �������
    EventQueue  events; //������� �������
};

}

namespace
{

typedef xtl::intrusive_ptr<SensorImpl> SensorPtr;

/// �������� ��������
class SensorManagerImpl: public xtl::trackable
{
  public:
///�����������
    SensorManagerImpl ()
      : paused (false)      
      , update_rate (0.0f)
    {
      try
      {
        sensors.resize (Platform::GetSensorsCount ());

        timer.reset (new Timer (xtl::bind (&SensorManagerImpl::PollSensors, this)));

        SetUpdateRate (DEFAULT_SENSOR_MANAGER_UPDATE_RATE);
        
        connect_tracker (Application::RegisterEventHandler (ApplicationEvent_OnPause, xtl::bind (&SensorManagerImpl::Pause, this)));
        connect_tracker (Application::RegisterEventHandler (ApplicationEvent_OnResume, xtl::bind (&SensorManagerImpl::Resume, this)));
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::SensorManagerImpl::SensorManagerImpl");
        throw;
      }
    }
    
///��������� �������
    SensorImpl& GetSensor (size_t index)
    {
      try
      {      
        if (index >= sensors.size ())
          throw xtl::make_range_exception ("", "index", index, sensors.size ());
          
        SensorPtr sensor = sensors [index];
        
        if (!sensor)
        {
          sensor = SensorPtr (new SensorImpl (index), false);   
          
          if (paused)
            sensor->Pause ();

          sensors [index] = sensor;
        }

        return *sensor;
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::SensorManagerImpl::GetSensor");
        throw;
      }
    }
    
///������� �� ����� ����������
    void Pause ()
    {
      try
      {
        if (paused)
          return;
        
        for (SensorArray::iterator iter=sensors.begin (), end=sensors.end (); iter!=end; ++iter)
          if (*iter)
            (*iter)->Pause ();
        
        paused = true;          
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::SensorManagerImpl::Pause");
        throw;
      }
    }
    
///������� �� �������������� ����������
    void Resume ()
    {
      try
      {
        if (!paused)
          return;
        
        for (SensorArray::iterator iter=sensors.begin (), end=sensors.end (); iter!=end; ++iter)
          if (*iter)
            (*iter)->Resume ();
        
        paused = false; 
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::SensorManagerImpl::Resume");
        throw;
      }
    }
    
///������� ������ �������� ��������
    void SetUpdateRate (float rate)
    {
      if (update_rate == rate)
        return;

      timer->SetPeriod (size_t (1000.0f / rate));
        
      update_rate = rate;
    }

    float UpdateRate ()
    {
      return update_rate;
    }

  private:
///���������� ��������
    void PollSensors ()
    {
      try
      {
        if (paused)
          return;
          
        for (SensorArray::iterator iter=sensors.begin (), end=sensors.end (); iter!=end; ++iter)
          if (*iter)
            (*iter)->Poll ();        
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::SensorManagerImpl::PollSensors");
        throw;
      }
    }

  private:
    typedef stl::vector<SensorPtr> SensorArray;    

  private:
    SensorArray          sensors;      //������ ��������
    bool                 paused;       //���� ����� ����������
    float                update_rate;  //������� ���������� ��������
    stl::auto_ptr<Timer> timer;        //������ ���������� ��������
};

typedef common::Singleton<SensorManagerImpl> SensorManagerSingleton;

}

/*
    ������������ / ���������� / ������������
*/

Sensor::Sensor (size_t sensor_index)  
{
  try
  {
    impl = &SensorManagerSingleton::Instance ()->GetSensor (sensor_index);
    
    addref (impl);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Sensor::Sensor(size_t)");
    throw;
  }
}

Sensor::Sensor (const Sensor& sensor)
  : impl (sensor.impl)
{
  addref (impl);
}

Sensor::~Sensor ()
{
  release (impl);
}

Sensor& Sensor::operator = (const Sensor& sensor)
{
  Sensor (sensor).Swap (*this);
  return *this;
}

/*
    ���
*/

const char* Sensor::Name () const
{
  return impl->Name ();
}

/*
    ��� ����������
*/

const char* Sensor::Type () const
{
  return impl->Type ();
}

/*
    �������������
*/

const char* Sensor::Vendor () const
{
  return impl->Vendor ();
}

/*
    ������������ ��������
*/

float Sensor::MaxRange () const
{
  try
  {
    return Platform::GetSensorMaxRange (impl->Handle ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Sensor::Name");
    throw;
  }
}

/*
    ������� ����������
*/

void Sensor::SetUpdateRate (float rate)
{
  try
  {
    Platform::SetSensorUpdateRate (impl->Handle (), rate);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Sensor::SetSensorUpdateRate");
    throw;
  }
}

float Sensor::UpdateRate () const
{
  try
  {
    return Platform::GetSensorUpdateRate (impl->Handle ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Sensor::GetSensorUpdateRate");
    throw;
  }
}

/*
    ���������-��������� ���������� �������
*/

const void* Sensor::Handle () const
{
  try
  {
    return Platform::GetNativeSensorHandle (impl->Handle ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Sensor::Handle");
    throw;
  }
}

/*
    ��������� ���������-��������� �������
*/

void Sensor::GetProperties (common::PropertyMap& properties) const
{
  try
  {
    properties.Clear ();
    
    return Platform::GetSensorProperties (impl->Handle (), properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Sensor::GetProperties");
    throw;
  }
}

/*
    ����������� ������� �������
*/

xtl::connection Sensor::RegisterEventHandler (const EventHandler& handler) const
{
  try
  {
    return impl->RegisterEventHandler (handler);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Sensor::RegisterEventHandler");
    throw;
  }
}

/*
    �����
*/

void Sensor::Swap (Sensor& sensor)
{
  stl::swap (impl, sensor.impl);
}
      
namespace syslib
{

void swap (Sensor& sensor1, Sensor& sensor2)
{
  sensor1.Swap (sensor2);
}

}

/*
    ���������
*/

bool Sensor::operator == (const Sensor& sensor) const
{
  return impl == sensor.impl;
}

bool Sensor::operator != (const Sensor& sensor) const
{
  return !(*this == sensor);
}

/*
===================================================================================================
    SensorManager
===================================================================================================
*/

/*
    ������������ ��������
*/

size_t SensorManager::SensorsCount ()
{
  try
  {
    return Platform::GetSensorsCount ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::SensorManager::SensorsCount");
    throw;
  }
}

syslib::Sensor SensorManager::Sensor (size_t index)
{
  try
  {
    return Sensor (index);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::SensorManager::Sensor");
    throw;
  }
}

/*
    ����� ������� �� ����
*/

int SensorManager::FindSensorByType (const char* type)
{
  try
  {
    if (!type)
      return -1;
    
    for (size_t i=0, count=Platform::GetSensorsCount (); i<count; i++)
      if (!strcmp (SensorManager::Sensor (i).Type (), type))
        return i;
        
    return -1;
  }
  catch (...)
  {
    return -1;
  }
}

/*
    ������� ������ �������� ��������
*/

void SensorManager::SetUpdateRate (float rate)
{
  SensorManagerSingleton::Instance ()->SetUpdateRate (rate);
}

float SensorManager::UpdateRate ()
{
  return SensorManagerSingleton::Instance ()->UpdateRate ();
}
