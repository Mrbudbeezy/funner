#include "shared.h"

using namespace Windows::UI::Core;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;

using namespace syslib;
using namespace syslib::win8;

namespace
{

ref class ApplicationView;

ApplicationView^ app_view;

/// ��������� ������� ����������
class IApplicationViewListener
{
  public:
    virtual void RunCore () = 0;
    virtual void OnExit () = 0;
};

/// �������� ���� ����������
ref class ApplicationView sealed: Windows::ApplicationModel::Core::IFrameworkView, public IApplicationContext
{
  internal:
    // Constructor
    ApplicationView (IApplicationViewListener* in_listener) : listener (in_listener) {}

  public:
    // This method is called on application launch.
    virtual void Initialize (CoreApplicationView^ view)
    {
      view->Activated += ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^> (this, &ApplicationView::OnActivated);
    }

    // This method is called after Initialize.
    virtual void SetWindow (CoreWindow^ window)
    {
      main_window = window;
    }

    virtual void Load (Platform::String^ entryPoint)
    {
    }

    virtual void Run ()
    {
      listener->RunCore ();
    }

    // This method is called before the application exits.
    virtual void Uninitialize ()
    {
    }

    /// Main window
    virtual Windows::UI::Core::CoreWindow^ MainWindow ()
    {
      return main_window.Get ();
    }

  private:
    void OnActivated (CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
    {
      CoreWindow::GetForCurrentThread ()->Activate ();
    }

  private:
    IApplicationViewListener*   listener;
    Platform::Agile<CoreWindow> main_window;
};

/// ������� �������� ��������� ���� ����������
ref class ApplicationSource sealed: Windows::ApplicationModel::Core::IFrameworkViewSource
{
  internal:
    ApplicationSource (IApplicationViewListener* in_listener) 
      : listener (in_listener)
    {
      Windows::ApplicationModel::Core::CoreApplication::Exiting += ref new EventHandler<Platform::Object^> (this, &ApplicationSource::OnExit);
    }

  public:
    virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView ()
    {
      return ref new ApplicationView (listener);
    }

    void OnExit (Platform::Object^, Platform::Object^)
    {   
      listener->OnExit ();
    }

  private:
    IApplicationViewListener* listener;
};

/// ������� ����������
class Win8ApplicationDelegate: public IApplicationDelegate, public xtl::reference_counter, private IApplicationViewListener
{
  public:
///�����������
    Win8ApplicationDelegate ()
    {
      idle_enabled    = false;
      is_exited       = false;
      listener        = 0;
      wake_up_message = RegisterWindowMessageW (L"Win8ApplicationDelegate.WakeUp");
      main_thread_id  = 0;
      
      if (!wake_up_message)
        raise_error ("::RegisterWindowMessage");
    }

///������ ����� ��������� ���������
    void Run ()
    {
      try
      {
	auto source = ref new ApplicationSource (this);

	Windows::ApplicationModel::Core::CoreApplication::Run (source);
      }
      catch (Platform::Exception^ e)
      {
        throw xtl::format_operation_exception ("syslib::Win8ApplicationDelegate::Run", "Platform exception %s. %s", tostring (e->Message).c_str (), _com_error (e->HResult).ErrorMessage ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::Win8ApplicationDelegate::Run");
        throw;
      }
    }

///����� �� ����������
    void Exit (int code)
    {
      Windows::ApplicationModel::Core::CoreApplication::Exit ();
    }

///��������� ������������� ������ ������� idle
    void SetIdleState (bool state)
    {
      idle_enabled = state;
      
      if (state)
      {
        if (!::PostThreadMessage (main_thread_id ? main_thread_id : GetCurrentThreadId (), wake_up_message, 0, 0))
          raise_error ("::PostThreadMessage");
      }
    }

///��������� ��������� ������� ����������
    void SetListener (IApplicationListener* in_listener)
    {
      listener = in_listener;
    }
    
///������� ������
    void AddRef ()
    {
      addref (this);
    }
    
    void Release ()
    {
      release (this);
    }

  private:
/// ���������� � ������ �� �����
    void OnExit ()
    {
      is_exited = true;
    }

/// ������� ����
    void RunCore ()
    {
      try
      {
        main_thread_id = GetCurrentThreadId ();
        
        if (listener)
          listener->OnInitialize ();
        
        while (!is_exited)
        {
          DoNextEvents ();

           //���� ��� ������������ OnIdle - ���������������� ����������

          if (!idle_enabled)
          {
            if (!is_exited)
              DoNextEvents ();
          }
          else
          {
            if (listener)
              listener->OnIdle ();
          }
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::Win8ApplicationDelegate::Run");
        throw;
      }
    }

///��������� ���������� �������
    void DoNextEvents ()
    {
      try
      {
        if (CoreWindow^ window = CoreWindow::GetForCurrentThread ())
          window->Dispatcher->ProcessEvents (CoreProcessEventsOption::ProcessOneAndAllPending);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("syslib::Win8ApplicationDelegate::DoNextEvents");
        throw;
      }
    }

  private:
    bool                  idle_enabled;
    bool                  is_exited;
    IApplicationListener* listener;
    UINT                  wake_up_message;
    DWORD                 main_thread_id;
};

}

namespace syslib
{

namespace win8
{

/// ��������� ��������� ����������
IApplicationContext^ get_context ()
{
  return app_view;
}

}

}

/*
    �������� �������� ����������
*/

IApplicationDelegate* WindowsApplicationManager::CreateDefaultApplicationDelegate ()
{
  return new Win8ApplicationDelegate;
}

/*
   �������� URL �� ������� ��������
*/

void WindowsApplicationManager::OpenUrl (const char* url)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ��������� �������� ���������� �����
*/

#undef GetEnvironmentVariable

stl::string WindowsApplicationManager::GetEnvironmentVariable (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ���������� �����������������
*/

void WindowsApplicationManager::SetScreenSaverState (bool state)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

bool WindowsApplicationManager::GetScreenSaverState ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
