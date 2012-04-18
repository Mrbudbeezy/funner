#include "shared.h"

using namespace syslib;
using namespace syslib::android;

namespace
{

/*
    ���������
*/

const char* ANDROID_SENSOR_MANAGER_CLASS_NAME = "android/hardware/SensorManager";
const char* ANDROID_LIST_CLASS_NAME           = "java/util/List";

/// ���� ��������
enum SensorType
{
    SENSOR_TYPE_ACCELEROMETER      = 1,
    SENSOR_TYPE_MAGNETIC_FIELD     = 2,
    SENSOR_TYPE_GYROSCOPE          = 4,
    SENSOR_TYPE_LIGHT              = 5,
    SENSOR_TYPE_PROXIMITY          = 8,
    SENSOR_TYPE_ALL                = -1
};

/*
    JNI ���� � ��������� ��������
*/

class JniSensorManager
{
  public:
///�����������
    JniSensorManager ()
    {
      try
      {         
          //���������� ����� ������� � �������
          
        JNIEnv& env = get_env ();

        local_ref<jobject> activity = get_activity ();
        
        if (!activity)
          throw xtl::format_operation_exception ("", "Null activity");
          
        local_ref<jclass> activity_class = env.GetObjectClass (activity.get ());
        
        if (!activity_class)
          throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed");
        
        sensor_manager_class = env.FindClass (ANDROID_SENSOR_MANAGER_CLASS_NAME);
      
        if (!sensor_manager_class)
          throw xtl::format_operation_exception ("", "Class %s not found in JNI environment", ANDROID_SENSOR_MANAGER_CLASS_NAME);
        
        get_sensor_list_method = find_method (&env, sensor_manager_class.get (), "getSensorList", "(I)Ljava/util/List;");
        
        list_class = env.FindClass (ANDROID_LIST_CLASS_NAME);
        
        if (!list_class)
          throw xtl::format_operation_exception ("", "Class %s not found in JNI environment", ANDROID_LIST_CLASS_NAME);
          
        get_list_size = find_method (&env, list_class.get (), "size", "()I");
        get_list_item = find_method (&env, list_class.get (), "get", "(I)Ljava/lang/Object;");
        
          //���������� ������ ��������
        
        AddSensors (SENSOR_TYPE_ALL);
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::android::JniSensorManager::JniSensorManager");
        throw;
      }      
    }
    
///���������� ��������
    size_t SensorsCount () { return sensors.size (); }

  private:
///���������� ��������
    void AddSensors (int type)
    {
      try
      {
        JNIEnv& env = get_env ();

        local_ref<jobject> activity = get_activity ();
        
        if (!activity)
          throw xtl::format_operation_exception ("", "Null activity");
          
        local_ref<jclass> activity_class = env.GetObjectClass (activity.get ());
        
        if (!activity_class)
          throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed");        

        jmethodID get_system_service_method = find_method (&env, activity_class.get (), "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");        
        
        static const char* ANDROID_SENSOR_MANAGER_SERVICE = "sensor";

        local_ref<jobject> sensor_manager = env.CallObjectMethod (activity.get (), get_system_service_method, tojstring (ANDROID_SENSOR_MANAGER_SERVICE).get ());
        
        if (!sensor_manager)
          throw xtl::format_operation_exception ("", "getSystemService('%s') failed", ANDROID_SENSOR_MANAGER_SERVICE);

        local_ref<jobject> sensors_list = env.CallObjectMethod (sensor_manager.get (), get_sensor_list_method, type);
        
        if (!sensors_list)
          throw xtl::format_operation_exception ("", "android.hardware.SensorManager.getSensorList failed");
          
        int items_count = env.CallIntMethod (sensors_list.get (), get_list_size);
        
        if (items_count <= 0)
          return;
          
        for (int i=0; i<items_count; i++)
        {
          local_ref<jobject> sensor = env.CallObjectMethod (sensors_list.get (), get_list_item, i);
          
          if (!sensor)
            continue;
            
          sensors.push_back (sensor);
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::android::JniSensorManager::AddSensors");
        throw;
      }
    }
    
  private:
    typedef global_ref<jobject>    SensorPtr;
    typedef stl::vector<SensorPtr> SensorArray;
      
  private:
    SensorArray        sensors;                //������ ��������
    global_ref<jclass> sensor_manager_class;   //����� ��������� ��������
    jmethodID          get_sensor_list_method; //����� ��������� ������ ��������
    global_ref<jclass> list_class;             //����� ������ �� �������
    jmethodID          get_list_size;          //����� ��������� ���������� ��������� � ������
    jmethodID          get_list_item;          //����� ��������� �������� ������
};

typedef common::Singleton<JniSensorManager> JniSensorManagerSingleton;

}

/*
    ���������� ��������
*/

size_t AndroidSensorManager::GetSensorsCount ()
{
  try
  {
    return JniSensorManagerSingleton::Instance ()->SensorsCount ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidSensorManager::GetSensorsCount");
    throw;
  }
}

/*
    �������� / �������� �������
*/

sensor_t AndroidSensorManager::CreateSensor (size_t sensor_index)
{
  throw xtl::make_not_implemented_exception ("syslib::AndroidSensorManager::CreateSensor");
}

void AndroidSensorManager::DestroySensor (sensor_t)
{
  throw xtl::make_not_implemented_exception ("syslib::AndroidSensorManager::DestroySensor");
}

/*
    ��� �������
*/

const char* AndroidSensorManager::GetSensorName (sensor_t)
{
  throw xtl::make_not_implemented_exception ("syslib::AndroidSensorManager::GetSensorName");
}

/*
    ������������� �������
*/

const char* AndroidSensorManager::GetSensorVendor (sensor_t)
{
  throw xtl::make_not_implemented_exception ("syslib::AndroidSensorManager::GetSensorVendor");
}

/*
    ��� ����������
*/

const char* AndroidSensorManager::GetSensorType (sensor_t)
{
  throw xtl::make_not_implemented_exception ("syslib::AndroidSensorManager::GetSensorType");
}

/*
    ������������ ��������
*/

float AndroidSensorManager::GetSensorMaxRange (sensor_t)
{
  throw xtl::make_not_implemented_exception ("syslib::AndroidSensorManager::GetSensorMaxRange");
}

/*
    ������� ����������
*/

void AndroidSensorManager::SetSensorUpdateRate (sensor_t, float rate)
{
  throw xtl::make_not_implemented_exception ("syslib::AndroidSensorManager::SetSensorUpdateRate");
}

float AndroidSensorManager::GetSensorUpdateRate (sensor_t)
{
  throw xtl::make_not_implemented_exception ("syslib::AndroidSensorManager::GetSensorUpdateRate");
}

/*
    ��������� ���������-���������� ����������� ������
*/

const void* AndroidSensorManager::GetNativeSensorHandle (sensor_t)
{
  throw xtl::make_not_implemented_exception ("syslib::AndroidSensorManager::GetNativeSensorHandle");
}

/*
    ��������� ���������-��������� ������� ������
*/

void AndroidSensorManager::GetSensorProperties (sensor_t, common::PropertyMap& properties)
{
  throw xtl::make_not_implemented_exception ("syslib::AndroidSensorManager::GetSensorProperties");
}

/*
    ������ ������� �������
*/

void AndroidSensorManager::StartSensorPolling (sensor_t)
{
  throw xtl::make_not_implemented_exception ("syslib::AndroidSensorManager::StartSensorPolling");
}

void AndroidSensorManager::StopSensorPolling (sensor_t)
{
  throw xtl::make_not_implemented_exception ("syslib::AndroidSensorManager::StopSensorPolling");
}

bool AndroidSensorManager::PollSensorEvent (sensor_t, SensorEvent&)
{
  return false;
}
