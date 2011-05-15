#include "shared.h"

using namespace sound::low_level::openal;

/*
   �������� ���������� OpenAL
*/

/*
   ����������� / ����������
*/

OpenALContextManagerImpl::OpenALContextManagerImpl ()
  : current_context (0)
  {}

OpenALContextManagerImpl::~OpenALContextManagerImpl ()
{
}

/*
   ��������� �������� ���������
*/

void OpenALContextManagerImpl::SetCurrentContext (OpenALContext& context)
{
  if (current_context == &context)
    return;

  context.MakeCurrent ();

  current_context = &context;
}

