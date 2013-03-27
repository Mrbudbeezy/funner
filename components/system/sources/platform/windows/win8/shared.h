#include "../shared.h"

#include <wrl/client.h>
#include <ppltasks.h>

#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <locale.h>
#include <wrl/client.h>

#include <xtl/utility>

#include <common/strconv.h>
#include <common/utf_converter.h>

namespace syslib
{

/// ������������� WinRT
class WinRtInitializer: public xtl::noncopyable
{
  public:
    /// �����������
    WinRtInitializer ();

    /// ����������
    ~WinRtInitializer ();
};

/// ������������� �����
stl::string tostring (Platform::String^ input);

/// ������ ���������� � ���������� HRESULT
void raise_com_error (const char* method, HRESULT error);

}
