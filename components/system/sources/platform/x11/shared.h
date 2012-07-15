#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#undef DestroyAll
#undef DisplayString
#undef DisableScreenSaver

#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/string.h>

#include <common/log.h>
#include <common/property_map.h>
#include <common/singleton.h>
#include <common/strlib.h>
#include <common/time.h>
#include <common/utf_converter.h>

#include <media/image.h>

#include <syslib/thread.h>
#include <syslib/platform/x11.h>

#include <shared/platform.h>
#include <shared/message_queue.h>
