#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl::windows;

namespace
{

/*
    ���������
*/

const char* WINDOW_CLASS_NAME = "OpenGL driver listener window class";

/*
    ���������� ��������� ��������� ��������� ����
*/

LRESULT CALLBACK WindowMessageHandler (HWND wnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    //��������� ��������� �� ��������� ������� ������

  IWindowListener* listener = reinterpret_cast<IWindowListener*> (GetWindowLong (wnd, GWL_USERDATA));
  
    //���� ��������� �� ���������������� ������ �� ���������� - ������ ������� ��� ����������
    
  if (!listener)
  {
    if (message == WM_CREATE) 
    {
      CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*> (lparam);
      
      if (!cs->lpCreateParams)
        return 0;

      listener = reinterpret_cast<IWindowListener*> (cs->lpCreateParams);

        //������������� ��������� �� ���������������� ������

      SetWindowLong (wnd, GWL_USERDATA, (LONG)listener);

      return 0;
    }

    return DefWindowProc (wnd, message, wparam, lparam);
  }

    //�������� ���������  

  try
  {  
    switch (message)
    {
      case WM_DESTROY:
        listener->OnDestroy ();
        return 0;
      case WM_DISPLAYCHANGE:
        listener->OnDisplayModeChange ();
        return 0;
      default:
        break;
    }
    
  }
  catch (...)
  {
    //���������� ���� ����������
  }

    //�������� ���������� ����������� "�� ���������"

  return DefWindowProc (wnd, message, wparam, lparam);    
}

/*
    ����������� �������� ������
*/

void RegisterWindowClass ()
{
  WNDCLASS wc;

  memset (&wc, 0, sizeof (wc));

  wc.style         = CS_OWNDC;
  wc.lpfnWndProc   = &WindowMessageHandler;
  wc.hInstance     = GetModuleHandle (0);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.lpszClassName = WINDOW_CLASS_NAME;

  if (!RegisterClass (&wc))
    raise_error ("::RegisterClass");
}

void UnregisterWindowClass ()
{
  UnregisterClass (WINDOW_CLASS_NAME, GetModuleHandle (0));
}

/*
    ���������� ����������� ��������� ����
*/

size_t listener_window_instances = 0;

}

/*
    ����������� / ����������
*/

DummyWindow::DummyWindow (HWND parent, IWindowListener* listener)
{
  try
  {
      //����������� ������ ����
    
    if (!listener_window_instances++)
      RegisterWindowClass ();
      
    try
    {
        //�������� ����
      
      window = CreateWindowA (WINDOW_CLASS_NAME, "", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                              parent, 0, GetModuleHandle (0), listener);

      if (!window)
        raise_error ("::CreateWindowA");
    }
    catch (...)
    {
      --listener_window_instances;
      throw;
    }    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::windows::DummyWindow::DummyWindow");
    throw;
  }
}

DummyWindow::~DummyWindow ()
{
    //����������� ����

  DestroyWindow (window);
  
    //������ ����������� ������ ����
  
  if (!--listener_window_instances)
    UnregisterWindowClass ();
}

/*
    ��������� ����������� ����
*/

HWND DummyWindow::GetHandle () const
{
  return window;
}
