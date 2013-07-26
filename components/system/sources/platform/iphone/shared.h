#ifndef SYSLIB_IPHONE_PLATFORM_SHARED_HEADER
#define SYSLIB_IPHONE_PLATFORM_SHARED_HEADER

#include <cfloat>
#include <ctime>

#include <CoreFoundation/CFRunLoop.h>

#include <stl/algorithm>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/connection.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>

#include <common/component.h>
#include <common/file.h>
#include <common/log.h>
#include <common/property_map.h>
#include <common/strlib.h>
#include <common/xml_writer.h>

#include <syslib/application.h>
#include <syslib/application_delegate.h>
#include <syslib/timer.h>

#include <syslib/platform/iphone.h>

#include <shared/platform.h>

#import <Foundation/NSObject.h>

#import <UIKit/UIAccelerometer.h>
#import <UIKit/UIApplication.h>
#import <UIKit/UIScreen.h>
#import <UIKit/UIScreenMode.h>
#import <UIKit/UIWindow.h>

namespace syslib
{

namespace iphone
{

bool is_in_run_loop (); //������� �� ������� ����

typedef stl::vector <IWindowListener*> WindowListenerArray;

//forward declarations
struct WindowImpl;

}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
@class ApplicationDelegateInternal;

@interface ApplicationDelegate : NSObject
{
  @private
    ApplicationDelegateInternal* impl;
}

-(void)setMainViewVisible:(bool)state;
-(bool)isMainViewVisible;

@end

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
@interface UIWindowWrapper : UIWindow
{
  @private
    syslib::iphone::WindowListenerArray *listeners;            //���������� �� �������
    syslib::iphone::WindowImpl          *window_impl;          //����
    syslib::WindowEventContext          *event_context;        //��������, ������������ ������������ �������
    UIViewController                    *root_view_controller; //�������� ����������
    int                                 allowed_orientations;  //����������� ���������� ����
    bool                                has_ios_4_0;           //������ ������������ ������� >= 4.0
}

-(void)onCharInput:(wchar_t)char_code;

@end

#endif
