#ifndef SYSLIB_WINDOW_HEADER
#define SYSLIB_WINDOW_HEADER

#include <stddef.h>
#include <exception>
#include <syslib/keydefs.h>
#include <xtl/functional_fwd>

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum WindowStyle
{
  WindowStyle_PopUp,      //���� ��� �����
  WindowStyle_Overlapped, //���� � ������
  
  WindowStyle_Default = WindowStyle_Overlapped
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum WindowEvent
{
  WindowEvent_OnChangeHandle,            //������ �������������� ���������� ����
  WindowEvent_OnDestroy,                 //���� ����������
  WindowEvent_OnClose,                   //���������� ����� ��������� ����
  WindowEvent_OnActivate,                //���� ����� ��������
  WindowEvent_OnDeactivate,              //���� ��������� ���� ��������
  WindowEvent_OnShow,                    //���� ����� �������
  WindowEvent_OnHide,                    //���� ����� �� �������
  WindowEvent_OnSetFocus,                //���� �������� ����� �����
  WindowEvent_OnLostFocus,               //���� �������� ����� �����
  WindowEvent_OnPaint,                   //���������� �����������
  WindowEvent_OnSize,                    //���������� ������� ����
  WindowEvent_OnMove,                    //���������� ��������� ����
  WindowEvent_OnMouseMove,               //������ ���� ������������ ��� �������� ����
  WindowEvent_OnMouseVerticalWheel,      //���������� ��������� ������������� ������ ����
  WindowEvent_OnMouseHorisontalWheel,    //���������� ��������� ��������������� ������ ����
  WindowEvent_OnLeftButtonDown,          //������ ����� ������ ����
  WindowEvent_OnLeftButtonUp,            //�������� ����� ������ ����
  WindowEvent_OnLeftButtonDoubleClick,   //������� ������ ����� �������� ����
  WindowEvent_OnRightButtonDown,         //������ ������ ������ ����
  WindowEvent_OnRightButtonUp,           //�������� ������ ������ ����  
  WindowEvent_OnRightButtonDoubleClick,  //������� ������ ������ �������� ����
  WindowEvent_OnMiddleButtonDown,        //������ ������� ������ ����  
  WindowEvent_OnMiddleButtonUp,          //�������� ������� ������ ����
  WindowEvent_OnMiddleButtonDoubleClick, //������� ������ ������ �������� ����  
  WindowEvent_OnKeyDown,                 //������ ������� ����������
  WindowEvent_OnKeyUp,                   //�������� ������� ����������
  WindowEvent_OnChar,                    //� ������ ����� ���� �������� ������
  
  WindowEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Point
{
  size_t x, y;

  Point ();
  Point (size_t x, size_t y);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Rect
{
  size_t left, top, right, bottom;
  
  Rect ();
  Rect (size_t left, size_t top, size_t right, size_t bottom);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct WindowEventContext
{
  const void*   handle;                          //���������� ����
  Rect          window_rect;                     //������� ����
  Point         cursor_position;                 //���������� �������
  float         mouse_vertical_wheel_delta;      //��������� ��������� ������������� ������ ���� (� �������)
  float         mouse_horisontal_wheel_delta;    //��������� ��������� ��������������� ������ ���� (� �������)
  Key           key;                             //������� ����������
  ScanCode      key_scan_code;                   //����-��� ������� ����������
  wchar_t       char_code;                       //��� �������
  bool          mouse_left_button_pressed : 1;   //������������ �� ����� ������ ����
  bool          mouse_right_button_pressed : 1;  //������������ �� ������ ������ ����
  bool          mouse_middle_button_pressed : 1; //������������ �� ������� ������ ����
  bool          keyboard_alt_pressed : 1;        //������������ Alt
  bool          keyboard_control_pressed : 1;    //������������ Control
  bool          keyboard_shift_pressed : 1;      //������������ Shift
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������: ������� ������ � �������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class ClosedWindowException: public std::exception
{
  public:
    const char* what () const throw () { return "closed window exception"; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��� ������ � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Window
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� � ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Window  (); //�������� �������� ����
    Window  (WindowStyle style);
    Window  (WindowStyle style, size_t width, size_t height);
    ~Window ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Init (WindowStyle style);
    void Init (WindowStyle style, size_t width, size_t height);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Close       ();
    void CancelClose (); //����� ���� ������ ����� �� ������������ ������� WindowEvent_OnClose
    void ForceClose  (); //������ CancelClose ������������
    
    bool IsClosed () const; //��������: ������� �� ����

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char*    Title        () const;
    const wchar_t* TitleUnicode () const;
    void           SetTitle     (const char*);
    void           SetTitle     (const wchar_t*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    WindowStyle Style    () const;
    void        SetStyle (WindowStyle style);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Width     () const;
    size_t Height    () const;
    void   SetWidth  (size_t width);
    void   SetHeight (size_t height);
    void   SetSize   (size_t width, size_t height);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���� / ���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Rect WindowRect () const;
    Rect ClientRect () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    Point Position    () const;
    void  SetPosition (const Point& position);
    void  SetPosition (size_t x, size_t y);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Point  CursorPosition    () const;    
    void   SetCursorPosition (const Point&);
    void   SetCursorPosition (size_t x, size_t y);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsVisible  () const;
    void SetVisible (bool state);
    void Show       () { SetVisible (true);  }
    void Hide       () { SetVisible (false); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsActive   () const;
    void SetActive  (bool state);
    void Activate   () { SetActive (true); }
    void Deactivate () { SetActive (false); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool HasFocus   () const;
    void SetFocus   (bool state = true);
    void KillFocus  () { SetFocus (false); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������� ���� (������� OnPaint)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Invalidate ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������������� ����������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const void* Handle () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (Window&, WindowEvent, const WindowEventContext&)> EventHandler;

    xtl::connection RegisterEventHandler (WindowEvent event, const EventHandler& handler);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������������� ������� ����������� ����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (const char*)> LogHandler;

    void              SetDebugLog (const LogHandler&);
    const LogHandler& DebugLog    () const;

  private:
    const void* CheckedHandle  () const;
    void        SetHandle      (const void* handle);
    void        Notify         (WindowEvent, const WindowEventContext&);
    void        Notify         (WindowEvent);
    static void MessageHandler (WindowEvent, const WindowEventContext&, void*);

  private:
    Window (const Window&); //no impl
    Window& operator = (const Window&); //no impl

  private:
    struct Impl;
    Impl* impl;
};

#include <syslib/detail/window.inl>

}

#endif
