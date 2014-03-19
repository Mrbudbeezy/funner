#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSBundle.h>
#import <Foundation/NSFileManager.h>

#import <UIKit/UIApplication.h>
#import <UIKit/UIImage.h>
#import <UIKit/UIImageView.h>
#import <UIKit/UILocalNotification.h>
#import <UIKit/UIViewController.h>
#import <UIKit/UIScreen.h>
#import <UIKit/UIWindow.h>

#import "AdXTracking.h"

#include <cstdio>

#include <utility/utils.h>

#include <launcher/application.h>

using namespace engine;
using namespace plarium::launcher;

//AD-X constants
static NSString* PHONE_ADX_URL_SCHEME = @"ADX1144";
static NSString* PAD_ADX_URL_SCHEME   = @"ADX1145";
static NSString* PHONE_APPLE_ID       = @"543831789";
static NSString* PAD_APPLE_ID         = @"586574454";

@interface Startup : NSObject
{
  UIImageView* imageView;
  NSTimer*     fadeOutTimer;
}

-(void)onStartup;

@end

@implementation Startup

-(void)dealloc
{
  [imageView release];

  [[NSNotificationCenter defaultCenter] removeObserver:self];

  [super dealloc];
}

-(void)onStartup
{
  UIWindow* keyWindow     = [UIApplication sharedApplication].keyWindow;
  UIView*   keyWindowView = keyWindow.rootViewController.view;
  UIView*   parentView;

  NSString* imageName;

  if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
  {
    imageName  = [UIScreen mainScreen].scale > 1 ? @"Default_ipad-Landscape@2x.png" : @"Default_ipad-Landscape.png";
    parentView = keyWindowView;
  }
  else
  {
    imageName  = [UIScreen mainScreen].scale > 1 ? @"Default-568h@2x.png" : @"Default.png";
    parentView = keyWindow;
  }

  CGRect keyWindowFrame = parentView.bounds;

  if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad && keyWindowFrame.size.width < keyWindowFrame.size.height)
  {
    float t = keyWindowFrame.size.width;

    keyWindowFrame.size.width  = keyWindowFrame.size.height;
    keyWindowFrame.size.height = t;
  }

  [imageView removeFromSuperview];
  [imageView release];
  imageView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:imageName]];

  imageView.contentMode = UIViewContentModeScaleAspectFill;

  imageView.frame = keyWindowFrame;

  [parentView addSubview:imageView];

  [fadeOutTimer invalidate];
  [fadeOutTimer release];
  fadeOutTimer = [[NSTimer scheduledTimerWithTimeInterval:3 target:self selector:@selector (startFadeout) userInfo:nil repeats:NO] retain];
}

-(void)startFadeout
{
  [UIView beginAnimations:nil context:nil];
  [UIView setAnimationDuration:0.5];

  [UIView setAnimationDelegate:self];
  [UIView setAnimationDidStopSelector:@selector(onFadedOut)];

  imageView.alpha = 0;

  [UIView commitAnimations];
}

-(void)onFadedOut
{
  [imageView removeFromSuperview];
  [self release];
}

@end

@interface TrackingWrapper : NSObject
{
  AdXTracking* tracker;
}

@end

@implementation TrackingWrapper

-(void)dealloc
{
  [tracker release];

  [[NSNotificationCenter defaultCenter] removeObserver:self];

  [super dealloc];
}

-(void)reportAppOpen
{
  if (!tracker)
  {
    tracker = [[AdXTracking alloc] init];
    [tracker setURLScheme:UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ? PAD_ADX_URL_SCHEME : PHONE_ADX_URL_SCHEME];
    [tracker setClientId:@"PLR7hjus768DP"];
    [tracker setAppleId:UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ? PAD_APPLE_ID : PHONE_APPLE_ID];
  }

  [NSTimer scheduledTimerWithTimeInterval:0.5 target:tracker selector:@selector (reportAppOpen) userInfo:nil repeats:NO];
}

-(void)handleOpenURL:(const char*)url
{
  [tracker handleOpenURL:[NSURL URLWithString:[NSString stringWithUTF8String:url]]];
}

@end

@interface SystemAlertDelegate : NSObject
{
  @private
    sgi_stl::string  tag;
    engine::IEngine* engine;
}

@end

@implementation SystemAlertDelegate

-(id)initWithTag:(sgi_stl::string&)inTag engine:(engine::IEngine*)inEngine
{
  self = [super init];

  if (!self)
    return nil;

  tag    = inTag;
  engine = inEngine;

  return self;
}

-(void)alertView:(UIAlertView*)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
  engine->PostNotification (plarium::utility::format ("SystemAlertButtonClicked %s %d", tag.c_str (), buttonIndex).c_str ());

  [self release];
}

@end

namespace
{

class OpenUrlHandler : public IOpenUrlHandler
{
  public:
    OpenUrlHandler (TrackingWrapper* in_tracker)
      : tracker (in_tracker)
      {}

    void HandleUrlOpen (const char* url)
    {
      [tracker handleOpenURL:url];
    }

  private:
    OpenUrlHandler (const OpenUrlHandler&);             //no impl
    OpenUrlHandler& operator = (const OpenUrlHandler&); //no impl

  private:
    TrackingWrapper* tracker;
};

class SystemAlertHandler : public INotificationListener
{
  private:
    static const char* SYSTEM_ALERT_NOTIFICATION_PREFIX;

  public:
    ///Constructor/destructor
    SystemAlertHandler (engine::IEngine* in_engine)
      : engine (in_engine)
    {
      engine->AttachNotificationListener (plarium::utility::format ("%s*", SYSTEM_ALERT_NOTIFICATION_PREFIX).c_str (), this);
    }

    ~SystemAlertHandler ()
    {
      engine->DetachNotificationListener (this);
    }

    ///Notification handler
    void OnNotification (const char* notification)
    {
      notification += strlen (SYSTEM_ALERT_NOTIFICATION_PREFIX);

      sgi_stl::vector<sgi_stl::string> components = plarium::utility::split (notification, "|");

      if (components.size () < 4)
        throw sgi_stl::invalid_argument (plarium::utility::format ("SystemAlertHandler::OnNotification: invalid 'SystemAlert' arguments '%s'", notification));

      for (size_t i = 0, count = components.size (); i < count; i++)
        if (components [i] == " ")
          components [i] = "";

      SystemAlertDelegate* alert_delegate = [[SystemAlertDelegate alloc] initWithTag:components [0] engine:engine];

      UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:[NSString stringWithUTF8String:components [1].c_str ()]
                                                          message:[NSString stringWithUTF8String:components [2].c_str ()]
                                                         delegate:alert_delegate
                                                cancelButtonTitle:[NSString stringWithUTF8String:components [3].c_str ()]
                                                otherButtonTitles:nil];

      for (size_t i = 4, count = components.size (); i < count; i++)
        [alertView addButtonWithTitle:[NSString stringWithUTF8String:components [i].c_str ()]];

      [alertView show];
      [alertView release];
    }

  private:
    SystemAlertHandler (const SystemAlertHandler&);             //no impl
    SystemAlertHandler& operator = (const SystemAlertHandler&); //no impl

  private:
    engine::IEngine *engine;
};

const char* SystemAlertHandler::SYSTEM_ALERT_NOTIFICATION_PREFIX = "ShowSystemAlert ";

class LocalNotificationsHandler : public INotificationListener
{
  private:
    static const char* LOCAL_NOTIFICATIONS_PREFIX;

  public:
    ///Constructor/destructor
    LocalNotificationsHandler (engine::IEngine* in_engine)
      : engine (in_engine)
    {
      engine->AttachNotificationListener (plarium::utility::format ("%s*", LOCAL_NOTIFICATIONS_PREFIX).c_str (), this);
    }

    ~LocalNotificationsHandler ()
    {
      engine->DetachNotificationListener (this);
    }

    ///Notification handler
    void OnNotification (const char* notification)
    {
      notification += strlen (LOCAL_NOTIFICATIONS_PREFIX);

      sgi_stl::vector<sgi_stl::string> components = plarium::utility::split (notification, "|");

      if (components.empty ())
        throw sgi_stl::invalid_argument (plarium::utility::format ("LocalNotificationsHandler::OnNotification: invalid 'LocalNotifications' arguments '%s'", notification));

      for (size_t i = 0, count = components.size (); i < count; i++)
        if (components [i] == " ")
          components [i] = "";

      if (components [0] == "RemoveAll")
      {
        [[UIApplication sharedApplication] cancelAllLocalNotifications];
      }
      else if (components [0] == "Schedule")
      {
        if (components.size () < 3)
          throw sgi_stl::invalid_argument (plarium::utility::format ("LocalNotificationsHandler::OnNotification: invalid 'LocalNotifications' arguments '%s'", notification));

        NSString* alertBody = nil;

        if (!components [1].empty ())
          alertBody = [NSString stringWithUTF8String:components [1].c_str ()];

        for (size_t i = 2; i < components.size (); i++)
        {
          UILocalNotification* localNotification = [[UILocalNotification alloc] init];

          localNotification.alertBody                  = alertBody;
          localNotification.applicationIconBadgeNumber = 1;
          localNotification.soundName                  = UILocalNotificationDefaultSoundName;
          localNotification.fireDate                   = [NSDate dateWithTimeIntervalSinceNow:atoi (components [i].c_str ())];

          [[UIApplication sharedApplication] scheduleLocalNotification:localNotification];

          [localNotification release];
        }
      }
      else
        throw sgi_stl::invalid_argument (plarium::utility::format ("LocalNotificationsHandler::OnNotification: invalid 'LocalNotifications' first argument '%s'", components [0].c_str ()));
    }

  private:
    LocalNotificationsHandler (const LocalNotificationsHandler&);             //no impl
    LocalNotificationsHandler& operator = (const LocalNotificationsHandler&); //no impl

  private:
    engine::IEngine *engine;
};

const char* LocalNotificationsHandler::LOCAL_NOTIFICATIONS_PREFIX = "LocalNotifications ";

}

//����� �����
int main (int argc, const char* argv [], const char* env [])
{
  NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

  NSBundle* main_bundle = [NSBundle mainBundle];

  if (!main_bundle)
  {
      printf ("Can't locate main bundle\n");
      return 1;
  }

  NSString* resources_path = [main_bundle resourcePath];

  if (![[NSFileManager defaultManager] changeCurrentDirectoryPath:resources_path])
  {
    NSLog (@"Can't set current dir '%@'\n", resources_path);
    [pool release];
    return 1;
  }

  [pool release];

  IEngine* funner = FunnerInit ();

  if (!funner)
  {
      printf ("Funner startup failed!");
      return 1;
  }

  if (!funner->ParseCommandLine (argc, argv, env))
      return 1;

  {
    pool = [[NSAutoreleasePool alloc] init];

    Startup* startup = [[Startup alloc] init];

    TrackingWrapper* tracking = [[TrackingWrapper alloc] init];

    [[NSNotificationCenter defaultCenter] addObserver:startup selector:@selector (onStartup) name:UIApplicationDidFinishLaunchingNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:tracking selector:@selector (reportAppOpen) name:UIApplicationDidFinishLaunchingNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:tracking selector:@selector (reportAppOpen) name:UIApplicationWillEnterForegroundNotification object:nil];

    SystemAlertHandler         system_alert_handler (funner);
    LocalNotificationsHandler  local_notifications_handler (funner);

    OpenUrlHandler open_url_handler (tracking);

    Application application;

    application.SetOpenUrlHandler (&open_url_handler);

    application.Run (funner);

    [pool release];
  }

  delete funner;
}
