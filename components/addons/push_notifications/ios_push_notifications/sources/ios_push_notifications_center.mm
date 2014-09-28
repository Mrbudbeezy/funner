#include "shared.h"

#import <UIKit/UIApplication.h>
#import <UIKit/UIUserNotificationSettings.h>

using namespace push_notifications;
using namespace push_notifications::ios_push_notifications;

namespace
{

const char* LOG_NAME                                    = "push_notifications.ios_push_notifications.IOSCenter"; //��� ���������
const char* CENTER_DESCRIPTION                          = "IOSCenter";                                           //��� ������
const char* NOTIFICATION_RECEIVED_PREFIX                = "PushNotificationReceived ";                           //������� ��������� � ����� ���-���������
const char* REGISTER_FOR_NOTIFICATIONS_SUCCEEDED_PREFIX = "RegisterForPushNotification Succeeded ";              //������� ��������� � �������� ����������� �� ���-���������
const char* REGISTER_FOR_NOTIFICATIONS_FAILED_PREFIX    = "RegisterForPushNotification Failed ";                 //������� ��������� � ������ ����������� �� ���-���������

//����� ��������� ���-���������
class IOSCenter
{
  public:
    ///�����������
    IOSCenter ()
      : log (LOG_NAME)
      , is_registering_for_notifications (false)
    {
      try
      {
        register_for_notifications_succeeded_connection = syslib::Application::RegisterNotificationHandler (common::format ("%s*", REGISTER_FOR_NOTIFICATIONS_SUCCEEDED_PREFIX).c_str (),
                                                                                               xtl::bind (&IOSCenter::OnRegisterForNotificationsSucceeded, this, _1));
        register_for_notifications_failed_connection = syslib::Application::RegisterNotificationHandler (common::format ("%s*", REGISTER_FOR_NOTIFICATIONS_FAILED_PREFIX).c_str (),
                                                                                               xtl::bind (&IOSCenter::OnRegisterForNotificationsFailed, this, _1));
        notification_received_connection = syslib::Application::RegisterNotificationHandler (common::format ("%s*", NOTIFICATION_RECEIVED_PREFIX).c_str (),
                                                                                            xtl::bind (&IOSCenter::OnNotificationReceived, this, _1));
      }
      catch (xtl::exception& e)
      {
        e.touch ("push_notifications::IOSCenter::IOSCenter");
        throw;
      }
    }

    ///����������� �� ���-���������
    void RegisterForNotifications (const PushNotificationsCenter::RegisterCallback& callback, const common::PropertyMap& properties)
    {
      try
      {
        if (is_registering_for_notifications)
          throw xtl::format_operation_exception ("", "Previous registration attempt is not finished yet");

        is_registering_for_notifications    = true;
        register_for_notifications_callback = callback;

        if ([[[UIDevice currentDevice] systemVersion] compare:@"8.0" options:NSNumericSearch] != NSOrderedAscending)
        {
          UIUserNotificationType types = UIUserNotificationTypeBadge | UIUserNotificationTypeSound | UIUserNotificationTypeAlert;

          if (properties.IsPresent ("Types"))
          {
            common::StringArray params_types = common::split (properties.GetString ("Types"));

            types = 0;

            for (size_t i = 0, count = params_types.Size (); i < count; i++)
            {
              const char* type = params_types [i];

              if (!xtl::xstrcmp (type, "Badge"))
                types |= UIUserNotificationTypeBadge;
              else if (!xtl::xstrcmp (type, "Sound"))
                types |= UIUserNotificationTypeSound;
              else if (!xtl::xstrcmp (type, "Alert"))
                types |= UIUserNotificationTypeAlert;
              else
                log.Printf ("Ignored unknown push notification type '%s'", type);
            }
          }

          NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

          UIUserNotificationSettings* notification_settings = [UIUserNotificationSettings settingsForTypes:types categories:[NSSet set]];

          [[UIApplication sharedApplication] registerUserNotificationSettings:notification_settings];
          [[UIApplication sharedApplication] registerForRemoteNotifications];

          [pool release];
        }
        else
        {
          UIRemoteNotificationType types = UIRemoteNotificationTypeBadge | UIRemoteNotificationTypeSound | UIRemoteNotificationTypeAlert;

          if (properties.IsPresent ("Types"))
          {
            common::StringArray params_types = common::split (properties.GetString ("Types"));

            types = 0;

            for (size_t i = 0, count = params_types.Size (); i < count; i++)
            {
              const char* type = params_types [i];

              if (!xtl::xstrcmp (type, "Badge"))
                types |= UIRemoteNotificationTypeBadge;
              else if (!xtl::xstrcmp (type, "Sound"))
                types |= UIRemoteNotificationTypeSound;
              else if (!xtl::xstrcmp (type, "Alert"))
                types |= UIRemoteNotificationTypeAlert;
              else
                log.Printf ("Ignored unknown push notification type '%s'", type);
            }
          }

          NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

          [[UIApplication sharedApplication] registerForRemoteNotificationTypes:types];

          [pool release];
        }
      }
      catch (xtl::exception& e)
      {
        is_registering_for_notifications    = false;
        register_for_notifications_callback = PushNotificationsCenter::RegisterCallback ();

        e.touch ("push_notifications::IOSCenter::RegisterForNotifications");
        throw;
      }
    }

    void UnregisterForNotifications ()
    {
      NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

      [[UIApplication sharedApplication] unregisterForRemoteNotifications];

      [pool release];
    }

    ///�������� �� ���-���������
    xtl::connection RegisterNotificationsHandler (const PushNotificationsCenter::NotificationsHandler& handler)
    {
      return notifications_signal.connect (handler);
    }

  private:
    ///��������� ���-���������
    void OnNotificationReceived (const char* notification)
    {
      notifications_signal (notification + xtl::xstrlen (NOTIFICATION_RECEIVED_PREFIX));
    }

    ///��������� ���������� ����������� �� ���-���������
    void OnRegisterForNotificationsSucceeded (const char* notification)
    {
      is_registering_for_notifications = false;

      register_for_notifications_callback (true, "", notification + xtl::xstrlen (REGISTER_FOR_NOTIFICATIONS_SUCCEEDED_PREFIX));
      register_for_notifications_callback = PushNotificationsCenter::RegisterCallback ();
    }

    void OnRegisterForNotificationsFailed (const char* notification)
    {
      is_registering_for_notifications = false;

      register_for_notifications_callback (false, notification + xtl::xstrlen (REGISTER_FOR_NOTIFICATIONS_FAILED_PREFIX), "");
      register_for_notifications_callback = PushNotificationsCenter::RegisterCallback ();
    }

  private:
    typedef xtl::signal<void (const char*)> NotificationsSignal;

  private:
    common::Log                               log;                                             //��������
    NotificationsSignal                       notifications_signal;                            //������ ���������� � ����� ����������
    bool                                      is_registering_for_notifications;                //������� �� � ������ ������ ������� ����������� �� ���-���������
    PushNotificationsCenter::RegisterCallback register_for_notifications_callback;             //������ ���������� ����������� �� ���-���������
    xtl::auto_connection                      register_for_notifications_succeeded_connection; //���������� ������� � �������� ����������� �� ���-���������
    xtl::auto_connection                      register_for_notifications_failed_connection;    //���������� ������� � ������ ����������� �� ���-���������
    xtl::auto_connection                      notification_received_connection;                //���������� ������� � ��������� ���-���������
};

typedef common::Singleton<IOSCenter> IOSCenterSingleton;

}

/*
   ����������� / ����������
*/

PushNotificationsCenterImpl::PushNotificationsCenterImpl ()
{
}

PushNotificationsCenterImpl::~PushNotificationsCenterImpl ()
{
}

/*
   ��������
*/

const char* PushNotificationsCenterImpl::Description ()
{
  return CENTER_DESCRIPTION;
}

/*
   ����������� �� ���-���������
*/

void PushNotificationsCenterImpl::RegisterForNotifications (const PushNotificationsCenter::RegisterCallback& callback, const common::PropertyMap& properties)
{
  IOSCenterSingleton::Instance ()->RegisterForNotifications (callback, properties);
}

void PushNotificationsCenterImpl::UnregisterForNotifications ()
{
  IOSCenterSingleton::Instance ()->UnregisterForNotifications ();
}

/*
   �������� �� ���-���������
*/

xtl::connection PushNotificationsCenterImpl::RegisterNotificationsHandler (const PushNotificationsCenter::NotificationsHandler& handler)
{
  return IOSCenterSingleton::Instance ()->RegisterNotificationsHandler (handler);
}
