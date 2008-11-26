#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/signal.h>

#include <common/component.h>
#include <common/console.h>
#include <common/singleton.h>
#include <common/strlib.h>

using namespace common;

namespace
{

/*
    ���������
*/

const char* CONSOLE_HANDLERS_MASK = "common.console.*";

/*
    ���������� �������
*/

class ConsoleImpl
{
  public:
///������ ���������
    void Print (const char* message)
    {
      if (!message)
        throw xtl::make_null_argument_exception ("common::Console::Print", "message");

      LoadConsoleHandlers ();

      try
      {
        signal (message);
      }
      catch (...)
      {
      }
    }

///����������� ������������ ������� �������
    xtl::connection RegisterEventHandler (const Console::EventHandler& handler)
    {
      return signal.connect (handler);
    }

  private:
    void LoadConsoleHandlers ()
    {
      static ComponentLoader console_handlers_loader (CONSOLE_HANDLERS_MASK);
    }

  private:
    typedef xtl::signal<void (const char*)> EventSignal;

  private:
    EventSignal signal;
};

/*
    �������� �������
*/

typedef Singleton<ConsoleImpl> ConsoleSingleton;

size_t print_lock = false; //������� ������������� ��� ������

}

/*
   ������ ���������
*/

void Console::Print (const char* message)
{
  if (print_lock)
    return;

  try
  {
    print_lock = true;
    
    ConsoleSingleton::Instance ().Print (message);
    
    print_lock = false;
  }
  catch (...)
  {
    print_lock = false;
    throw;
  }
}

void Console::Printf (const char* message, ...)
{
  va_list list;

  va_start (list, message);
  VPrintf  (message, list);
}

void Console::VPrintf (const char* message, va_list list)
{
  if (!message)
    throw xtl::make_null_argument_exception ("common::Console::VPrintf", "message");
    
  if (print_lock)
    return;

  Print (common::vformat (message, list).c_str ());
}

/*
   ����������� ������������ ������� �������
*/

xtl::connection Console::RegisterEventHandler (const Console::EventHandler& handler)
{
  return ConsoleSingleton::Instance ().RegisterEventHandler (handler);
}
