#include <process.h>

#include <stl/hash_map>
#include <stl/hash_set>

#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/component.h>
#include <common/file.h>
#include <common/log.h>
#include <common/property_map.h>
#include <common/singleton.h>
#include <common/strlib.h>
#include <common/xml_writer.h>

#include <syslib/application.h>

#include <shared/platform.h>

#define _WIN32_WINNT 0x0600 //��� ������������� WM_MOUSEWHEEL
#include <windows.h>
#include <Wtsapi32.h>

namespace syslib
{

//�������� ������ ������������� WinAPI � ��������� ���������� � ������ �� �������
void check_errors (const char* source);
void raise_error  (const char* source);

//������� tls ����
void cleanup_tls ();

}
