#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/bind.h>
#include <xtl/ref.h>

#include <common/var_registry.h>
#include <common/var_registry_container.h>
#include <common/strlib.h>

#include <syslib/window.h>
#include <syslib/application.h>

#include <render/low_level/driver.h>
#include <render/low_level/device.h>
#include <render/mid_level/low_level_driver.h>

#include "shared.h"

using namespace common;
using namespace render::low_level;

namespace
{

/*
    ���������
*/

const char* CONFIGURATION_FILE_NAME   = "media/configuration.xml"; //��� ����� ������������
const char* CONFIGURATION_BRANCH_NAME = "Demo"; //��� ����� ������� � �����������
const char* MID_LEVEL_RENDERER_NAME   = "MyRenderer"; //��� ������� ������������ �������� ������
const char* MATERIAL_LIB_FILE_NAME    = "media/materials.xmtl"; //��� ����� � �����������

const size_t DEFAULT_WINDOW_WIDTH  = 400;             //��������� ������ ����
const size_t DEFAULT_WINDOW_HEIGHT = 300;             //��������� ������ ����
const char*  DEFAULT_WINDOW_TITLE  = "Render2d test"; //��������� ����

const size_t DEFAULT_FB_COLOR_BITS    = 24; //������� ������ �����
const size_t DEFAULT_FB_ALPHA_BITS    = 8;  //������� �����-������
const size_t DEFAULT_FB_DEPTH_BITS    = 24; //������� z-buffer'�
const size_t DEFAULT_FB_STENCIL_BITS  = 8;  //������� ������ ���������
const size_t DEFAULT_FB_BUFFERS_COUNT = 2;  //���������� ������� � ������� ������
const size_t DEFAULT_FB_FULL_SCREEN_STATE = 0; //fullscreen �� ���������

const char* DEFAULT_DEVICE_INIT_STRING = ""; //������ ������������� ���������� ����������

/*
    �������
*/

template <class T> T get (VarRegistry& registry, const char* name, const T& default_value)
{
//  printf ("Read value of %s, branch name is %s\n", name, registry.BranchName ());

  if (!registry.HasVariable (name))
    return default_value;
    
//  printf ("Readed value of %s\n", name);

  try
  {
    return xtl::any_multicast<T> (registry.GetValue (name));
  }
  catch (...)
  {
    return default_value;
  }
}

stl::string get (VarRegistry& registry, const char* name, const char* default_value) //??????
{
  if (!registry.HasVariable (name))
    return default_value;

  return to_string (registry.GetValue (name));
}

void log_print (const char* message)
{
  printf ("%s\n", message);
}

}

/*
    �������� ���������� TestApplication
*/

typedef xtl::com_ptr<render::low_level::ISwapChain> SwapChainPtr;
typedef xtl::com_ptr<render::low_level::IDevice>    DevicePtr;
typedef VarRegistryContainer<stl::string>           StringRegistry;
typedef xtl::com_ptr<input::low_level::IDevice>     InputDevicePtr;

struct TestApplication::Impl
{
  StringRegistry                string_registry;     //����������� ������ �����  
  VarRegistry                   config;              //������ ���������������� ��������
  stl::auto_ptr<syslib::Window> window;              //������� ���� ����������
  SwapChainPtr                  swap_chain;          //������� ������ �������� ���� ����������
  DevicePtr                     device;              //���������� ���������� �������� ���� ����������
  xtl::auto_connection          app_idle_connection; //���������� ������� ����������� ��������� ���� ����������
  SceneRender                   render;              //������ �����
  InputDevicePtr                input_device;        //���������� �����
  
  void OnClose ()
  {
    syslib::Application::Exit (0);
  }

  void OnRedraw ()
  {
    try
    {
      static clock_t last     = 0;  
      static size_t  last_fps = 0, frames_count = 0;
    
      if (clock () - last_fps > CLK_TCK)
      {
        printf ("FPS: %.2f\n", float (frames_count)/float (clock () - last_fps)*float (CLK_TCK));

        last_fps     = clock ();
        frames_count = 0;
      }

      render.Draw ();
      
      frames_count++;
    }
    catch (std::exception& e)
    {
      printf (format ("Exception at window redraw: %s\n", e.what ()).c_str ());
    }
    catch (...)
    {
      printf ("Exception at window redraw\n");
    }
  }  
  
  void OnResize ()
  {
    try
    {
      syslib::Rect            client_rect = window->ClientRect ();
      render::low_level::Rect viewport;

      viewport.x      = client_rect.left;
      viewport.y      = client_rect.top;
      viewport.width  = client_rect.right - client_rect.left;
      viewport.height = client_rect.bottom - client_rect.top;

      mid_level::LowLevelDriver::SetViewport (MID_LEVEL_RENDERER_NAME, viewport);

      window->Invalidate ();
    }
    catch (std::exception& exception)
    {
      printf ("Eexception at window resize: %s\n", exception.what ());
    }
  }
};

/*
    ����������� / ����������
*/

TestApplication::TestApplication ()
  : impl (new Impl)
{
  try
  {
      //������������ �������
      
    impl->string_registry.Mount (CONFIGURATION_BRANCH_NAME);
    
      //������ ��������
      
    impl->config.Open (CONFIGURATION_BRANCH_NAME);

    load_xml_configuration (VarRegistry (""), CONFIGURATION_FILE_NAME);

      //�������� ����
      
    impl->window = new syslib::Window (get (impl->config, "FullScreen", DEFAULT_FB_FULL_SCREEN_STATE) ? 
        syslib::WindowStyle_PopUp : syslib::WindowStyle_Overlapped, get (impl->config, "WindowWidth", DEFAULT_WINDOW_WIDTH),
        get (impl->config, "WindowHeight", DEFAULT_WINDOW_HEIGHT));
        
    impl->window->SetDebugLog (&log_print);

    impl->window->SetTitle (get (impl->config, "WindowTitle", DEFAULT_WINDOW_TITLE).c_str ());    

    impl->window->Show ();
    
      //����������� ������������ ������� ����

    impl->window->RegisterEventHandler (syslib::WindowEvent_OnPaint, xtl::bind (&Impl::OnRedraw, &*impl));
    impl->window->RegisterEventHandler (syslib::WindowEvent_OnSize,  xtl::bind (&Impl::OnResize, &*impl));
    impl->window->RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&Impl::OnClose, &*impl));    

      //�������� ������� ������ � ���������� ����������

    SwapChainDesc desc;

    memset (&desc, 0, sizeof (desc));

    desc.frame_buffer.color_bits   = get (impl->config, "ColorBits", DEFAULT_FB_COLOR_BITS);
    desc.frame_buffer.alpha_bits   = get (impl->config, "AlphaBits", DEFAULT_FB_ALPHA_BITS);
    desc.frame_buffer.depth_bits   = get (impl->config, "DepthBits", DEFAULT_FB_DEPTH_BITS);
    desc.frame_buffer.stencil_bits = get (impl->config, "StencilBits", DEFAULT_FB_STENCIL_BITS);
    desc.buffers_count             = get (impl->config, "BuffersCount", DEFAULT_FB_BUFFERS_COUNT);
    desc.swap_method               = SwapMethod_Discard;
    desc.fullscreen                = get (impl->config, "FullScreen", DEFAULT_FB_FULL_SCREEN_STATE) != 0;
    desc.window_handle             = impl->window->Handle ();
    
    render::low_level::DriverManager::CreateSwapChainAndDevice ("*", desc, get (impl->config, "DeviceInitString", DEFAULT_DEVICE_INIT_STRING).c_str (),
      impl->swap_chain, impl->device);
      
      //�������� ������� ������������ �������� ������
      
    render::mid_level::LowLevelDriver::RegisterRenderer (MID_LEVEL_RENDERER_NAME, impl->device.get (), impl->swap_chain.get ());
    
      //������������� �������

    impl->render.SetLogHandler (&log_print);

    impl->render.SetRenderer (render::mid_level::LowLevelDriver::Name (), MID_LEVEL_RENDERER_NAME);
//    impl->render.SetRenderer ("Debug", "Renderer2d");

      //�������� ��������

    impl->render.LoadResource (MATERIAL_LIB_FILE_NAME);
    
      //��������� ��������� ������� ������
    
    impl->OnResize ();    

      //������������� ������� �����

    input::low_level::WindowDriver::RegisterDevice (*(impl->window.get ()));

    impl->input_device = InputDevicePtr (input::low_level::DriverManager::CreateDevice ("*", "*"), false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("TestApplication::TestApplication");
    throw;
  }
}

TestApplication::~TestApplication ()
{
}

/*
    ��������� �������� ����������
*/

SceneRender& TestApplication::Render ()
{
  return impl->render;
}

input::low_level::IDevice& TestApplication::InputDevice ()
{
  return *(impl->input_device);
}

/*
    ������ ����������
*/

int TestApplication::Run ()
{
  syslib::Application::Run ();

  return syslib::Application::GetExitCode ();
}

/*
    �����������
*/

void TestApplication::PostRedraw ()
{
  impl->window->Invalidate ();
}

/*
    ��������� idle-�������
*/

void TestApplication::SetIdleHandler (const IdleFunction& idle)
{
  impl->app_idle_connection = syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (idle, xtl::ref (*this)));
}
