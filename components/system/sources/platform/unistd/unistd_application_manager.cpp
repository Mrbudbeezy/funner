#include <unistd.h>

#include <shared/message_queue.h>
#include <shared/platform.h>

using namespace syslib;

/*
    ������������ ���������� ���������� �� miliseconds ����������
*/

void UnistdApplicationManager::Sleep (size_t miliseconds)
{
  usleep (miliseconds * 1000);
}

/*
    �������� �������� ����������
*/

IApplicationDelegate* UnistdApplicationManager::CreateDefaultApplicationDelegate ()
{
  return create_message_queue_application_delegate ();
}
