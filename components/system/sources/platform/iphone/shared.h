#ifndef SYSLIB_IPHONE_PLATFORM_SHARED_HEADER
#define SYSLIB_IPHONE_PLATFORM_SHARED_HEADER

#include <ctime>

#include <CFRunLoop.h>

#include <stl/algorithm>
#include <stl/vector>

#include <xtl/common_exceptions.h>

#include <common/log.h>

#include <syslib/platform/iphone.h>

#include <platform/platform.h>

namespace syslib
{

bool is_in_run_loop (); //������� �� ������� ����

}

#endif
