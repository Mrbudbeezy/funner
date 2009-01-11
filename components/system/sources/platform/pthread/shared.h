#include <pthread.h>

#include <stl/auto_ptr.h>

#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>

#include <common/strlib.h>

#include <syslib/thread.h>

#include <platform/platform.h>

namespace syslib
{

#ifdef _WIN32

void thread_init ();      //�㭪�� ���樠����樨 ������⥪�
void thread_done (void*); //�㭪�� �����樠����樨 ������⥪�

#else

inline void thread_init () {}
inline void thread_done (void*) {}

#endif

//������� �᪫�祭�� � ����� �訡��
void pthread_raise_error (const char* source, int status);

}
