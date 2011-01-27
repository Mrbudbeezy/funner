#include "shared.h"

#import <UIKit/UIApplication.h>
#import <UIKit/UIGeometry.h>
#import <UIKit/UITextField.h>
#import <UIKit/UIWindow.h>

using namespace input::low_level::iphone_driver;

namespace
{

NSString* EMPTY_STRING_REPLACE = @" ";

const size_t MESSAGE_BUFFER_SIZE = 16;
const float  EPSILON             = 0.01f;

bool float_compare (float value1, float value2)
{
  return (value1 > (value2 - EPSILON)) && (value1 < (value2 + EPSILON));
}

//�����, ��������������� �� ���������� ������
class KeyboardListener
{
  public:
    //���������� ��� ���������� ������
    virtual void OnChar (unsigned short new_char) = 0;

    //���������� ��� ���������� ������
    virtual void KeyboardWasShown (float center_x, float center_y, float width, float height) = 0;

  protected:
    virtual ~KeyboardListener () {}
};

}

@interface TextFieldListener : NSObject <UITextFieldDelegate>
{
  @private
    UITextField*      text_field;
    KeyboardListener* listener;
    NSString*         last_text;            //�����, ������������ � ������ �� ����� ���������� ����������
}

@property (nonatomic, assign) KeyboardListener *listener;
@property (nonatomic, copy)   NSString         *last_text;

- (id)initWithTextField:(UITextField*)in_text_field listener:(KeyboardListener*)in_listener;

@end

@implementation TextFieldListener

@synthesize listener, last_text;

- (id)init
{
  return [self initWithTextField:0 listener:0];
}

- (id)initWithTextField:(UITextField*)in_text_field listener:(KeyboardListener*)in_listener
{
  self = [super init];

  if (!self)
    return nil;

  if (!in_text_field)
  {
    [self release];
    return nil;
  }

  text_field     = in_text_field;
  listener       = in_listener;

  text_field.text = EMPTY_STRING_REPLACE;             //������������� ������ � ���� ��� �����, ��� ��������� ������� backspace
  self.last_text  = text_field.text;

  NSNotificationCenter *notificationCenter = [NSNotificationCenter defaultCenter];

  [notificationCenter addObserver:self selector:@selector(keyboardWasShown:) name:UIKeyboardDidShowNotification object:nil];
  [notificationCenter addObserver:self selector:@selector(keyboardTextChanged) name:UITextFieldTextDidChangeNotification object:text_field];

  return self;
}

- (void)dealloc
{
  self.last_text = nil;

  [[NSNotificationCenter defaultCenter] removeObserver:self];

  [super dealloc];
}

- (void)keyboardWasShown:(NSNotification*)notification
{
  if (!listener)
    return;

  NSDictionary* keyboard_info   = [notification userInfo];
  CGSize        keyboard_size   = [[keyboard_info objectForKey:UIKeyboardBoundsUserInfoKey] CGRectValue].size;
  CGPoint       keyboard_center = [[keyboard_info objectForKey:UIKeyboardCenterEndUserInfoKey] CGPointValue];

  listener->KeyboardWasShown (keyboard_center.x, keyboard_center.y, keyboard_size.width, keyboard_size.height);
}

- (void)keyboardTextChanged
{
  size_t new_text_length  = [text_field.text length],
         last_text_length = [last_text length];

  if (new_text_length == last_text_length)
    return;

  if (listener)
  {
    if (last_text_length > new_text_length)   //Backspace
    {
      listener->OnChar (8);

      if (![text_field.text length])
      {
        self.last_text  = EMPTY_STRING_REPLACE;
        text_field.text = EMPTY_STRING_REPLACE;  //������������ ��������� ������ ������ �����������, ����� ������ ����� ���� ���������� ������� backspace
      }
    }
    else                                             //New char
      listener->OnChar ([text_field.text characterAtIndex:(new_text_length - 1)]);
  }

  self.last_text = text_field.text;
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
  if (listener)
    listener->OnChar (13);

  return NO;
}

@end

struct IPhoneKeyboard::Impl : public KeyboardListener
{
  typedef xtl::signal<void (const char*)> DeviceSignal;

  stl::string       name;                  //��� ����������
  stl::string       full_name;             //������ ��� ����������
  DeviceSignal      signals;               //����������� �������
  stl::wstring      control_name;          //��� ��������
  UITextField       *text_field;           //���, � ������� �������� ����������� ������ � �����������
  TextFieldListener *text_field_listener;  //��������� ��������� ���������� ����

  ///����������� / ����������
  Impl (const char* in_name, const char* in_full_name)
    : name (in_name), full_name (in_full_name), text_field (0), text_field_listener (0)
  {
    try
    {
      CGRect frame_rect;

      frame_rect.origin.x    = -2;
      frame_rect.origin.y    = -2;
      frame_rect.size.width  = 1;
      frame_rect.size.height = 1;

      text_field = [[UITextField alloc] initWithFrame:frame_rect];

      if (!text_field)
        throw xtl::format_operation_exception ("", "Can't create needed text field");

      [[UIApplication sharedApplication].keyWindow addSubview:text_field];

      text_field_listener = [[TextFieldListener alloc] initWithTextField:text_field listener:this];

      ShowKeyboard ();

      text_field.delegate = text_field_listener;
    }
    catch (xtl::exception& e)
    {
      [text_field release];
      [text_field_listener release];
      e.touch ("input::low_level::iphone_driver::IPhoneKeyboard::IPhoneKeyboard");
      throw;
    }
  }

  ~Impl ()
  {
    text_field.delegate = nil;

    [text_field_listener release];

    try
    {
      HideKeyboard ();
    }
    catch (...)
    {
      //���������� ����������
    }

    [text_field release];
  }

  void ShowKeyboard ()
  {
    if (![text_field becomeFirstResponder])
      throw xtl::format_operation_exception ("input::low_level::iphone_driver::IPhoneKeyboard::ShowKeyboard", "Can't show keyboard");
  }

  void HideKeyboard ()
  {
    if (![text_field resignFirstResponder])
      xtl::format_operation_exception ("input::low_level::iphone_driver::IPhoneKeyboard::HideKeyboard", "Can't hide keyboard");
  }

  void OnChar (unsigned short new_char)
  {
    static char message[MESSAGE_BUFFER_SIZE];

    xtl::xsnprintf (message, MESSAGE_BUFFER_SIZE, "CharCode %u", new_char);
    signals (message);
  }

  void KeyboardWasShown (float center_x, float center_y, float width, float height)
  {
    //TODO set this values as properties
  }
};

/*
   �����������/����������
*/

IPhoneKeyboard::IPhoneKeyboard (const char* name, const char* full_name)
  : impl (new Impl (name, full_name))
  {}

IPhoneKeyboard::~IPhoneKeyboard ()
{
  delete impl;
}

/*
   ��������� ����� ����������
*/

const char* IPhoneKeyboard::GetName ()
{
  return impl->name.c_str ();
}

/*
   ������ ��� ���������� (���.���.�������������)
*/

const char* IPhoneKeyboard::GetFullName ()
{
  return impl->full_name.c_str ();
}

/*
   ��������� ����� ��������
*/

const wchar_t* IPhoneKeyboard::GetControlName (const char* control_id)
{
  impl->control_name = common::towstring (control_id);

  return impl->control_name.c_str ();
}

/*
   �������� �� ������� ����������
*/

xtl::connection IPhoneKeyboard::RegisterEventHandler (const input::low_level::IDevice::EventHandler& handler)
{
  return impl->signals.connect (handler);
}

/*
   ��������� ����������
*/

const char* IPhoneKeyboard::GetProperties ()
{
  return "";
}

void IPhoneKeyboard::SetProperty (const char* name, float value)
{
  throw xtl::make_argument_exception ("input::low_level::iphone_driver::IPhoneKeyboard::SetProperty", "name", name);
}

float IPhoneKeyboard::GetProperty (const char* name)
{
  throw xtl::make_argument_exception ("input::low_level::iphone_driver::IPhoneKeyboard::GetProperty", "name", name);
}
