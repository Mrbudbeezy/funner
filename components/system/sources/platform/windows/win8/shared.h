#include "../shared.h"

#include <wrl/client.h>
#include <ppltasks.h>
#include <agile.h>

#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <locale.h>
#include <wrl/client.h>

#include <xtl/bind.h>
#include <xtl/utility>

#include <common/strconv.h>
#include <common/utf_converter.h>

#include <syslib/mutex.h>

namespace syslib
{

namespace win8
{

/// �������� ����������
interface class IApplicationContext
{
  /// ��������� �������� ����
  Windows::UI::Core::CoreWindow^ MainWindow ();
};

/// ��������� ��������� ����������
IApplicationContext^ get_context ();

/// �������������� �����
stl::string tostring (Platform::String^ input);

/// ������ ���������� � ���������� HRESULT
void raise_com_error (const char* method, HRESULT error);

/// �������������� ����� ������
Key get_key_code (Windows::System::VirtualKey key);

}

}
