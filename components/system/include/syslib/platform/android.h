#ifndef SYSLIB_PLATFORM_ANDROID_HEADER
#define SYSLIB_PLATFORM_ANDROID_HEADER

#include <syslib/window.h>

///forward declarations
struct ANativeWindow;
class _jobject;

namespace syslib
{

namespace android
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct NativeWindow
{
  ANativeWindow* native_window; //��������� �� �������� ����
  _jobject*      view;          //��������� �� view
};

}

}

#endif
