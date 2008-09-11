#include "shared.h"

using namespace render::low_level;
using namespace common;

namespace
{

/*
    ��������� (������� � ������������)!!!
*/

const char* CONFIG_FILE_NAME = "media/configuration.xml"; //��� ����� ������������

const char* DEFAULT_LOG_FILE_NAME = "water-slides.log"; //��� ����� ���������

const size_t DEFAULT_WINDOW_WIDTH  = 400;                         //��������� ������ ����
const size_t DEFAULT_WINDOW_HEIGHT = 300;                         //��������� ������ ����
const char*  DEFAULT_WINDOW_TITLE  = "Water slides demo ($Id$)"; //��������� ����

const char*  WATER_DROP_SOUND_NAME   = "media/drop.snddecl";      //��� ����� ���������� � ������ �����

const size_t DEFAULT_FB_COLOR_BITS    = 24; //������� ������ �����
const size_t DEFAULT_FB_ALPHA_BITS    = 8;  //������� �����-������
const size_t DEFAULT_FB_DEPTH_BITS    = 24; //������� z-buffer'�
const size_t DEFAULT_FB_STENCIL_BITS  = 8;  //������� ������ ���������
const size_t DEFAULT_FB_BUFFERS_COUNT = 2;  //���������� ������� � ������� ������
const size_t DEFAULT_FB_FULL_SCREEN_STATE = 0; //fullscreen �� ���������

const char* DEFAULT_DEVICE_INIT_STRING = ""; //������ ������������� ���������� ����������

}

/*
    �������� ���������� ����������
*/

struct MyApplication::Impl
{
  public:
      //�����������
    Impl () :
      configuration (CONFIG_FILE_NAME),
      log_file (configuration.GetString ("LogFileName", DEFAULT_LOG_FILE_NAME)),
      log_stream (xtl::bind (&Impl::LogWriteBuffer, this, _1, _2)),
      window (configuration.GetInteger ("FullScreen", DEFAULT_FB_FULL_SCREEN_STATE) ? 
        syslib::WindowStyle_PopUp : syslib::WindowStyle_Overlapped, configuration.GetInteger ("WindowWidth", DEFAULT_WINDOW_WIDTH), configuration.GetInteger ("WindowHeight", DEFAULT_WINDOW_HEIGHT)),
      sound_manager (0),
      scene_player (0)
    {
      try
      {        
        sound_manager = new sound::SoundManager ("*", "*");

        LogMessage ("Loading sound declarations...");
        sound_manager->LoadSoundLibrary (WATER_DROP_SOUND_NAME);
      }
      catch (...)
      {
        delete sound_manager;
        sound_manager = 0;        

        LogMessage ("Can't create sound manager, exception caught");
      }      

      if (sound_manager)
      {
        try
        {
          scene_player = new sound::ScenePlayer;
          
          scene_player->SetManager (sound_manager);
        }
        catch (...)
        {
          delete sound_manager;
          sound_manager = 0;
          LogMessage ("Can't create scene player, exception caught");
        }
      }

      LogMessage ("Create window...");
      
        //��������� ���������������� ������� ����������� ����������������

      window.SetDebugLog (xtl::bind (&Impl::LogMessage, this, _1));
      
        //��������� ��������� ����
      
      window.SetTitle (configuration.GetString ("WindowTitle", DEFAULT_WINDOW_TITLE));
      
        //����� ����

      window.Show ();
      
      LogMessage ("Initializing rendering device...");

        //�������� ������� ������ � ���������� ����������

      SwapChainDesc desc;

      memset (&desc, 0, sizeof (desc));

      desc.frame_buffer.color_bits   = configuration.GetInteger ("ColorBits", DEFAULT_FB_COLOR_BITS);
      desc.frame_buffer.alpha_bits   = configuration.GetInteger ("AlphaBits", DEFAULT_FB_ALPHA_BITS);
      desc.frame_buffer.depth_bits   = configuration.GetInteger ("DepthBits", DEFAULT_FB_DEPTH_BITS);
      desc.frame_buffer.stencil_bits = configuration.GetInteger ("StencilBits", DEFAULT_FB_STENCIL_BITS);
      desc.buffers_count             = configuration.GetInteger ("BuffersCount", DEFAULT_FB_BUFFERS_COUNT);
      desc.swap_method               = SwapMethod_Discard;
      desc.fullscreen                = configuration.GetInteger ("FullScreen", DEFAULT_FB_FULL_SCREEN_STATE) != 0;
      desc.window_handle             = window.Handle ();
      
      render::low_level::DriverManager::CreateSwapChainAndDevice ("*", "*", desc, configuration.GetString ("DeviceInitString", DEFAULT_DEVICE_INIT_STRING),
        swap_chain, device);
      
        //���������� �� ��������� ��������
      
      OnResize ();

        //����������� ������������ ������� ����

      window.RegisterEventHandler (syslib::WindowEvent_OnPaint, xtl::bind (&Impl::OnRedraw, this));
      window.RegisterEventHandler (syslib::WindowEvent_OnSize, xtl::bind (&Impl::OnResize, this));
      window.RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&Impl::OnClose, this));
      
      for (int i=syslib::WindowEvent_OnLeftButtonDown; i<=syslib::WindowEvent_OnMiddleButtonDoubleClick; i++)
        window.RegisterEventHandler ((syslib::WindowEvent)i, xtl::bind (&Impl::OnMouse, this, _2, _3));
        
      window.RegisterEventHandler (syslib::WindowEvent_OnMouseMove, xtl::bind (&Impl::OnMouse, this, _2, _3));        

        //����������� ������������ ������� ����������
        
      app_idle_connection = syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&Impl::OnIdle, this));      
    }
    
      //����������
    ~Impl ()
    {
      try
      {
        delete sound_manager;
        delete scene_player;

        LogMessage ("Close application");        
        
        SetView (GameView ());                

        LogMessage ("Destroy rendering device...");
        
        device = 0;
        swap_chain = 0;
        
        LogMessage ("Close window...");
        
        window.Close ();
      }
      catch (...)
      {
        //���������� ���� ����������
      }
    }
    
      //��������������
    void LogMessage (const char* message)
    {
      if (!message)
        return;

          //������������ �������� ����������������

      time_t ltime;

      time (&ltime);

      stl::string prefix = common::format ("[%s", ctime (&ltime));
      
      prefix.pop_back ();

      prefix += common::format (" : %03u] ", MyApplication::Milliseconds () % 1000);

        //������������ ������ ����� ��� ������ � ��������

      stl::vector<stl::string> lines = split (message, "\n");
      
      for (size_t i=0; i<lines.size (); i++)
      {
        stl::string& line = lines [i];

        if (line.empty ())
          continue;

        write (log_stream, prefix);
        write (log_stream, line);
        write (log_stream, "\r\n");
      }

        //����� ��������� �� ����

      log_stream.Buffer ().Flush ();
    }    
    
      //������ � ������� ������������
    void SetView (const GameView& view)
    {
      if (current_view)
      {
        try
        {
          current_view->FlushResources ();                        
        }
        catch (...)
        {
          //���������� ���� ����������
        }
      }
      
      current_view = view;                  
      
      if (current_view)
      {
        try
        {
          try
          {
            current_view->LoadResources (scene_player, *device);        

            window.Invalidate (); 
          }
          catch (...)
          {
            current_view->FlushResources ();
            
            current_view = GameView ();
            
            throw;
          }
        }
        catch (std::exception& e)
        {
          LogMessage (format ("Exception at LoadResources: %s", e.what ()).c_str ());
        }
        catch (...)
        {
          LogMessage ("Exception at LoadResources");
        }
      }      
    }

    const GameView& View () const { return current_view; }

    const ::Configuration& Configuration () const
    {
      return configuration;
    }

  private:
      //����� ������ � ����-��������������
    size_t LogWriteBuffer (const void* buffer, size_t size)
    {
      size_t result = log_file.Write (buffer, size);

      log_file.Flush ();

      return result;
    }
  
      //���������� �������� ����
    void OnClose ()
    {
      syslib::Application::Exit (0);
    }
  
      //���������� ��������� �������� ����
    void OnResize ()
    {
      try
      {
        Viewport vp;
        syslib::Rect client_rect = window.ClientRect ();

        vp.x         = client_rect.left;
        vp.y         = client_rect.top;
        vp.width     = client_rect.right - client_rect.left;
        vp.height    = client_rect.bottom - client_rect.top;
        vp.min_depth = 0;
        vp.max_depth = 1;

        device->RSSetViewport (vp);

        window.Invalidate ();
      }
      catch (std::exception& e)
      {
        LogMessage (format ("Exception at window resize: %s", e.what ()).c_str ());
      }
      catch (...)
      {
        LogMessage ("Exception at window resize");
      }
    }        

      //���������� ����������� ����
    void OnRedraw ()
    {
      try
      {
        Color4f clear_color;

        clear_color.red   = 0;
        clear_color.green = 0.7f;
        clear_color.blue  = 0.7f;
        clear_color.alpha = 0;

        device->ClearViews (ClearFlag_All, clear_color, 1.0f, 0);

        if (current_view)
          current_view->OnDraw ();
       
        swap_chain->Present ();    
      }
      catch (std::exception& e)
      {
        LogMessage (format ("Exception at window redraw: %s", e.what ()).c_str ());
      }
      catch (...)
      {
        LogMessage ("Exception at window redraw");
      }
    }
    
      //���������� ������� / ���������� ������ ����
    void OnMouse (syslib::WindowEvent event, const syslib::WindowEventContext& context)
    {
      if (!current_view)
        return;

      try
      {
        syslib::Rect client_rect = window.ClientRect ();

        int cursor_x = (context.cursor_position.x - client_rect.left) * current_view->Width () / (client_rect.right - client_rect.left),
            cursor_y = (context.cursor_position.y - client_rect.top) * current_view->Height () / (client_rect.bottom - client_rect.top);

        current_view->OnMouse (event, cursor_x, cursor_y);

//        window.Invalidate ();        
      }
      catch (std::exception& e)
      {
        LogMessage (format ("Exception at process mouse click: %s", e.what ()).c_str ());
      }
      catch (...)
      {
        LogMessage ("Exception at process mouse click");
      }
    }
    
      //���������� "��������� ����" ����������
    void OnIdle ()
    {
      if (!current_view)
        return;
        
      try
      {
        current_view->OnIdle ();
        
        window.Invalidate ();
      }
      catch (std::exception& e)
      {
        LogMessage (format ("Exception at idle: %s", e.what ()).c_str ());
      }
      catch (...)
      {
        LogMessage ("Exception at idle");
      }
    }        

  private:
    ::Configuration     configuration;       //����������
    OutputFile          log_file;            //���� ��������������
    OutputTextStream    log_stream;          //����� ���������������� ������ ����������
    syslib::Window      window;              //������� ���� ����������
    SwapChainPtr        swap_chain;          //������� ������ �������� ���� ����������
    DevicePtr           device;              //���������� ���������� �������� ���� ����������
    xtl::connection     app_idle_connection; //���������� ������� ����������� ��������� ���� ����������
    GameView            current_view;        //������� ������� �����������
    sound::SoundManager *sound_manager;      //�������� ������
    sound::ScenePlayer  *scene_player;       //������������� ������
};

/*
    ����������� / ����������
*/

MyApplication::MyApplication ()
{
  impl = new Impl;
}

MyApplication::~MyApplication ()
{
}

/*
    ��������� ���������� ����������
*/

MyApplication& MyApplication::Instance ()
{
  static MyApplication application;
  
  return application;
}

/*
    ����������������
*/

void MyApplication::LogMessage (const char* message)
{
  impl->LogMessage (message);
}

void MyApplication::LogFormatMessage (const char* format, ...)
{
  va_list list;
  
  va_start (list, format);
  
  VLogFormatMessage (format, list);
}

void MyApplication::VLogFormatMessage (const char* format, va_list list)
{
  if (!format)
    return;

  LogMessage (common::vformat (format, list).c_str ());
}

/*
    ��������� ���������� ����������, ��������� �� ������� ������� ����������
*/

size_t MyApplication::Milliseconds ()
{
  return clock () * 1000 / CLOCKS_PER_SEC;
}

/*
    ��������� �������� �����������
*/

void MyApplication::SetView (const GameView& view)
{
  impl->SetView (view);
}

const GameView& MyApplication::View () const
{
  return impl->View ();
}

const ::Configuration& MyApplication::Configuration () const
{
  return impl->Configuration ();
}

