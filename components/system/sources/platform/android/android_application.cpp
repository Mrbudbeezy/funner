#include "shared.h"

using namespace syslib;

/*
    �������� �������� ����������
*/

IApplicationDelegate* Platform::CreateDefaultApplicationDelegate ()
{
  return create_message_queue_application_delegate ();
}

/*
   �������� URL �� ������� ��������
*/

void Platform::OpenUrl (const char* url)
{
  throw xtl::format_not_supported_exception ("syslib::AndroidPlatform::OpenUrl", "Open URL not supported");
}
