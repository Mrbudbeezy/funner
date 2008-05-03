#include <stdio.h>
#include <string.h>

#include <xtl/bind.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/function.h>
#include <xtl/reference_counter.h>
#include <xtl/connection.h>

#include <common/exception.h>

#include <input/low_level/driver.h>
#include <input/low_level/device.h>
#include <input/events_source.h>

using namespace input;
using namespace input::low_level;
using namespace common;

const char* TEST_DEVICE_NAME = "TestInput";
const char* EVENTS [] = {"event1", "event2", "last_event"};

class TestInput;

TestInput* current_input = 0;

//��⮢�� ���ன�⢮ �����
class TestInput: virtual public IDevice, public xtl::reference_counter
{
  public:
    TestInput  () {current_input = this;}
    ~TestInput () {current_input = 0;}

///����祭�� ����� ���ன�⢠
    const char* GetName () { return TEST_DEVICE_NAME; }

///�����᪠ �� ᮡ��� ���ன�⢠
    void SetEventHandler (const EventHandler& handler)
    {
      event_handler = handler;
    }
    const EventHandler& GetEventHandler ()
    {
      return event_handler;
    }

///����ன�� ���ன�⢠
    const char* GetProperties () {return "";}
    void        SetProperty   (const char* name, float value) 
    {
      RaiseInvalidArgument ("TestInput::SetProperty", "name", name);
    }
    float GetProperty   (const char* name)
    {
      RaiseInvalidArgument ("TestInput::GetProperty", "name", name);
      return 0;
    }    

///����ன�� ���ன�⢠
    void GenerateTestEvents ()
    {
      for (size_t i = 0; i < sizeof (EVENTS) / sizeof (EVENTS[0]); i++)
        event_handler (EVENTS[i]);
    }
    
///������� ��뫮�
    void AddRef () { addref (this); }  
    void Release () { release (this); }

  private:
    float        dead_zone;
    float        saturation_zone;
    EventHandler event_handler;
};

//��⮢� �ࠩ���
class TestDriver: virtual public IDriver, public xtl::reference_counter
{
  public:
    TestDriver () {}

///���ᠭ�� �ࠩ���
    const char* GetDescription () { return "TestDriver"; }

///����᫥��� ����㯭�� ���ன�� �����
    size_t      GetDevicesCount ()
    {
      return 1;
    }
    const char* GetDeviceName (size_t index)
    {
      return TEST_DEVICE_NAME;
    }

///��������� ���ன�⢠ �����
    IDevice* CreateDevice (const char* name) 
    {
      return new TestInput;
    }

///��⠭���� �㭪樨 �⫠��筮�� ��⮪���஢���� �ࠩ���
    void SetDebugLog (const LogHandler& in_log)
    {
      log_fn = in_log;
    }
    
    const LogHandler& GetDebugLog ()
    {
      return log_fn;
    }
    
///������� ��뫮�
    void AddRef () { addref (this); }  
    void Release () { release (this); }

  private:
    LogHandler log_fn;
};

void my_event_handler (const char* event)
{
  printf ("New event: '%s'\n", event);
}

void my_print (const char* message)
{
  printf ("Error: %s\n", message);
}

int main ()
{
  printf ("Results of events_source1_test:\n");
  
  try
  {
    xtl::com_ptr<IDriver> driver (new TestDriver, false);
    
    DriverManager::RegisterDriver ("test_drv", get_pointer (driver));
    
    EventsSource events_source;
    
    printf ("Is connected: %c\n", events_source.IsConnected () ? 'y' : 'n');
    events_source.Connect ("*", "*", &my_print);
    printf ("Is connected: %c\n", events_source.IsConnected () ? 'y' : 'n');

    printf ("Driver name = '%s'\n", events_source.DriverName ());
    printf ("Device name = '%s'\n", events_source.DeviceName ());
  
    events_source.RegisterHandler (&my_event_handler);

    events_source.Disconnect ();
    printf ("Is connected: %c\n", events_source.IsConnected () ? 'y' : 'n');

    events_source.Connect ("*", "*");

    if (current_input)
      current_input->GenerateTestEvents ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
