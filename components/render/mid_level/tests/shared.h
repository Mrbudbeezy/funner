#ifndef RENDER_MID_LEVEL_TESTS_SHARED_HEADER
#define RENDER_MID_LEVEL_TESTS_SHARED_HEADER

#include <cstdio>
#include <exception>

#include <xtl/bind.h>
#include <xtl/function.h>
#include <xtl/trackable.h>

#include <render/mid_level/manager.h>

#include <media/image.h>

#include <syslib/application.h>
#include <syslib/window.h>

using namespace common;
using namespace render::mid_level;

//���������
const size_t WINDOW_WIDTH  = 800;
const size_t WINDOW_HEIGHT = 600;

///�������� ����������
class Test: private xtl::trackable
{
  public:
///�����������
    Test (const wchar_t* title)
      : window (syslib::WindowStyle_Overlapped, WINDOW_WIDTH, WINDOW_HEIGHT)
      , render_window (CreateRenderWindow ())
    {
      window.SetTitle (title);
      window.Show ();
      
      connect_tracker (window.RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&Test::OnWindowClose, this)));
    }
    
///��������� ��������� ����������
    render::mid_level::RenderManager RenderManager ()
    {
      return render_manager;
    }
    
///������� ����
    int Run ()
    {
      syslib::Application::Run ();

      return syslib::Application::GetExitCode ();
    }
    
///���� ��������� ����� ����
    void TestChangeWindowStyle ()
    {
      window.SetStyle (syslib::WindowStyle_PopUp);
      window.SetStyle (syslib::WindowStyle_Overlapped);
    }
    
  private:
///�������� ���� ����������
    Window CreateRenderWindow ()
    {
      connect_tracker (render_manager.RegisterWindowEventHandler (RenderManagerWindowEvent_OnAdded, xtl::bind (&Test::OnWindowAdded, this, _2)));
      connect_tracker (render_manager.RegisterWindowEventHandler (RenderManagerWindowEvent_OnRemoved, xtl::bind (&Test::OnWindowRemoved, this, _2)));

      PropertyMap properties;
      
      properties.SetProperty ("ColorBits", 24);
      properties.SetProperty ("DepthBits", 16);
    
      return render_manager.CreateWindow (window, properties);
    }
    
///��������� ������� ���������� ����
    void OnWindowAdded (Window& window)
    {
      printf ("window %ux%u added\n", window.Width (), window.Height ());
      
      connect_tracker (window.RegisterEventHandler (WindowEvent_OnResize, xtl::bind (&Test::OnWindowResize, this, _1)));
    }
    
///��������� ������� �������� ����
    void OnWindowRemoved (Window& window)
    {
      printf ("window %ux%u removed\n", window.Width (), window.Height ());
    }
    
///��������� ������� ��������� �������� ����
    void OnWindowResize (Window& window)
    {
      printf ("window resize %ux%u\n", window.Width (), window.Height ());
    }
    
///��������� ������� �������� ����
    void OnWindowClose ()
    {
      syslib::Application::Exit (0);
    }
    
///���������
  
  private:
    syslib::Window                    window;
    render::mid_level::RenderManager  render_manager;
    Window                            render_window;
};

#endif
