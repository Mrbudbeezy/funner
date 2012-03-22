#include "shared.h"

using namespace syslib;
using namespace syslib::android;

//TODO: ��������� ���������� � CreateWindow � �������� ����

/*
    ����
*/

struct syslib::window_handle
{
  global_ref<jobject>  view;                           //android ����
  NativeWindow         window;                         //�������� ����
  WindowMessageHandler message_handler;                //���������� ���������
  void*                user_data;                      //���������������� ������ ����
  unsigned int         background_color;               //���� ������� �����
  unsigned int         background_state;               //������� ������� ����
  jmethodID            get_top_method;                 //����� ��������� �������� ���� ����
  jmethodID            get_left_method;                //����� ��������� ������ ���� ����
  jmethodID            get_width_method;               //����� ��������� ������ ����
  jmethodID            get_height_method;              //����� ��������� ������ ����
  jmethodID            layout_method;                  //����� ��������� �������� � ��������� ����
  jmethodID            set_visibility_method;          //����� ��������� ��������� ����
  jmethodID            get_visibility_method;          //����� ��������� ��������� ����
  jmethodID            request_focus_method;           //����� ������� ������ �����
  jmethodID            set_background_color_method;    //����� ��������� ����� ������� ����� ����
  jmethodID            maximize_method;                //����� ������������ ����
  jmethodID            get_surface_method;             //����� ��������� �����������
  jmethodID            post_invalidate_method;         //����� ���������� � ������������� ����������� ����
  jmethodID            bring_to_front_method;          //����� ����������� ���� �� �������� ����
  bool                 is_multitouch_enabled;          //������� �� multitouch
  volatile bool        is_native_handle_received;      //������� �� android window handle
  
///�����������
  window_handle ()
    : message_handler (0)
    , user_data (0)
    , background_color (0)
    , background_state (0)
    , is_multitouch_enabled (false)
    , is_native_handle_received (false)
  {
    window.native_window = 0;
    window.view          = 0;
    
    MessageQueueSingleton::Instance ()->RegisterHandler (this);
  }

///����������
  ~window_handle ()
  {
    MessageQueueSingleton::Instance ()->UnregisterHandler (this);
    
    if (window.native_window)
      ANativeWindow_release (window.native_window);
  }
 
  void Notify (WindowEvent event, const WindowEventContext& context)
  {
    try
    {
      message_handler (this, event, context, user_data);
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }
  
  void OnLayoutCallback (int left, int top, int right, int bottom)
  {
    WindowEventContext context;

    memset (&context, 0, sizeof (context));

    Notify (WindowEvent_OnMove, context);
    Notify (WindowEvent_OnSize, context);
  }

  void OnDisplayHintCallback (int hint)
  {
  }

  void OnDrawCallback ()
  {
    WindowEventContext context;

    memset (&context, 0, sizeof (context));

    Notify (WindowEvent_OnPaint, context);
  }

  void FillTouchEventContext (int pointer_id, float x, float y, WindowEventContext& context)
  {
    memset (&context, 0, sizeof (context));

    context.touches_count = 1;

    Touch& touch = context.touches [0];

    touch.touch_id   = pointer_id;
    touch.position.x = x;
    touch.position.y = y;
  }

  void OnTouchCallback (int pointer_id, int action, float x, float y)
  {
    WindowEventContext context;

    FillTouchEventContext (pointer_id, x, y, context);

    switch (action)
    {
      case AMOTION_EVENT_ACTION_DOWN:
        Notify (WindowEvent_OnTouchesBegan, context);
        break;
      case AMOTION_EVENT_ACTION_MOVE:
        Notify (WindowEvent_OnTouchesMoved, context);
        break;
      case AMOTION_EVENT_ACTION_UP:
      case AMOTION_EVENT_ACTION_CANCEL:
        Notify (WindowEvent_OnTouchesEnded, context);
        break;
      default:
        printf ("Unhandled touch action %d received\n", action);
    }
  }

  void OnDoubletapCallback (int pointer_id, float x, float y)
  {
    WindowEventContext context;

    FillTouchEventContext (pointer_id, x, y, context);

    Notify (WindowEvent_OnTouchesDoubletap, context);
  }

  void OnTrackballCallback (int pointer_id, int action, float x, float y)
  {
//    printf ("on_trackball_callback(%d, %d, %g, %g)\n", pointer_id, action, x, y);
//    fflush (stdout);
  }

  void OnKeyCallback (int key, int action, bool is_alt_pressed, bool is_shift_pressed, bool is_sym_pressed)
  {
    WindowEventContext context;

    memset (&context, 0, sizeof (context));

    context.keyboard_alt_pressed     = is_alt_pressed;
    context.keyboard_shift_pressed   = is_shift_pressed;
    context.keyboard_control_pressed = is_sym_pressed;
    context.key_scan_code            = (ScanCode)key;

    Key syslib_key = Key_Unknown;

    switch (key)
    {
      case AKEYCODE_0:             syslib_key = Key_0;            break;
      case AKEYCODE_1:             syslib_key = Key_1;            break;
      case AKEYCODE_2:             syslib_key = Key_2;            break;
      case AKEYCODE_3:             syslib_key = Key_3;            break;
      case AKEYCODE_4:             syslib_key = Key_4;            break;
      case AKEYCODE_5:             syslib_key = Key_5;            break;
      case AKEYCODE_6:             syslib_key = Key_6;            break;
      case AKEYCODE_7:             syslib_key = Key_7;            break;
      case AKEYCODE_8:             syslib_key = Key_8;            break;
      case AKEYCODE_9:             syslib_key = Key_9;            break;
      case AKEYCODE_A:             syslib_key = Key_A;            break;
      case AKEYCODE_B:             syslib_key = Key_B;            break;
      case AKEYCODE_C:             syslib_key = Key_C;            break;
      case AKEYCODE_D:             syslib_key = Key_D;            break;
      case AKEYCODE_E:             syslib_key = Key_E;            break;
      case AKEYCODE_F:             syslib_key = Key_F;            break;
      case AKEYCODE_G:             syslib_key = Key_G;            break;
      case AKEYCODE_H:             syslib_key = Key_H;            break;
      case AKEYCODE_I:             syslib_key = Key_I;            break;
      case AKEYCODE_J:             syslib_key = Key_J;            break;
      case AKEYCODE_K:             syslib_key = Key_K;            break;
      case AKEYCODE_L:             syslib_key = Key_L;            break;
      case AKEYCODE_M:             syslib_key = Key_M;            break;
      case AKEYCODE_N:             syslib_key = Key_N;            break;
      case AKEYCODE_O:             syslib_key = Key_O;            break;
      case AKEYCODE_P:             syslib_key = Key_P;            break;
      case AKEYCODE_Q:             syslib_key = Key_Q;            break;
      case AKEYCODE_R:             syslib_key = Key_R;            break;
      case AKEYCODE_S:             syslib_key = Key_S;            break;
      case AKEYCODE_T:             syslib_key = Key_T;            break;
      case AKEYCODE_U:             syslib_key = Key_U;            break;
      case AKEYCODE_V:             syslib_key = Key_V;            break;
      case AKEYCODE_W:             syslib_key = Key_W;            break;
      case AKEYCODE_X:             syslib_key = Key_X;            break;
      case AKEYCODE_Y:             syslib_key = Key_Y;            break;
      case AKEYCODE_Z:             syslib_key = Key_Z;            break;
      case AKEYCODE_COMMA:         syslib_key = Key_Comma;        break;
      case AKEYCODE_PERIOD:        syslib_key = Key_Dot;          break;
      case AKEYCODE_ALT_LEFT:
      case AKEYCODE_ALT_RIGHT:     syslib_key = Key_Alt;          break;
      case AKEYCODE_SHIFT_LEFT:
      case AKEYCODE_SHIFT_RIGHT:   syslib_key = Key_Shift;        break;
      case AKEYCODE_TAB:           syslib_key = Key_Tab;          break;
      case AKEYCODE_SPACE:         syslib_key = Key_Space;        break;
      case AKEYCODE_ENTER:         syslib_key = Key_Enter;        break;
      case AKEYCODE_DEL:           syslib_key = Key_Delete;       break;
      case AKEYCODE_MINUS:         syslib_key = Key_Minus;        break;
      case AKEYCODE_LEFT_BRACKET:  syslib_key = Key_LeftBracket;  break;
      case AKEYCODE_RIGHT_BRACKET: syslib_key = Key_RightBracket; break;
      case AKEYCODE_BACKSLASH:     syslib_key = Key_BackSlash;    break;
      case AKEYCODE_SEMICOLON:     syslib_key = Key_Semicolon;    break;
      case AKEYCODE_APOSTROPHE:    syslib_key = Key_Apostrophe;   break;
      case AKEYCODE_SLASH:         syslib_key = Key_Slash;        break;
      case AKEYCODE_PLUS:          syslib_key = Key_Plus;         break;
      case AKEYCODE_PAGE_UP:       syslib_key = Key_PageUp;       break;
      case AKEYCODE_PAGE_DOWN:     syslib_key = Key_PageDown;     break;
    }

    context.key = syslib_key;
    
    static const int ACTION_DOWN = 0, ACTION_UP = 1;
    
    switch (action)
    {
      case ACTION_DOWN:
        Notify (WindowEvent_OnKeyDown, context);
        break;
      case ACTION_UP:
        Notify (WindowEvent_OnKeyUp, context);
        break;
      default:
        break;
    }
  }

  void OnFocusCallback (bool gained)
  {
    WindowEventContext context;

    memset (&context, 0, sizeof (context));
        
    if (gained) Notify (WindowEvent_OnSetFocus, context);
    else        Notify (WindowEvent_OnLostFocus, context);
  }

  void OnSurfaceCreatedCallback ()
  {
      //��������� �����������
      
    local_ref<jobject> surface = check_errors (get_env ().CallObjectMethod (view.get (), get_surface_method));
    
    if (!surface)
      throw xtl::format_operation_exception ("", "EngineView::getSurfaceThreadSafe failed");      

      //��������� ����������� ����
      
    ANativeWindow* new_window = ANativeWindow_fromSurface (&get_env (), surface.get ());
    
    if (new_window != window.native_window)
    {
      window.native_window = new_window;
      
        //���������� �� ��������� �����������
      
      WindowEventContext context;

      memset (&context, 0, sizeof (context));    
      
      context.handle = &window;
      
      Notify (WindowEvent_OnChangeHandle, context);    
    }
  }

  void OnSurfaceDestroyedCallback ()
  {
    if (!window.native_window)
      return;
    
    ANativeWindow_release (window.native_window);
    
    window.native_window = 0;

      //���������� �� ��������� �����������

    WindowEventContext context;

    memset (&context, 0, sizeof (context));    

    Notify (WindowEvent_OnChangeHandle, context);
  }  
};

namespace
{

///���������
const int VIEW_VISIBLE   = 0; //���� ������
const int VIEW_INVISIBLE = 4; //���� ��������
const int VIEW_GONE      = 8; //���� �������� � ��������� �� ������� �������

///��������� ������ ����
class JniWindowManager
{
  public:
///�����������
    JniWindowManager ()
    {
      try
      {          
          //��������� ������ Activity

        activity_class = get_env ().GetObjectClass (get_context ().activity.get ());
        
        if (!activity_class)
          throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed");
          
        create_view_method = find_method (&get_env (), activity_class.get (), "createEngineView", "(Ljava/lang/String;J)Landroid/view/View;");
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::android::JniWindowManager::JniWindowManager");
        throw;
      }
    }
    
///�������� ����
    local_ref<jobject> CreateView (const char* init_string, void* window_ref)
    {
      const ApplicationContext& context = get_context ();     
      
      local_ref<jobject> view = check_errors (get_env ().CallObjectMethod (context.activity.get (), create_view_method, tojstring (init_string), window_ref));

      if (!view)
        throw xtl::format_operation_exception ("", "EngineActivity::createEngineView failed");
        
      return view;
    }
    
///����� ����������� ����
    static window_t FindWindow (jobject view)
    {
      try
      {
        if (!view)
          return 0;

        JNIEnv& env = get_env ();

        local_ref<jclass> view_class = env.GetObjectClass (view);

        if (!view_class)
          throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed (for View)");
        
        jmethodID get_window_ref = env.GetMethodID (view_class.get (), "getWindowRef", "()J");

        if (!get_window_ref)
          return 0;

        return (window_t)check_errors (env.CallLongMethod (view, get_window_ref));
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::AndroidWindowManager::FindWindow");
        throw;
      }
    }
    
  private:
    global_ref<jclass> activity_class;
    jmethodID          create_view_method;
};

typedef common::Singleton<JniWindowManager> JniWindowManagerSingleton;

/*
    ������� ��������� ������
*/

template <class Fn> class WindowMessage: public MessageQueue::Message
{
  public:
    WindowMessage (window_t in_window, const Fn& in_fn)
      : window (in_window)
      , fn (in_fn) {}

    void Dispatch ()
    {
      fn (window);
    }

  private:
    window_t window;
    Fn                 fn;
};

template <class Fn> void push_message (jobject view, const Fn& fn)
{
  try
  {
    window_t window = JniWindowManager::FindWindow (view);

    if (!window)
      return;

    MessageQueueSingleton::Instance ()->PushMessage (window, MessageQueue::MessagePtr (new WindowMessage<Fn> (window, fn), false));
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

void on_layout_callback (JNIEnv& env, jobject view, int left, int top, int right, int bottom)
{
  push_message (view, xtl::bind (&window_handle::OnLayoutCallback, _1, left, top, right, bottom));
}

void on_display_hint_callback (JNIEnv& env, jobject view, int hint)
{
  push_message (view, xtl::bind (&window_handle::OnDisplayHintCallback, _1, hint));
}

void on_draw_callback (JNIEnv& env, jobject view)
{
  push_message (view, xtl::bind (&window_handle::OnDrawCallback, _1));
}

void on_touch_callback (JNIEnv& env, jobject view, int pointer_id, int action, float x, float y)
{
  push_message (view, xtl::bind (&window_handle::OnTouchCallback, _1, pointer_id, action, x, y));
}

void on_doubletap_callback (JNIEnv& env, jobject view, int pointer_id, float x, float y)
{
  push_message (view, xtl::bind (&window_handle::OnDoubletapCallback, _1, pointer_id, x, y));
}

void on_trackball_callback (JNIEnv& env, jobject view, int pointer_id, int action, float x, float y)
{
  push_message (view, xtl::bind (&window_handle::OnTrackballCallback, _1, pointer_id, action, x, y));
}

void on_key_callback (JNIEnv& env, jobject view, int key, int action, jboolean is_alt_pressed, jboolean is_shift_pressed, jboolean is_sym_pressed)
{
  push_message (view, xtl::bind (&window_handle::OnKeyCallback, _1, key, action, is_alt_pressed != 0, is_shift_pressed != 0, is_sym_pressed != 0));
}

void on_focus_callback (JNIEnv& env, jobject view, jboolean gained)
{
  push_message (view, xtl::bind (&window_handle::OnFocusCallback, _1, gained != 0));
}

void on_surface_created_callback (JNIEnv& env, jobject view)
{
  try
  {
    push_message (view, xtl::bind (&window_handle::OnSurfaceCreatedCallback, _1));
    push_message (view, xtl::bind (&window_handle::OnDrawCallback, _1));

    window_t window = JniWindowManager::FindWindow (view);

    if (!window)
      return;    

    window->is_native_handle_received = true;
  }
  catch (...)
  {
  }
}

void on_surface_changed_callback (JNIEnv& env, jobject view, jint format, jint width, jint height)
{
  push_message (view, xtl::bind (&window_handle::OnDrawCallback, _1));    
}

void on_surface_destroyed_callback (JNIEnv& env, jobject view)
{
  push_message (view, xtl::bind (&window_handle::OnSurfaceDestroyedCallback, _1));
}

}

/*
    ��������/��������/����������� ����
*/

window_t AndroidWindowManager::CreateWindow (WindowStyle, WindowMessageHandler handler, const void* parent_handle, const char* init_string, void* user_data)
{
  try
  {
    if (!handler)
      throw xtl::make_null_argument_exception ("", "handler");
      
    stl::auto_ptr<window_handle> window (new window_handle);

    window->view            = JniWindowManagerSingleton::Instance ()->CreateView (init_string, window.get ());
    window->message_handler = handler;
    window->user_data       = user_data;

      //��������� �������
    
    JNIEnv& env = get_env ();

    local_ref<jclass> view_class = env.GetObjectClass (window->view.get ());
        
    if (!view_class)
      throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed (for View)");      
      
    window->get_left_method             = find_method (&env, view_class.get (), "getLeftThreadSafe", "()I");
    window->get_top_method              = find_method (&env, view_class.get (), "getTopThreadSafe", "()I");
    window->get_width_method            = find_method (&env, view_class.get (), "getWidthThreadSafe", "()I");
    window->get_height_method           = find_method (&env, view_class.get (), "getHeightThreadSafe", "()I");
    window->layout_method               = find_method (&env, view_class.get (), "layoutThreadSafe", "(IIII)V");
    window->set_visibility_method       = find_method (&env, view_class.get (), "setVisibilityThreadSafe", "(I)V");
    window->get_visibility_method       = find_method (&env, view_class.get (), "getVisibilityThreadSafe", "()I");
    window->request_focus_method        = find_method (&env, view_class.get (), "requestFocusThreadSafe", "()Z");
    window->bring_to_front_method       = find_method (&env, view_class.get (), "bringToFrontThreadSafe", "()V");
    window->set_background_color_method = find_method (&env, view_class.get (), "setBackgroundColorThreadSafe", "(I)V");
    window->maximize_method             = find_method (&env, view_class.get (), "maximizeThreadSafe", "()V");
    window->get_surface_method          = find_method (&env, view_class.get (), "getSurfaceThreadSafe", "()Landroid/view/Surface;");
    window->post_invalidate_method      = find_method (&env, view_class.get (), "postInvalidate", "()V");

      //��������� ����������� �����������
    
    local_ref<jobject> surface = check_errors (env.CallObjectMethod (window->view.get (), window->get_surface_method));

    if (!surface)
      throw xtl::format_operation_exception ("", "EngineView::getSurfaceThreadSafe failed");      

      //�������� �������� �����������
            
    for (;;)
    {
      if (window->is_native_handle_received)
      {
        window->window.native_window = ANativeWindow_fromSurface (&env, surface.get ());
        window->window.view          = window->view.get ();

        if (!window->window.native_window)
          throw xtl::format_operation_exception ("", "::ANativeWindow_fromSurface failed");

        break;
      }
      
      static const size_t WAIT_TIME_IN_MICROSECONDS = 100*1000; //100 milliseconds
      
      usleep (WAIT_TIME_IN_MICROSECONDS);
    }
  
    return window.release ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::CreateWindow");
    throw;
  }
}

void AndroidWindowManager::CloseWindow (window_t)
{
  throw xtl::make_not_implemented_exception ("syslib::AndroidWindowManager::CloseWindow");
}

void AndroidWindowManager::DestroyWindow (window_t)
{
  //TODO: release window handle

  throw xtl::make_not_implemented_exception ("syslib::AndroidWindowManager::DestroyWindow");
}

/*
    ��������� ���������-���������� ����������� ����
*/

const void* AndroidWindowManager::GetNativeWindowHandle (window_t window)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");
      
    return &window->window;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::GetNativeWindowHandle");
    throw;
  }
}

const void* AndroidWindowManager::GetNativeDisplayHandle (window_t)
{
  return 0;
}

/*
    ��������� ����
*/

void AndroidWindowManager::SetWindowTitle (window_t, const wchar_t*)
{
}

void AndroidWindowManager::GetWindowTitle (window_t, size_t size, wchar_t* buffer)
{
  if (!size)
    return;

  if (!buffer)
    throw xtl::make_null_argument_exception ("syslib::AndroidWindowManager::GetWindowTitle", "buffer");

  *buffer = L'\0';
}

/*
    ������� ���� / ���������� �������
*/

void AndroidWindowManager::SetWindowRect (window_t window, const Rect& rect)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");    
    
    JNIEnv& env = get_env ();
    
    env.CallVoidMethod (window->view.get (), window->layout_method, rect.left, rect.top, rect.right, rect.bottom);
    
    check_errors ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::SetWindowRect");
    throw;
  }
}

void AndroidWindowManager::SetClientRect (window_t window, const Rect& rect)
{
  try
  {
    SetWindowRect (window, rect);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::SetClientRect");
    throw;
  }
}

void AndroidWindowManager::GetWindowRect (window_t window, Rect& out_result)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");    
    
    Rect result;
    
    JNIEnv& env = get_env ();
    
    result.left   = check_errors (env.CallIntMethod (window->view.get (), window->get_left_method));
    result.top    = check_errors (env.CallIntMethod (window->view.get (), window->get_top_method));
    result.right  = result.left + check_errors (env.CallIntMethod (window->view.get (), window->get_width_method));
    result.bottom = result.top + check_errors (env.CallIntMethod (window->view.get (), window->get_height_method));
    
    out_result = result;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::GetWindowRect");
    throw;
  }
}

void AndroidWindowManager::GetClientRect (window_t window, Rect& rect)
{
  try
  {
    GetWindowRect (window, rect);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::GetClientRect");
    throw;
  }
}

/*
    ��������� ������ ����
*/

void AndroidWindowManager::SetWindowFlag (window_t window, WindowFlag flag, bool state)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");    
    
    JNIEnv& env = get_env ();    
    
    switch (flag)
    {
      case WindowFlag_Visible:
      {
        env.CallVoidMethod (window->view.get (), window->set_visibility_method, state ? VIEW_VISIBLE : VIEW_INVISIBLE);
        check_errors ();
        break;
      }
      case WindowFlag_Active:
        env.CallVoidMethod (window->view.get (), window->bring_to_front_method);
        check_errors ();
        break;
      case WindowFlag_Focus:
        if (!check_errors (env.CallBooleanMethod (window->view.get (), window->request_focus_method)))
          throw xtl::format_operation_exception ("", "EngineView::requestFocusThreadSafe failed");

        break;
      case WindowFlag_Maximized:
        env.CallVoidMethod (window->view.get (), window->maximize_method);        
        check_errors ();
        break;        
      case WindowFlag_Minimized:
      {
        env.CallVoidMethod (window->view.get (), window->set_visibility_method, VIEW_GONE);  
        check_errors ();      
        
        break;
      }
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
    }    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::SetWindowFlag");
    throw;
  }
}

bool AndroidWindowManager::GetWindowFlag (window_t window, WindowFlag flag)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");    
    
    JNIEnv& env = get_env ();    
    
    switch (flag)
    {
      case WindowFlag_Visible:
        return check_errors (env.CallIntMethod (window->view.get (), window->get_visibility_method)) != 0;
      case WindowFlag_Maximized:
      case WindowFlag_Active:
      case WindowFlag_Focus:
      case WindowFlag_Minimized:
        throw xtl::make_not_implemented_exception ("GetWindowFlag");
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
    }    
    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::GetWindowFlag");
    throw;
  }
}

/*
    ���������� ����
*/

void AndroidWindowManager::InvalidateWindow (window_t window)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");
      
    get_env ().CallVoidMethod (window->view.get (), window->post_invalidate_method);

    check_errors ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::InvalidateWindow");
    throw;
  }
}

/*
   ��������� multitouch ������ ��� ����
*/

void AndroidWindowManager::SetMultitouchEnabled (window_t window, bool enabled)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");

    window->is_multitouch_enabled = enabled;    //??????? filter touch events to keep only one touch if multitouch disabled
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::SetMultitouchEnabled");
    throw;
  }
}

bool AndroidWindowManager::IsMultitouchEnabled (window_t window)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");

    return window->is_multitouch_enabled;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::IsMultitouchEnabled");
    throw;
  }
}

/*
    ���� ����
*/

void AndroidWindowManager::SetBackgroundColor (window_t window, const Color& color)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");    
        
    unsigned int int_color = window->background_state + (unsigned int)color.red * 0x10000 + (unsigned int)color.green * 0x100 +
      color.blue;
                             
    get_env ().CallVoidMethod (window->view.get (), window->set_background_color_method, int_color);
    
    check_errors ();
    
    window->background_color = int_color & 0xffffffu;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::SetBackgroundColor");
    throw;
  }
}

void AndroidWindowManager::SetBackgroundState (window_t window, bool state)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");    

    window->background_state = state ? 0xff000000u : 0u;

    get_env ().CallVoidMethod (window->view.get (), window->set_background_color_method, window->background_color | window->background_state);

    check_errors ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::SetBackgroundState");
    throw;
  }
}

Color AndroidWindowManager::GetBackgroundColor (window_t window)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");        
    
    unsigned int int_color = window->background_color;
    
    Color result;
    
    result.blue  = int_color % 0x100;
    result.green = int_color / 0x100 % 0x100;
    result.red   = int_color / 0x10000 % 0x100;
    
    return result;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::GetBackgroundColor");
    throw;
  }
}

bool AndroidWindowManager::GetBackgroundState (window_t window)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");        
    
    return window->background_state != 0;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::GetBackgroundState");
    throw;
  }
}

namespace syslib
{

namespace android
{

/// ����������� ������� ��������� ������ ����
void register_window_callbacks (JNIEnv* env)
{
  try
  {
    if (!env)
      throw xtl::make_null_argument_exception ("", "env");
      
    jclass view_class = env->FindClass ("com/untgames/funner/application/EngineView");
    
    if (!view_class)
      throw xtl::format_operation_exception ("", "Can't find EngineView class\n");
    
    static const JNINativeMethod methods [] = {
      {"onLayoutCallback", "(IIII)V", (void*)&on_layout_callback},
      {"onDisplayHintCallback", "(I)V", (void*)&on_display_hint_callback},
      {"onDrawCallback", "()V", (void*)&on_draw_callback},
      {"onTouchCallback", "(IIFF)V", (void*)&on_touch_callback},
      {"onDoubletapCallback", "(IFF)V", (void*)&on_doubletap_callback},
      {"onTrackballCallback", "(IIFF)V", (void*)&on_trackball_callback},
      {"onKeyCallback", "(IIZZZ)V", (void*)&on_key_callback},
      {"onFocusCallback", "(Z)V", (void*)&on_focus_callback},
      {"onSurfaceCreatedCallback", "()V", (void*)&on_surface_created_callback},
      {"onSurfaceDestroyedCallback", "()V", (void*)&on_surface_destroyed_callback},
      {"onSurfaceChangedCallback", "(III)V", (void*)&on_surface_changed_callback},
    };

    static const size_t methods_count = sizeof (methods) / sizeof (*methods);

    jint status = env->RegisterNatives (view_class, methods, methods_count);

    if (status)
      throw xtl::format_operation_exception ("", "Can't register natives (status=%d)", status);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::android::register_window_callbacks");
    throw;
  }
}

}

}
