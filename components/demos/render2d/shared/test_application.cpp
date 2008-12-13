#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/bind.h>
#include <xtl/ref.h>
#include <xtl/any.h>

#include <common/var_registry.h>
#include <common/strlib.h>

#include <syslib/window.h>
#include <syslib/application.h>

#include <render/mid_level/window_driver.h>

#include "shared.h"

using namespace common;

namespace
{

/*
    ���������
*/

const char* CONFIGURATION_BRANCH_NAME = "Configuration.WindowSettings"; //��� ����� ������� � �����������
const char* CONFIGURATION_FILE_NAME   = "media/config.xml"; //��� ����� ������������
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

struct TestApplication::Impl
{
  VarRegistry                    config;              //������ ���������������� ��������
  stl::auto_ptr<syslib::Window>  window;              //������� ���� ����������
  xtl::auto_connection           app_idle_connection; //���������� ������� ����������� ��������� ���� ����������
  SceneRender                    render;              //������ �����
  render::RenderTarget           render_target;       //���� ����������

  void OnClose ()
  {
    syslib::Application::Exit (0);

    render.ResetRenderer ();

    app_idle_connection.disconnect ();
  }

  void OnRedraw ()
  {
    try
    {
      static size_t  last_fps = 0, frames_count = 0;

      if (common::milliseconds () - last_fps > 1000)
      {
        printf ("FPS: %.2f\n", float (frames_count) / float (milliseconds () - last_fps) * 1000.f);

        last_fps     = common::milliseconds ();
        frames_count = 0;
      }

      render_target.Update ();

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
};

/*
    ����������� / ����������
*/

TestApplication::TestApplication ()
  : impl (new Impl)
{
  try
  {
      //������ ��������

    impl->config.Open (CONFIGURATION_BRANCH_NAME);

      //�������� ����

    impl->window = new syslib::Window (get (impl->config, "FullScreen", DEFAULT_FB_FULL_SCREEN_STATE) ?
        syslib::WindowStyle_PopUp : syslib::WindowStyle_Overlapped, get (impl->config, "WindowWidth", DEFAULT_WINDOW_WIDTH),
        get (impl->config, "WindowHeight", DEFAULT_WINDOW_HEIGHT));

    impl->window->SetDebugLog (&log_print);

    impl->window->SetTitle (get (impl->config, "WindowTitle", DEFAULT_WINDOW_TITLE).c_str ());

    impl->window->Show ();

      //����������� ������������ ������� ����

    impl->window->RegisterEventHandler (syslib::WindowEvent_OnPaint, xtl::bind (&Impl::OnRedraw, &*impl));
    impl->window->RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&Impl::OnClose, &*impl));

      //������������� ������� ����������

    common::Parser cfg_parser (CONFIGURATION_FILE_NAME);
    common::ParseNode cfg_root = cfg_parser.Root ().First ("Configuration");

    render::mid_level::WindowDriver::RegisterRenderer (MID_LEVEL_RENDERER_NAME, cfg_root);
    render::mid_level::WindowDriver::RegisterWindow (MID_LEVEL_RENDERER_NAME, *impl->window, cfg_root);

      //������������� �������

    impl->render.SetLogHandler (&log_print);
    impl->render.SetRenderer   (render::mid_level::WindowDriver::Name (), MID_LEVEL_RENDERER_NAME);
//    impl->render.SetRenderer ("Debug", "Renderer2d");

    impl->render_target = impl->render.RenderTarget (0);

      //�������� ��������

    impl->render.LoadResource (MATERIAL_LIB_FILE_NAME);
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

/*
    ��������� ���� ����������
*/

RenderTarget& TestApplication::RenderTarget ()
{
  return impl->render_target;
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
