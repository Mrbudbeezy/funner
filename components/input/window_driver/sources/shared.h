#ifndef INPUT_LOW_LEVEL_WINDOW_DRIVER_SHARED_HEADER
#define INPUT_LOW_LEVEL_WINDOW_DRIVER_SHARED_HEADER

#include <cstdio>

#include <stl/algorithm>
#include <stl/vector>
#include <stl/string>
#include <stl/bitset>

#include <xtl/bind.h>
#include <xtl/connection.h>
#include <xtl/string.h>
#include <xtl/function.h>
#include <xtl/reference_counter.h>
#include <xtl/shared_ptr.h>
#include <xtl/trackable.h>
#include <xtl/common_exceptions.h>

#include <common/singleton.h>

#include <syslib/window.h>
#include <syslib/keydefs.h>

#include <input/low_level/driver.h>
#include <input/low_level/device.h>
#include <input/low_level/window_driver.h>

namespace input
{

namespace low_level
{

namespace window
{

class Device: virtual public input::low_level::IDevice, public xtl::reference_counter, private xtl::trackable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Device (syslib::Window* window, const char* name);
    ~Device ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetName () { return name.c_str (); }

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
    const char* GetProperties ();
    void        SetProperty   (const char* name, float value);
    float       GetProperty   (const char* name);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef () { addref (this); }  
    void Release () { release (this); }

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void WindowEventHandler (syslib::Window& window, syslib::WindowEvent event, const syslib::WindowEventContext& window_event_context);

  private:
    Device (const Device& source);             //no impl
    Device& operator = (const Device& source); //no impl

  private:
    stl::string                             name;                         //��� ����������
    stl::string                             properties;                   //���������
    input::low_level::IDevice::EventHandler event_handler;                //���������� �������
    size_t                                  x_cursor_pos;                 //��������� ���������� �������
    size_t                                  y_cursor_pos;                 //��������� ���������� �������
    bool                                    autocenter_cursor;            //�������������� ������������� �������
    float                                   cursor_sensitivity;           //��������� delt'� �������
    float                                   vertical_wheel_sensitivity;   //��������� delt'� ������������� ������ ����
    float                                   horisontal_wheel_sensitivity; //��������� delt'� ��������������� ������ ����
    stl::bitset<syslib::ScanCode_Num>       pressed_keys;                 //����� ������ �������� ��������
};

}

}

}

#endif
