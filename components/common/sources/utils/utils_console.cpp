#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
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

const char*  CONSOLE_HANDLERS_MASK   = "common.console.*";
const size_t DEFAULT_MAX_LINES_COUNT = 256;

/*
    ���������� �������
*/

class ConsoleImpl
{
  public:
///�����������
    ConsoleImpl () : max_lines_count (DEFAULT_MAX_LINES_COUNT), first_line_index (0)
    {
      lines.reserve (max_lines_count);
    }

///������ ���������
    void Print (const char* message)
    {
      if (!message)
        throw xtl::make_null_argument_exception ("common::Console::Print", "message");
        
        //���������� � ������������� ������ ���������

      LoadConsoleHandlers ();

      try
      {
        signals [ConsoleEvent_OnPrint] (message);
      }
      catch (...)
      {
      }
        
        //�������� �� ������ � ���������� � ��������� �����

      const char* volatile pos = strchr (message, '\n');

      if (!pos)
      {
        current_line += message;
      }
      else
      {
        const char* prev_pos = message;
        
        do
        {
          current_line.append (prev_pos, pos - prev_pos);
          
          AddLine ();          

          current_line.clear ();          

          prev_pos = pos + 1;
          pos      = strchr (prev_pos, '\n');
        } while (pos);

          //��������� �������� ���������

        if (*prev_pos)
          current_line = prev_pos;
      }
    }

///����������� ������������ ������� �������
    xtl::connection RegisterEventHandler (ConsoleEvent event, const Console::EventHandler& handler)
    {
      if (event < 0 || event > ConsoleEvent_Num)
        throw xtl::make_argument_exception ("common::Console::RegisterEventHandler", "event", event);

      return signals [event].connect (handler);
    }

///������������ ���������� �������� ���������
    size_t MaxLinesCount () { return max_lines_count; }

///��������� ������������� ���������� �������� ���������
    void SetMaxLinesCount (size_t in_max_lines_count)
    {
      lines.reserve (in_max_lines_count);

      if (in_max_lines_count < lines.size ())
        lines.erase (lines.begin () + in_max_lines_count, lines.end ());

      max_lines_count = in_max_lines_count;
      
      current_line.clear ();
    }

///���������� ���������
    size_t LinesCount () { return lines.size (); }

///��������� ���������
    const char* Message (size_t index)
    {
      if (index >= lines.size ())
        throw xtl::make_range_exception ("common::Console::Message", "index", index, lines.size ());

      return lines [(first_line_index+index) % lines.size ()]->data.c_str ();
    }

  private:
    void LoadConsoleHandlers ()
    {
      static ComponentLoader console_handlers_loader (CONSOLE_HANDLERS_MASK);
    }
    
    void AddLine ()
    {
        //���������� � ��������� ����� ������
      
      try
      {
        signals [ConsoleEvent_OnPrintLine] (current_line.c_str ());
      }
      catch (...)
      {
      }      
      
        //���������� ������� ���������

      if (!max_lines_count)
        return;

      if (lines.size () != max_lines_count)
      {
        lines.push_back (LinePtr (new ConsoleLine (current_line), false));
        
        if (lines.size () == max_lines_count)
          first_line_index = 0;
      }
      else
      {
        lines [first_line_index % lines.size ()]->data = current_line;

        first_line_index++;
      }
    }

  private:
    //������ �������
    struct ConsoleLine: public xtl::reference_counter
    {
      ConsoleLine (const stl::string& in_data) : data (in_data) {}

      stl::string data;
    };  
  
    typedef xtl::intrusive_ptr<ConsoleLine>  LinePtr;
    typedef stl::vector<LinePtr>             LineList;
    typedef xtl::signal<void (const char*)>  EventSignal;

  private:
    size_t      max_lines_count;
    size_t      first_line_index;
    stl::string current_line;
    LineList    lines;
    EventSignal signals [ConsoleEvent_Num];
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

xtl::connection Console::RegisterEventHandler (ConsoleEvent event, const Console::EventHandler& handler)
{
  return ConsoleSingleton::Instance ().RegisterEventHandler (event, handler);
}

/*
    ������������ ���������� �������� ���������
*/

void Console::SetMaxLinesCount (size_t lines_count)
{
  ConsoleSingleton::Instance ().SetMaxLinesCount (lines_count);
}

size_t Console::MaxLinesCount ()
{
  return ConsoleSingleton::Instance ().MaxLinesCount ();
}

/*
    ������������ ����� �������
*/

size_t Console::LinesCount ()
{
  return ConsoleSingleton::Instance ().LinesCount ();
}

const char* Console::Message (size_t index)
{
  return ConsoleSingleton::Instance ().Message (index);
}
