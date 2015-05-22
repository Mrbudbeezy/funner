#ifdef __APPLE__
  #include <mach/clock.h>
  #include <mach/mach.h>
#endif

#include <limits.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

#ifdef _WIN32_WCE
#include <implement.h>
#endif

#include <stl/auto_ptr.h>

#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>

#include <common/strlib.h>

#include <syslib/thread.h>

#include <shared/platform.h>

namespace syslib
{

struct mutex_handle
{
  pthread_mutex_t mutex;
};

//������� �᪫�祭�� � ����� �訡��
void pthread_raise_error (const char* source, int status);

#ifdef _WIN32

void thread_init ();      //�㭪�� ���樠����樨 ������⥪�
void thread_done (void*); //�㭪�� �����樠����樨 ������⥪�

#else

inline void thread_init () {}
inline void thread_done (void*) {}

#endif

}
