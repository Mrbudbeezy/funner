#include "shared.h"

using namespace engine;
using namespace engine::dot_net;
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Windows::Forms;

namespace Funner
{

///���������� ���������
public delegate void WindowPaintEventHandler ();
public delegate void WindowSizeEventHandler  (unsigned int width, unsigned int height);
public delegate void WindowMouseEventHandler (int x, int y);
public delegate void WindowKeyEventHandler   (String^ key);

///���������� ���� ������
public ref class WindowImpl: public Form
{
  public:
    ///�������
    WindowPaintEventHandler^ EnginePaint;
    WindowSizeEventHandler^  EngineSize;
    WindowMouseEventHandler^ EngineMouseMove;
    WindowMouseEventHandler^ EngineMouseEnter;
    WindowMouseEventHandler^ EngineMouseLeave;
    WindowMouseEventHandler^ EngineMouseHover;
    WindowMouseEventHandler^ EngineLeftButtonDown;
    WindowMouseEventHandler^ EngineLeftButtonUp;
    WindowMouseEventHandler^ EngineLeftButtonDoubleClick;
    WindowMouseEventHandler^ EngineRightButtonDown;
    WindowMouseEventHandler^ EngineRightButtonUp;
    WindowMouseEventHandler^ EngineRightButtonDoubleClick;
    WindowMouseEventHandler^ EngineMiddleButtonDown;
    WindowMouseEventHandler^ EngineMiddleButtonUp;
    WindowMouseEventHandler^ EngineMiddleButtonDoubleClick;
    WindowKeyEventHandler^   EngineKeyUp;
    WindowKeyEventHandler^   EngineKeyDown;
};

namespace
{

///��������� ������� ���� ������
class EngineWindowListener: public IWindowListener
{
  public:
    ///�����������
    EngineWindowListener (msclr::auto_gcroot<WindowImpl^> in_window)
      : window (in_window)
    {
    }
    
    ///���������� ������� ����������� ����
    void OnPaint ()
    {
      if (window->EnginePaint != nullptr)
        window->EnginePaint ();
    }
    
    ///���������� ��������� �������� ����
    void OnSize (size_t width, size_t height)
    {
      if (window->EngineSize != nullptr)
        window->EngineSize (width, height);    
    }

    ///����������� ������� �����
    void OnMouseMove (int x, int y)
    {
      if (window->EngineMouseMove != nullptr)
        window->EngineMouseMove (x, y);
    }
    
    void OnMouseEnter (int x, int y)
    {
      if (window->EngineMouseEnter != nullptr)
        window->EngineMouseEnter (x, y);
    }

    void OnMouseHover (int x, int y)
    {
      if (window->EngineMouseHover != nullptr)
        window->EngineMouseHover (x, y);
    }

    void OnMouseLeave (int x, int y)
    {
      if (window->EngineMouseLeave != nullptr)
        window->EngineMouseLeave (x, y);
    }

    void OnMouseDown (MouseButton button, int x, int y)
    {
      switch (button)
      {
        case MouseButton_Left:
          if (window->EngineLeftButtonDown != nullptr)
            window->EngineLeftButtonDown (x, y);
            
          break;
        case MouseButton_Right:
          if (window->EngineRightButtonDown != nullptr)
            window->EngineRightButtonDown (x, y);

          break;
        case MouseButton_Middle:
          if (window->EngineMiddleButtonDown != nullptr)
            window->EngineMiddleButtonDown (x, y);

          break;
        default:
          break;
      }
    }

    void OnMouseUp (MouseButton button, int x, int y)
    {
      switch (button)
      {
        case MouseButton_Left:
          if (window->EngineLeftButtonUp != nullptr)
            window->EngineLeftButtonUp (x, y);
            
          break;
        case MouseButton_Right:
          if (window->EngineRightButtonUp != nullptr)
            window->EngineRightButtonUp (x, y);

          break;
        case MouseButton_Middle:
          if (window->EngineMiddleButtonUp != nullptr)
            window->EngineMiddleButtonUp (x, y);

          break;
        default:
          break;
      }
    }

    void OnMouseDoubleClick (MouseButton button, int x, int y)
    {
      switch (button)
      {
        case MouseButton_Left:
          if (window->EngineLeftButtonDoubleClick != nullptr)
            window->EngineLeftButtonDoubleClick (x, y);
            
          break;
        case MouseButton_Right:
          if (window->EngineRightButtonDoubleClick != nullptr)
            window->EngineRightButtonDoubleClick (x, y);

          break;
        case MouseButton_Middle:
          if (window->EngineMiddleButtonDoubleClick != nullptr)
            window->EngineMiddleButtonDoubleClick (x, y);

          break;
        default:
          break;
      }
    }

    void OnKeyDown (const char* key)
    {
      if (window->EngineKeyDown != nullptr)
        window->EngineKeyDown (gcnew String (key));
    }

    void OnKeyUp (const char* key)
    {
      if (window->EngineKeyUp != nullptr)
        window->EngineKeyUp (gcnew String (key));
    }

  private:
    msclr::auto_gcroot<WindowImpl^> window; //�������� ������ �� ����
};

}

///�������� ��������� ���� ������
public ref class Window: public WindowImpl
{
  public:
    ///�����������
    Window (IEngine* in_engine, const char* name)
      : engine (in_engine)
      , window (0)
    {
      if (!engine)
        throw gcnew ArgumentException ("Null engine passed to constructor of Funner.Window class");
        
      if (!name)
        throw gcnew ArgumentException ("Null name passed to constructor of Funner.Window class");
        
      window_listener = new EngineWindowListener (this);

      try
      {
        window = engine->CreateWindow (name);
      }
      catch (...)
      {
        delete window_listener;
        throw;
      }
    }

    ///����������
    ~Window ()
    {
      delete window;
      delete window_listener;
      
      window = 0;
      window_listener = 0;
    }
    
  private:
    IEngine*              engine;          //��������� �� ������
    IWindow*              window;          //��������� �� ���� ������
    EngineWindowListener* window_listener; //��������� ������� ����
};

/*
    ����� ������
*/

public ref class Engine
{
  public:
///�������� ���������� ��������� ������� ����
    static Window^ CreateWindow (String^ name)
    {
      return gcnew Window (GetEngine (), AutoString (name).Data ());
    }

  private:
///������ native API ������
    [DllImport ("funner.dll")]
    static IEngine* FunnerInit ();
    
///��������� ������
    static IEngine* GetEngine ()
    {
      if (engine)
        return engine;
      
      engine = FunnerInit ();
      
      if (!engine)
        throw gcnew InvalidOperationException ("Funner native engine has bad entries");
        
      return engine;
    }

  private:
    static IEngine* engine = 0; //��������� �� ������ ������
};

}
