#include "shared.h"

using namespace syslib;

/*
    ������ � �������� ���������
*/

bool Platform::IsMessageQueueEmpty ()
{
  return true;
}

void Platform::DoNextEvent ()
{
}

void Platform::WaitMessage ()
{
}

void Platform::UpdateMessageQueue ()
{
}

/*
    ������ ����������
*/

void Platform::RunLoop (IRunLoopContext* context)
{
  if (!context)
    throw xtl::make_null_argument_exception ("syslib::DefaultPlatform::RunLoop", "context");
    
  context->DoCustomRunLoop ();
}

