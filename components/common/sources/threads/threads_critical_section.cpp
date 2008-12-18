#include "shared.h"

using namespace common;

/*
    ���� / ����� �� ����������� ������
*/

void CriticalSection::Enter ()
{
  try
  {
    Platform::GetThreadSystem ()->EnterCriticalSection ();    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::CriticalSection::Enter");
    throw;
  }
}

void CriticalSection::Exit ()
{
  try
  {
    Platform::GetThreadSystem ()->ExitCriticalSection ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::CriticalSection::Exit");
    throw;
  }
}
