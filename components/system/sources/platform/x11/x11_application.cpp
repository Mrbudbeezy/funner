#include "shared.h"

using namespace syslib;

/*
    �������� �������� ����������
*/

IApplicationDelegate* Platform::CreateDefaultApplicationDelegate ()
{
  return create_message_queue_application_delegate ();
}
