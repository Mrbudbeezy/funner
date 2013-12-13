#include <ctime>
#include <cstdio>

#include <unistd.h>
#include <errno.h>

#include <android/input.h>
#include <android/keycodes.h>
#include <android/looper.h>
#include <android/log.h>
#include <android/native_window_jni.h>

#include <stl/auto_ptr.h>
#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/ref.h>
#include <xtl/reference_counter.h>
#include <xtl/trackable_ptr.h>

#include <common/component.h>
#include <common/file.h>
#include <common/log.h>
#include <common/property_map.h>
#include <common/singleton.h>
#include <common/strlib.h>
#include <common/time.h>

#include <syslib/keydefs.h>
#include <syslib/thread.h>

#include <syslib/platform/android.h>

#include <shared/platform.h>
#include <shared/message_queue.h>

namespace syslib
{

namespace android
{

/*
    ���������� ��������
*/

///�������� ������� ����������
struct ApplicationContext
{
  JavaVM*             vm;                          //����������� ������
  global_ref<jclass>  utils_class;                 //EngineUtils class
  global_ref<jclass>  sensor_event_listener_class; //EngineSensorEventListener class
  
  ApplicationContext () : vm (0) {}
};

/// ����� ����� � ����������
void start_application (JavaVM* vm, jobject activity, const char* program_name, const char* args, const char* env_vars);

/// ����������� ������� ��������� ������ ����
void register_window_callbacks (JNIEnv* env);

/// ����������� ������� ��������� ������ web-view
void register_web_view_callbacks (JNIEnv* env);

/// ����������� ������� ��������� ������ ��������� ��������
void register_sensor_manager_callbacks (JNIEnv* env);

/// ����������� ������� ��������� ������ activity
void register_activity_callbacks (JNIEnv* env, jclass activity_class);

/// ����������� ���������� ���������� ����������
void register_crash_handlers ();

/// ����������� ������� ��������� ������ screen manager
void register_screen_callbacks (JNIEnv* env, jclass activity_class);

/// ����������� ������� ��������� ������ google cloud messaging
void register_gcm_callbacks (JNIEnv* env);

/// ��������� ��������� ������� ����������
const ApplicationContext& get_context ();

/// ��������� activity
void set_activity (jobject activity);

void startStdioRedirection    (JavaVM*);
void shutdownStdioRedirection ();

}

}
