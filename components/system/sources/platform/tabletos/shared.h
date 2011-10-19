#include <ctime>

#include <errno.h>

#include <bps/bps.h>
#include <screen/screen.h>

#include <xtl/common_exceptions.h>
#include <xtl/reference_counter.h>

#include <common/singleton.h>
#include <common/time.h>

#include <shared/platform.h>

namespace syslib
{

namespace tabletos
{

//������������� ���������
void platform_initialize ();

//�������� ������ ������������� BPS � ��������� ���������� � ������ �� �������
void check_errors (const char* source);
void raise_error  (const char* source);

}

}
