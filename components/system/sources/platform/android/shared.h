#include <ctime>
#include <cstdio>

#include <jni.h>
#include <android/looper.h>

#include <stl/auto_ptr.h>

#include <xtl/type_traits>
#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/singleton.h>
#include <common/strlib.h>

#include <syslib/thread.h>

#include <platform/platform.h>

namespace syslib
{

namespace android
{

/// ��������� ����������� ������
JavaVM* get_vm ();

/// ��������� ��������� ������� ����
JNIEnv& get_env ();

/*
    JNI ������
*/

///��������� �������� ��������� JNI �������
template <class T> struct jni_local_ref_strategy
{
  static T*   clone   (T* ptr) { get_env ().NewLocalRef (ptr); return ptr; }
  static void release (T* ptr) { get_env ().DeleteLocalRef (ptr); }  
};

///��������� �������� ���������� JNI �������
template <class T> struct jni_global_ref_strategy
{
  static T*   clone   (T* ptr) { get_env ().NewGlobalRef (ptr); return ptr; }
  static void release (T* ptr) { get_env ().DeleteGlobalRef (ptr); }  
};

template <class T> class global_ref;

/// ��������� JNI ������
template <class T> class local_ref: public xtl::intrusive_ptr<typename xtl::type_traits::remove_pointer<T>::type, jni_local_ref_strategy>
{
  typedef xtl::intrusive_ptr<typename xtl::type_traits::remove_pointer<T>::type, jni_local_ref_strategy> base;
  public:
    typedef typename base::element_type element_type;

    local_ref () {}
    local_ref (element_type* ptr, bool addref = true) : base (ptr, addref) {}
    local_ref (const local_ref& ref) : base (ref) {}

    template <class T1> local_ref (const local_ref<T1>& ref) : base (ref) {}    
    template <class T1> local_ref (const global_ref<T1>& ref) : base (&*ref) {}    
};

/// ���������� JNI ������
template <class T> class global_ref: public xtl::intrusive_ptr<typename xtl::type_traits::remove_pointer<T>::type, jni_global_ref_strategy>
{
  typedef xtl::intrusive_ptr<typename xtl::type_traits::remove_pointer<T>::type, jni_global_ref_strategy> base;
  public:
    typedef typename base::element_type element_type;

    global_ref () {}
    global_ref (element_type* ptr, bool addref = true) : base (ptr, addref) {}
    global_ref (const global_ref& ref) : base (ref) {}

    template <class T1> global_ref (const global_ref<T1>& ref) : base (ref) {}
    template <class T1> global_ref (const local_ref<T1>& ref) : base (&*ref) {}    
};

/*
    ����� �������
*/

inline jmethodID find_static_method (JNIEnv* env, jclass class_, const char* name, const char* signature)
{
  jmethodID result = env->GetStaticMethodID (class_, name, signature);
  
  if (!result)
    throw xtl::format_operation_exception ("JNIEnv::GetStaticMethodID", "Static method '%s %s' not found", name, signature);
    
  return result;
}

inline jmethodID find_method (JNIEnv* env, jclass class_, const char* name, const char* signature)
{
  jmethodID result = env->GetMethodID (class_, name, signature);
  
  if (!result)
    throw xtl::format_operation_exception ("JNIEnv::GetMethodID", "Method '%s %s' not found", name, signature);
    
  return result;
}

/*
    ������������ ������
*/

inline jstring tojstring (const char* s)
{
  if (!s)
    return 0;

  return get_env ().NewStringUTF (s);
}

/*
    ���������� ��������
*/

///�������� ������� ����������
struct ApplicationContext
{
  JavaVM*             vm;       //����������� ������
  global_ref<jobject> activity; //activity, ����������� ����������
  ALooper*            looper;   //������� ��������� ��������� ���������
};

/// ����� ����� � ����������
void start_application (JavaVM* vm, jobject activity, const char* program_name, const char* args);

/// ��������� ��������� ������� ����������
const ApplicationContext& get_context ();

}

}

