#include "shared.h"

#import <syslib/platform/iphone.h>

#import <UIKit/UIApplication.h>
#import <UIKit/UIScreen.h>
#import <UIKit/UITouch.h>
#import <UIKit/UIViewController.h>
#import <UIKit/UIWindow.h>

#import <QuartzCore/CAEAGLLayer.h>

using namespace syslib;
using namespace syslib::iphone;

namespace
{

InterfaceOrientation get_interface_orientation (UIInterfaceOrientation interface_orientation)
{
  switch (interface_orientation)
  {
    case UIInterfaceOrientationPortrait:           return InterfaceOrientation_Portrait;
    case UIInterfaceOrientationPortraitUpsideDown: return InterfaceOrientation_PortraitUpsideDown;
    case UIInterfaceOrientationLandscapeLeft:      return InterfaceOrientation_LandscapeLeft;
    case UIInterfaceOrientationLandscapeRight:     return InterfaceOrientation_LandscapeRight;
    default:                                       return InterfaceOrientation_Unknown;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct WindowImpl
{
  void*                user_data;       //��������� �� ���������������� ������
  WindowMessageHandler message_handler; //������� ��������� ��������� ����
  window_t             cocoa_window;    //����

  //�����������/����������
  WindowImpl (WindowMessageHandler handler, void* in_user_data, void* new_window)
    : user_data (in_user_data), message_handler (handler), cocoa_window ((window_t)new_window)
    {}

  ~WindowImpl ()
  {
    [(UIWindow*)cocoa_window release];
  }

  //������� ���������
  void Notify (WindowEvent event, const WindowEventContext& context)
  {
    try
    {
      message_handler (cocoa_window, event, context, user_data);
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }
};

}

typedef stl::vector <IWindowListener*> ListenerArray;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
@interface UIWindowWrapper : UIWindow
{
  @private
    WindowImpl         *window_impl;          //����
    ListenerArray      *listeners;            //���������� �� �������
    WindowEventContext *event_context;        //��������, ������������ ������������ �������
    UIViewController   *root_view_controller; //�������� ����������
    int                allowed_orientations;  //����������� ���������� ����
    bool               has_ios_4_0;           //������ ������������ ������� >= 4.0
}

@property (nonatomic, assign) WindowImpl* window_impl;
@property (nonatomic)         int         allowed_orientations;

-(void)onPaint;

-(WindowEventContext&)getEventContext;

-(void)onInterfaceOrientationWillChangeFrom:(InterfaceOrientation)from_orientation to:(InterfaceOrientation)to_orientation duration:(float)duration;
-(void)onInterfaceOrientationChangedFrom:(InterfaceOrientation)from_orientation to:(InterfaceOrientation)to_orientation;

@end

/*
   �����, �������������� ������� ����� � ���������� �� ���������
*/

@interface UIViewWrapper : UIView
{
  @private
    Color   background_color;       //���� ����
    UIColor *ui_background_color;   //���� ����
    bool    background_state;       //��������� ��������� ����� ����
}

@property (nonatomic, assign) Color    background_color;
@property (nonatomic, retain) UIColor* ui_background_color;
@property (nonatomic, assign) bool     background_state;

@end

@implementation UIViewWrapper

@synthesize background_color, ui_background_color, background_state;

-(void)dealloc
{
  self.ui_background_color = nil;

  [super dealloc];
}

+(Class)layerClass
{
  return [CAEAGLLayer class];
}

-(BOOL)canBecomeFirstResponder
{
  return YES;
}

-(id)initWithFrame:(CGRect)rect
{
  self = [super initWithFrame:rect];

  if (!self)
    return nil;

  self.layer.delegate = self;

  return self;
}

-(void) displayLayer:(CALayer*)layer
{
  [(UIWindowWrapper*)self.window onPaint];
}

@end

/*
   �����, ���������� �� ���������� ����������� ����
*/

@interface UIViewControllerWrapper : UIViewController
{
}

@end

@implementation UIViewControllerWrapper

-(void)dealloc
{
  self.view = nil;

  [super dealloc];
}

-(void)loadView
{
  self.view = [[UIViewWrapper alloc] initWithFrame:[UIScreen mainScreen].applicationFrame];
  [self.view release];

  if ([[[UIDevice currentDevice] systemVersion] compare:@"4.0" options:NSNumericSearch] != NSOrderedAscending)
    self.view.contentScaleFactor = [UIScreen mainScreen].scale;
}

-(void)viewDidUnload
{
  [super viewDidUnload];

  self.view = nil;
}

-(BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interface_orientation
{
  InterfaceOrientation desired_orientation = get_interface_orientation (interface_orientation);

  return ((UIWindowWrapper*)self.view.window).allowed_orientations & desired_orientation;
}

-(void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)from_orientation
{
  [((UIWindowWrapper*)self.view.window) onInterfaceOrientationChangedFrom:get_interface_orientation (from_orientation) to:get_interface_orientation (self.interfaceOrientation)];
}

-(void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)to_orientation duration:(NSTimeInterval)duration
{
  [((UIWindowWrapper*)self.view.window) onInterfaceOrientationWillChangeFrom:get_interface_orientation (self.interfaceOrientation) to:get_interface_orientation (to_orientation) duration:duration];
}

@end

@implementation UIWindowWrapper

@synthesize window_impl, allowed_orientations;

-(void) dealloc
{
  [[NSNotificationCenter defaultCenter] removeObserver:self];

  delete listeners;
  delete event_context;

  [super dealloc];
}

-(CGFloat)contentScaleFactor
{
  if (has_ios_4_0)
    return super.contentScaleFactor;

  return 1;
}

-(UIViewController*)rootViewController
{
  return root_view_controller;
}

-(void)setRootViewController:(UIViewController*)in_root_view_controller
{
  if (root_view_controller == in_root_view_controller)
    return;

  [root_view_controller.view removeFromSuperview];

  [root_view_controller release];
  root_view_controller = [in_root_view_controller retain];

  [self addSubview:root_view_controller.view];

  [root_view_controller.view becomeFirstResponder];
}

-(id) initWithFrame:(CGRect)rect
{
  self = [super initWithFrame:rect];

  if (!self)
    return nil;

  allowed_orientations = InterfaceOrientation_Portrait;
  has_ios_4_0          = [[[UIDevice currentDevice] systemVersion] compare:@"4.0" options:NSNumericSearch] != NSOrderedAscending;

  try
  {
    event_context = new WindowEventContext;

    event_context->handle = self;

    listeners = new ListenerArray;
  }
  catch (...)
  {
    delete event_context;
    delete listeners;

    [self release];

    return nil;
  }

  self.rootViewController = [[UIViewControllerWrapper alloc] init];
  [self.rootViewController release];

  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector (onShow) name:UIWindowDidBecomeVisibleNotification object:self];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector (onHide) name:UIWindowDidBecomeHiddenNotification object:self];

  return self;
}

-(void)onPaint
{
  window_impl->Notify (WindowEvent_OnPaint, [self getEventContext]);
}

-(void)onShow
{
  window_impl->Notify (WindowEvent_OnShow, [self getEventContext]);
}

-(void)onHide
{
  window_impl->Notify (WindowEvent_OnHide, [self getEventContext]);
}

-(WindowEventContext&) getEventContext
{
  event_context->touches_count = 0;

  return *event_context;
}

-(WindowEventContext&)getEventContextWithTouches:(NSSet*)touches
{
  if ([touches count] > MAX_TOUCHES_COUNT)
    @throw [NSException exceptionWithName:@"Invalid operation" reason:@"Touches event occured with touches count more than maximum" userInfo:nil];

  WindowEventContext& return_value = [self getEventContext];

  return_value.touches_count = [touches count];

  UIView                 *view              = self.rootViewController.view;
  NSEnumerator           *enumerator        = [touches objectEnumerator];
  Touch                  *touch_description = return_value.touches;
  CGSize                 view_size          = view.frame.size;

  for (UITouch *iter = [enumerator nextObject]; iter; iter = [enumerator nextObject], touch_description++)
  {
    CGPoint current_location = [iter locationInView:view];

    touch_description->touch_id   = (size_t)iter;
    touch_description->position.x = current_location.x * [self contentScaleFactor];
    touch_description->position.y = current_location.y * [self contentScaleFactor];
  }

  return return_value;
}

-(void) touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
  window_impl->Notify (WindowEvent_OnTouchesBegan, [self getEventContextWithTouches:touches]);

  NSMutableSet* doubletap_touches = [[NSMutableSet alloc] initWithCapacity:[touches count]];

  for (UITouch* touch in touches)
  {
    if (!(touch.tapCount % 2))
      [doubletap_touches addObject:touch];
  }

  if ([doubletap_touches count])
    window_impl->Notify (WindowEvent_OnTouchesDoubletap, [self getEventContextWithTouches:doubletap_touches]);

  [doubletap_touches release];
}

-(void) touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
  window_impl->Notify (WindowEvent_OnTouchesEnded, [self getEventContextWithTouches:touches]);
}

-(void) touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
  window_impl->Notify (WindowEvent_OnTouchesMoved, [self getEventContextWithTouches:touches]);
}

-(void) touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
  [self touchesEnded:touches withEvent:event];
}

-(void) motionBegan:(UIEventSubtype)motion withEvent:(UIEvent*)event
{
  if (motion != UIEventSubtypeMotionShake)
    return;

  for (ListenerArray::iterator iter = listeners->begin (), end = listeners->end (); iter != end; ++iter)
    (*iter)->OnShakeMotionBegan ();
}

-(void) motionEnded:(UIEventSubtype)motion withEvent:(UIEvent*)event
{
  if (motion != UIEventSubtypeMotionShake)
    return;

  for (ListenerArray::iterator iter = listeners->begin (), end = listeners->end (); iter != end; ++iter)
    (*iter)->OnShakeMotionEnded ();
}

-(void) motionCancelled:(UIEventSubtype)motion withEvent:(UIEvent*)event
{
  [self motionEnded:motion withEvent:event];
}

-(void)onInterfaceOrientationWillChangeFrom:(InterfaceOrientation)from_orientation to:(InterfaceOrientation)to_orientation duration:(float)duration
{
  for (ListenerArray::iterator iter = listeners->begin (), end = listeners->end (); iter != end; ++iter)
    (*iter)->OnInterfaceOrientationWillChange (from_orientation, to_orientation, duration);
}

-(void)onInterfaceOrientationChangedFrom:(InterfaceOrientation)from_orientation to:(InterfaceOrientation)to_orientation
{
  for (ListenerArray::iterator iter = listeners->begin (), end = listeners->end (); iter != end; ++iter)
    (*iter)->OnInterfaceOrientationChanged (from_orientation, to_orientation);

  window_impl->Notify (WindowEvent_OnSize, [self getEventContext]);
}

/*
   ����������/�������� �����������
*/

-(void) attachListener:(IWindowListener*)listener
{
  listeners->push_back (listener);
}

-(void) detachListener:(IWindowListener*)listener
{
  listeners->erase (stl::remove (listeners->begin (), listeners->end (), listener), listeners->end ());
}

@end

/*
    ��������/��������/����������� ����
*/

window_t IPhoneWindowManager::CreateWindow (WindowStyle window_style, WindowMessageHandler handler, const void* parent_handle, const char* init_string, void* user_data)
{
  static const char* METHOD_NAME = "syslib::IPhoneWindowManager::CreateWindow";

  if (!is_in_run_loop ())
    throw xtl::format_operation_exception (METHOD_NAME, "Can't create window before entering run loop");

  if (parent_handle)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Parent windows not supported for iPhonePlatform");

    //�������� ����
  UIWindowWrapper* new_window = [[UIWindowWrapper alloc] initWithFrame:[UIScreen mainScreen].applicationFrame];

  if (!new_window)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't create window.");

  if ([[[UIDevice currentDevice] systemVersion] compare:@"4.0" options:NSNumericSearch] != NSOrderedAscending)
    new_window.contentScaleFactor = [UIScreen mainScreen].scale;

  new_window.rootViewController.view.clearsContextBeforeDrawing = NO;
  new_window.rootViewController.view.multipleTouchEnabled       = YES;

  WindowImpl* window_impl = new WindowImpl (handler, user_data, new_window);

  new_window.window_impl = window_impl;

  return (window_t)new_window;
}

void IPhoneWindowManager::CloseWindow (window_t handle)
{
  WindowImpl* window = ((UIWindowWrapper*)handle).window_impl;

  window->Notify (WindowEvent_OnClose, [(UIWindowWrapper*)handle getEventContext]);
}

void IPhoneWindowManager::DestroyWindow (window_t handle)
{
  WindowImpl* window = ((UIWindowWrapper*)handle).window_impl;

  window->Notify (WindowEvent_OnDestroy, [(UIWindowWrapper*)handle getEventContext]);

  delete window;
}

/*
    ��������� ���������-���������� ����������� ����
*/

const void* IPhoneWindowManager::GetNativeWindowHandle (window_t handle)
{
  return reinterpret_cast<const void*> (handle);
}

const void* IPhoneWindowManager::GetNativeDisplayHandle (window_t)
{
  return 0;
}

/*
    ��������� ����
*/

void IPhoneWindowManager::SetWindowTitle (window_t, const wchar_t*)
{
}

void IPhoneWindowManager::GetWindowTitle (window_t, size_t size, wchar_t* buffer)
{
  if (!size)
    return;

  if (!buffer)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::GetWindowTitle", "buffer");

  *buffer = L'\0';
}

/*
    ������� ���� / ���������� �������
*/

void IPhoneWindowManager::SetWindowRect (window_t window, const Rect& rect)
{
  if (!window)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::SetWindowRect", "window");

  UIWindowWrapper* wnd = (UIWindowWrapper*)window;

  float scale_factor = wnd.contentScaleFactor;

  CGRect frame;

  frame.size.width  = (rect.right - rect.left) / scale_factor;
  frame.size.height = (rect.bottom - rect.top) / scale_factor;
  frame.origin.x    = rect.left / scale_factor;
  frame.origin.y    = rect.top / scale_factor;

  UIInterfaceOrientation ui_orientation = wnd.rootViewController.interfaceOrientation;

  switch (ui_orientation)
  {
    case UIInterfaceOrientationLandscapeLeft:
    case UIInterfaceOrientationLandscapeRight:
    {
      float temp = frame.size.width;

      frame.size.width  = frame.size.height;
      frame.size.height = temp;

      temp = frame.origin.x;

      frame.origin.x = frame.origin.y;
      frame.origin.y = temp;

      break;
    }
    default:
      break;
  }

  wnd.frame = frame;

  WindowImpl* window_impl = wnd.window_impl;

  WindowEventContext& dummy_context = [wnd getEventContext];

  window_impl->Notify (WindowEvent_OnMove, dummy_context);
  window_impl->Notify (WindowEvent_OnSize, dummy_context);
}

void IPhoneWindowManager::SetClientRect (window_t handle, const Rect& rect)
{
  SetWindowRect (handle, rect);
}

void IPhoneWindowManager::GetWindowRect (window_t window, Rect& rect)
{
  if (!window)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::GetWindowRect", "window");

  UIWindow               *wnd           = (UIWindow*)window;
  UIInterfaceOrientation ui_orientation = wnd.rootViewController.interfaceOrientation;

  float scale_factor = wnd.contentScaleFactor;

  CGRect frame = wnd.frame;

  rect.bottom = (frame.origin.y + frame.size.height) * scale_factor;
  rect.right  = (frame.origin.x + frame.size.width) * scale_factor;
  rect.top    = frame.origin.y * scale_factor;
  rect.left   = frame.origin.x * scale_factor;

  switch (ui_orientation)
  {
    case UIInterfaceOrientationLandscapeLeft:
    case UIInterfaceOrientationLandscapeRight:
    {
      float temp = rect.bottom;

      rect.bottom = rect.right;
      rect.right  = temp;

      temp = rect.top;

      rect.top  = rect.left;
      rect.left = rect.top;

      break;
    }
    default:
      break;
  }
}

void IPhoneWindowManager::GetClientRect (window_t handle, Rect& target_rect)
{
  GetWindowRect (handle, target_rect);
}

/*
    ��������� ������ ����
*/

void IPhoneWindowManager::SetWindowFlag (window_t handle, WindowFlag flag, bool state)
{
  if (state == GetWindowFlag (handle, flag))
    return;

  UIWindowWrapper* wnd = (UIWindowWrapper*)handle;

  WindowImpl* window = wnd.window_impl;

  WindowEventContext& dummy_context = [(UIWindowWrapper*)handle getEventContext];

  try
  {
    switch (flag)
    {
      case WindowFlag_Visible: //��������� ����
        if (state)
        {
          [wnd makeKeyAndVisible];
          wnd.hidden = NO;

          window->Notify (WindowEvent_OnShow, dummy_context);
        }
        else
        {
          wnd.hidden = YES;

          window->Notify (WindowEvent_OnHide, dummy_context);
        }

        break;
      case WindowFlag_Active: //���������� ����
        if (state)
        {
          [wnd makeKeyAndVisible];

          window->Notify (WindowEvent_OnActivate, dummy_context);
        }
        else
          throw xtl::format_operation_exception ("", "Can't make window inactive");

        break;
      case WindowFlag_Focus: //����� �����
        if (state)
        {
          wnd.rootViewController.view.userInteractionEnabled = YES;

          window->Notify (WindowEvent_OnSetFocus, dummy_context);
        }
        else
        {
          wnd.rootViewController.view.userInteractionEnabled = NO;

          window->Notify (WindowEvent_OnLostFocus, dummy_context);
        }

        break;
      case WindowFlag_Maximized:
        wnd.bounds = [UIScreen mainScreen].applicationFrame;
        break;
      case WindowFlag_Minimized:
        throw xtl::format_operation_exception ("", "Can't minimize window");
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::IPhoneWindowManager::SetWindowFlag");
    throw;
  }
}

bool IPhoneWindowManager::GetWindowFlag (window_t handle, WindowFlag flag)
{
  UIWindow* wnd = (UIWindow*)handle;

  try
  {
    switch (flag)
    {
      case WindowFlag_Visible:
        return wnd.hidden == NO;
      case WindowFlag_Active:
        return wnd == [UIApplication sharedApplication].keyWindow;
      case WindowFlag_Focus:
        return wnd.rootViewController.view.userInteractionEnabled == YES;
      case WindowFlag_Maximized:
      {
        CGRect maximized_rect = [UIScreen mainScreen].applicationFrame;

        maximized_rect.origin.x = 0;
        maximized_rect.origin.y = 0;

        return CGRectEqualToRect (wnd.frame, maximized_rect);
      }
      case WindowFlag_Minimized:
        throw xtl::format_operation_exception ("", "Can't get window flag %d value", flag);
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::IPhoneWindowManager::GetWindowFlag");
    throw;
  }
}

/*
    ���������� ����
*/

void IPhoneWindowManager::InvalidateWindow (window_t handle)
{
  [((UIWindowWrapper*)handle).rootViewController.view setNeedsDisplay];
}

/*
   ��������� multitouch ������ ��� ����
*/

void IPhoneWindowManager::SetMultitouchEnabled (window_t window, bool enabled)
{
  if (!window)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::SetMultitouchEnabled", "window");

  ((UIWindowWrapper*)window).rootViewController.view.multipleTouchEnabled = enabled;
}

bool IPhoneWindowManager::IsMultitouchEnabled (window_t window)
{
  if (!window)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::IsMultitouchEnabled", "window");

  return ((UIWindowWrapper*)window).rootViewController.view.multipleTouchEnabled;
}

/*
   ���� ����
*/

void IPhoneWindowManager::SetBackgroundColor (window_t window, const Color& color)
{
  if (!window)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::SetBackgroundColor", "window");

  UIWindow*      wnd  = (UIWindow*)window;
  UIViewWrapper* view = (UIViewWrapper*)wnd.rootViewController.view;

  UIColor *background_color = [[UIColor alloc] initWithRed:color.red / 255.f green:color.green / 255.f blue:color.blue / 255.f alpha:1];

  [view setBackground_color:color];

  view.ui_background_color = background_color;

  if (view.background_state)
    view.backgroundColor = background_color;

  [background_color release];
}

void IPhoneWindowManager::SetBackgroundState (window_t window, bool state)
{
  if (!window)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::SetBackgroundState", "window");

  UIWindow*      wnd  = (UIWindow*)window;
  UIViewWrapper* view = (UIViewWrapper*)wnd.rootViewController.view;

  view.clearsContextBeforeDrawing = state;
  view.backgroundColor            = state ? view.ui_background_color : nil;
  view.background_state           = state;
}

Color IPhoneWindowManager::GetBackgroundColor (window_t window)
{
  if (!window)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::GetBackgroundColor", "window");

  UIWindow*      wnd  = (UIWindow*)window;
  UIViewWrapper *view = (UIViewWrapper*)wnd.rootViewController.view;

  return view.ui_background_color ? view.background_color : Color ();
}

bool IPhoneWindowManager::GetBackgroundState (window_t window)
{
  if (!window)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::GetBackgroundState", "window");

  return ((UIViewWrapper*)((UIWindow*)window).rootViewController.view).background_state;
}

/*
   �������� ����
*/

/*
   ����������/�������� �����������
*/

void WindowManager::AttachWindowListener (const Window& window, IWindowListener* listener)
{
  if  (!listener)
    return;

  if (!is_in_run_loop ())
    throw xtl::format_operation_exception ("syslib::iphone::attach_window_listener", "Can't attach window listener before entering run loop");

  [(UIWindowWrapper*)(window.Handle ()) attachListener:listener];
}

void WindowManager::DetachWindowListener (const Window& window, IWindowListener* listener)
{
  if (!is_in_run_loop ())
    return;

  [(UIWindowWrapper*)(window.Handle ()) detachListener:listener];
}

/*
   ���������/��������� ����������� ���������� ����
*/

void WindowManager::SetAllowedOrientations (const Window& window, int orientations)
{
  ((UIWindowWrapper*)window.Handle ()).allowed_orientations = orientations;
}

int WindowManager::GetAllowedOrientations (const Window& window)
{
  return ((UIWindowWrapper*)window.Handle ()).allowed_orientations;
}
