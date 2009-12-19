#ifndef RENDER_MID_LEVEL_LOW_LEVEL_DRIVER_TESTS_SHARED_HEADER
#define RENDER_MID_LEVEL_LOW_LEVEL_DRIVER_TESTS_SHARED_HEADER

#include <cstdio>
#include <cstring>

#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/exception.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/ref.h>
#include <xtl/shared_ptr.h>
#include <xtl/trackable.h>

#include <math/utility.h>

#include <common/component.h>
#include <common/file.h>
#include <common/log.h>
#include <common/time.h>
#include <common/parser.h>

#include <media/compressed_image.h>
#include <media/image.h>

#include <syslib/application.h>
#include <syslib/window.h>

#include <render/low_level/device.h>
#include <render/low_level/driver.h>

#include <render/mid_level/driver.h>
#include <render/mid_level/window_driver.h>
#include <render/mid_level/renderer2d.h>

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace math;

/*
    ���������
*/

const size_t WINDOW_WIDTH              = 1024;                        //������ ����
const size_t WINDOW_HEIGHT             = 768;                         //������ ����
const char*  CONFIGURATION_FILE_NAME   = "data/configuration.xml";

const math::vec4f CLEAR_COLOR (0.7f, 0.f, 0.f, 0.f);

/*
    ��������������� �����
*/

typedef xtl::com_ptr<render::low_level::ISwapChain>   SwapChainPtr;
typedef xtl::com_ptr<render::low_level::IDevice>      DevicePtr;
typedef xtl::com_ptr<render::mid_level::IFrameBuffer> FrameBufferPtr;
typedef xtl::com_ptr<render::mid_level::IRenderer>    RendererPtr;
typedef xtl::com_ptr<renderer2d::IRenderer>           Renderer2dPtr;
typedef xtl::com_ptr<IClearFrame>                     ClearFramePtr;
typedef xtl::com_ptr<renderer2d::IFrame>              FramePtr;
typedef xtl::com_ptr<ITexture>                        TexturePtr;
typedef xtl::com_ptr<IPrimitive>                      PrimitivePtr;

/*
   ��������� ������� ������� ����������
*/

class RendererListener : public IRendererListener
{
  public:
    void OnFrameBufferCreate (IFrameBuffer* frame_buffer)
    {
      printf ("Created frame buffer %p\n", frame_buffer);
    }

    void OnFrameBufferDestroy (IFrameBuffer* frame_buffer)
    {
      printf ("Destroyed frame buffer %p\n", frame_buffer);
    }

    void OnFrameBufferUpdate (IFrameBuffer* frame_buffer)
    {
      //printf ("Updated frame buffer %p\n", frame_buffer);
    }

    void OnFrameBufferResize (IFrameBuffer* frame_buffer, size_t width, size_t height)
    {
      printf ("Changed size of frame buffer %p to %ux%u\n", frame_buffer, width, height);
    }
};

/*
   ���� ����������
*/

class RenderWindow : public IRendererListener, public xtl::trackable
{
  public:
    RenderWindow (const wchar_t* title, common::ParseNode& cfg_root)
      : window (syslib::WindowStyle_Overlapped, WINDOW_WIDTH, WINDOW_HEIGHT), style (0)
    {
        //������������� ����

      window.SetTitle (title);
      window.Show ();

      WindowDriver::RegisterWindow ("MyRenderer", window, cfg_root);

      RendererPtr basic_renderer (DriverManager::CreateRenderer (WindowDriver::Name (), "MyRenderer"), false);

      renderer = Renderer2dPtr (dynamic_cast<renderer2d::IRenderer*> (basic_renderer.get ()));

      if (!renderer)
        throw xtl::format_operation_exception ("RenderWindow::RenderWindow", "No renderer2d");

      renderer->AttachListener (this);

      OnFrameBufferCreate (renderer->GetFrameBuffer (renderer->GetFrameBuffersCount () - 1));

        //����������� ������������ ������� ����

      window.RegisterEventHandler (syslib::WindowEvent_OnPaint, xtl::bind (&RenderWindow::OnRedraw, this));
      window.RegisterEventHandler (syslib::WindowEvent_OnSize,  xtl::bind (&RenderWindow::OnResize, this));
//      window.RegisterEventHandler (syslib::WindowEvent_OnChangeHandle, xtl::bind (&RenderWindow::OnChangeHandle, this));
      window.RegisterEventHandler (syslib::WindowEvent_OnDestroy, xtl::bind (&RenderWindow::OnDestroy, this));
      window.RegisterEventHandler (syslib::WindowEvent_OnLeftButtonDown, xtl::bind (&RenderWindow::ChangeWindowStyle, this));

        //��������� �������� ������� ������

      OnResize ();
    }

    ~RenderWindow ()
    {
      renderer->DetachListener (this);
    }

    void OnFrameBufferCreate (IFrameBuffer* new_frame_buffer)
    {
      if (frame_buffer)
        return;

      frame_buffer = new_frame_buffer;

        //�������� ���������� �����

      clear_frame = ClearFramePtr (renderer->CreateClearFrame (), false);

      clear_frame->SetRenderTargets (frame_buffer->GetColorBuffer (), frame_buffer->GetDepthStencilBuffer ());
      clear_frame->SetFlags         (render::mid_level::ClearFlag_All);
      clear_frame->SetColor         (CLEAR_COLOR);

        //�������� �������� �����

      frame = FramePtr (renderer->CreateFrame (), false);

      frame->SetRenderTargets (frame_buffer->GetColorBuffer (), frame_buffer->GetDepthStencilBuffer ());
      frame->SetProjection    (GetOrthoProjectionMatrix (-100, 100, -100, 100, -1000, 1000));
    }

    void OnFrameBufferDestroy (IFrameBuffer* destroyed_frame_buffer)
    {
      if (frame_buffer.get () == destroyed_frame_buffer)
        frame_buffer = 0;
    }

    void OnFrameBufferUpdate (IFrameBuffer* updated_frame_buffer)
    {
      if (frame_buffer.get () == updated_frame_buffer)
        OnRedraw ();
    }

    void OnFrameBufferResize (IFrameBuffer* resized_frame_buffer, size_t width, size_t height)
    {
      if (frame_buffer.get () == resized_frame_buffer)
        OnResize ();
    }

    renderer2d::IRenderer* Renderer ()
    {
      return renderer.get ();
    }

    renderer2d::IFrame* Frame ()
    {
      return frame.get ();
    }

    void AddPrimitive (IPrimitive* primitive)
    {
      frame->AddPrimitive (primitive);
    }

///���������� ������� ����������� ����
    void OnRedraw ()
    {
      Redraw ();
    }

    syslib::Window& Window ()
    {
      return window;
    }

  private:
    void ChangeWindowStyle ()
    {
      window.SetStyle ((syslib::WindowStyle)(style++ % 2));
    }

///�����������
    void Redraw ()
    {
      try
      {
        renderer->AddFrame   (clear_frame.get ());
        renderer->AddFrame   (frame.get ());

        renderer->DrawFrames ();
      }
      catch (std::exception& e)
      {
        printf ("redraw exception: %s\n", e.what ());
      }
    }

///���������� ������� ��������� �������� ����
    void OnResize ()
    {
      try
      {
        syslib::Rect client_rect = window.ClientRect ();

        render::mid_level::Viewport viewport;

        viewport.x      = client_rect.left;
        viewport.y      = client_rect.top;
        viewport.width  = client_rect.right - client_rect.left;
        viewport.height = client_rect.bottom - client_rect.top;

        frame->SetViewport (viewport);

        window.Invalidate ();
      }
      catch (std::exception& e)
      {
        printf ("resize exception: %s\n", e.what ());
      }
    }

///���������� ������� �������� ����
    void OnDestroy ()
    {
      frame_buffer = 0;
    }

///��������� ��������������� ������� ��������
    static math::mat4f GetOrthoProjectionMatrix (float left, float right, float bottom, float top, float znear, float zfar)
    {
      math::mat4f proj_matrix;

      float width  = right - left,
            height = top - bottom,
            depth  = zfar - znear;

        //������� ������� �������������, ������������ gluOrtho2D

      proj_matrix [0] = math::vec4f (2.0f / width, 0, 0, - (right + left) / width);
      proj_matrix [1] = math::vec4f (0, 2.0f / height, 0, - (top + bottom) / height);
      proj_matrix [2] = math::vec4f (0, 0, 2.0f / depth, - (znear + zfar) / depth);
      proj_matrix [3] = math::vec4f (0, 0, 0, 1);

      return proj_matrix;
    }

  private:
    syslib::Window      window;
    Renderer2dPtr       renderer;
    FrameBufferPtr      frame_buffer;
    ClearFramePtr       clear_frame;
    FramePtr            frame;
    size_t              style;
};

/*
    �������� ����������
*/

class BasicTest
{
  public:
///�����������
    BasicTest (const wchar_t* title) : log_filter ("*", &LogMessage), cfg_parser (CONFIGURATION_FILE_NAME)
    {
      WindowDriver::RegisterRenderer ("MyRenderer", cfg_parser.Root ().First ("Configuration"));

      AddWindow ();

      render_windows [0]->Renderer ()->AttachListener (&renderer_listener);
    }

    virtual ~BasicTest () {}

///��������� �������� ������������
    renderer2d::IRenderer* Renderer ()
    {
      if (render_windows.empty ())
        return 0;

      return render_windows [0]->Renderer ();
    }

    renderer2d::IFrame* Frame ()
    {
      if (render_windows.empty ())
        return 0;

      return render_windows [0]->Frame ();
    }

///�������� ��������
    TexturePtr LoadTexture (const char* file_name)
    {
      if (!file_name)
        throw xtl::make_null_argument_exception ("BasicTest::LoadTexture", "file_name");

      if (render_windows.empty ())
        throw xtl::format_operation_exception ("BasicTest::LoadTexture", "No windows");
        
      static common::ComponentLoader loader ("media.compressed_image.*");
        
      if (media::CompressedImageManager::FindLoader (file_name, common::SerializerFindMode_ByName) != 0)
      {
        media::CompressedImage image (file_name);

        return TexturePtr (render_windows[0]->Renderer ()->CreateTexture (image), false);        
      }
      else
      {
        media::Image image (file_name, media::PixelFormat_RGBA8);

        return TexturePtr (render_windows[0]->Renderer ()->CreateTexture (image), false);      
      }      
    }

///���������� �����
    int Run ()
    {
      idle_connection = syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&BasicTest::OnIdleWrapper, this));

      syslib::Application::Run ();

      return syslib::Application::GetExitCode ();
    }

    void AddWindow ()
    {
      common::ParseNode cfg_root = cfg_parser.Root ().First ("Configuration");

      render_windows.push_back (new RenderWindow (L"Additional window", cfg_root));

      render_windows.back ()->Window ().RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&BasicTest::OnClose, this, _1));
    }

    void AddPrimitive (IPrimitive* primitive)
    {
      for (size_t i = 0; i < render_windows.size (); i++)
        render_windows[i]->AddPrimitive (primitive);
    }

  private:
///���������� ������� �������� ����� ����������
    virtual void OnIdle () {}

///���������� ������� �������� �������� ����
    void OnClose (syslib::Window& closed_window)
    {
      for (size_t i = 0; i < render_windows.size (); i++)
        if (&(render_windows[i]->Window ()) == &closed_window)
        {
          render_windows.erase (render_windows.begin () + i);
          break;
        }

      if (render_windows.empty ())
      {
        idle_connection.disconnect ();
        syslib::Application::Exit (0);
      }
    }

///������ ��� ������������ ������� �������� ����� ����������
    void OnIdleWrapper ()
    {
      try
      {
        static size_t last_fps = 0, frames_count = 0;

          //����� ���������� ������ � �������

        if (common::milliseconds () - last_fps > 1000.f)
        {
          printf ("FPS: %.2f\n", float (frames_count)/float (common::milliseconds () - last_fps) * 1000.f);

          last_fps     = common::milliseconds ();
          frames_count = 0;
        }

          //��������� ������

        for (size_t i = 0; i < render_windows.size (); i++)
          render_windows[i]->Window ().Invalidate ();

          //���������� ����� ������

        frames_count++;

        OnIdle ();
      }
      catch (std::exception& exception)
      {
        printf ("exception at idle: %s\n", exception.what ());
      }
    }

  private:
///����������������
    static void LogMessage (const char* name, const char* message)
    {
      printf ("%s: %s\n", name, message);
    }

  private:
    typedef xtl::shared_ptr<RenderWindow> RenderWindowPtr;

  private:
    common::LogFilter          log_filter;
    common::Parser             cfg_parser;
    stl::vector<RenderWindow*> render_windows;
    RendererListener           renderer_listener;
    xtl::connection            idle_connection;
};

//��������� ���������� ����� � ��������� [min_value;max_value]
float frand (float min_value=0.0f, float max_value=1.0f)
{
  return min_value + float (rand ()) / RAND_MAX * (max_value - min_value);
}

#endif
