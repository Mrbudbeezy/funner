#include "shared.h"

using namespace common;

namespace
{

/*
    ���������
*/

const size_t DEFAULT_MESSAGES_COUNT_RESERVE = 128;  //������������� ���������� ��������� �� ������� �������
const size_t DEFAULT_MESSAGE_BUFFER_RESERVE = 1024; //������������� ������ ������ ������

/*
    ���������� � ���������
*/

struct ParseLogMessage
{
  ParseLogMessageType type;   //��� ���������  
  size_t              offset; //�������� �� ��������� �� ������
};

typedef stl::vector<ParseLogMessage> MessageArray;

}

/*
    �������� ���������� ��������� �������
*/

struct ParseLog::Impl: public xtl::reference_counter
{
  MessageArray messages;       //��������� ���������
  stl::string  message_buffer; //����� � ����������� �� �������
  bool         has_errors;     //���� ������ �������
  bool         has_warnings;   //���� �������������� �������

///�����������
  Impl ()
  {
    has_errors   = false;
    has_warnings = false;
    
    messages.reserve (DEFAULT_MESSAGES_COUNT_RESERVE);
    message_buffer.reserve (DEFAULT_MESSAGE_BUFFER_RESERVE);
  }

///���������� ���������
  void InsertMessage (const char* file_name, size_t line, const char* node_name, ParseLogMessageType type, const char* format, va_list args, bool need_prefix = true)
  {
    static const char* METHOD_NAME = "common::ParseLog::Impl::InsertMessage";
    
      //�������� ������������ ����������
      
    if (need_prefix && !file_name)
      throw xtl::make_null_argument_exception (METHOD_NAME, "file_name");

    if (!format)
      throw xtl::make_null_argument_exception (METHOD_NAME, "format");

      //�������������� ���� ���������

    const char* message_type_string;

    switch (type)
    {
      case ParseLogMessageType_Error:      message_type_string = "error";   break;
      case ParseLogMessageType_Warning:    message_type_string = "warning"; break;
      case ParseLogMessageType_FatalError: message_type_string = "fatal error"; break;
      default:
        throw xtl::make_argument_exception (METHOD_NAME, "message_type", type);
    }
    
      //���������� ��������� �� ������
      
    ParseLogMessage message = {type, message_buffer.size ()};
    
    if (need_prefix)
    {
      if (node_name)
      {
        if (type != ParseLogMessageType_FatalError)
        {
          message_buffer += common::format ("%s(%u): %s: node '%s': ", file_name, line, message_type_string, node_name);
        }
        else
        {
          message_buffer += common::format ("%s: node '%s': ", message_type_string, node_name);
        }        
      }
      else
      {
        if (type != ParseLogMessageType_FatalError)
        {
          message_buffer += common::format ("%s(%u): %s: ", file_name, line, message_type_string);
        }
        else
        {
          message_buffer += common::format ("%s: ", message_type_string);      
        }        
      }
    }
    
    try
    {
      message_buffer += common::vformat (format, args);
      message_buffer += '\0';

      messages.push_back (message);
    }
    catch (...)
    {
      message_buffer.erase (message.offset);
      throw;
    }
  }
};

/*
    ������������ / ���������� / ������������
*/

ParseLog::ParseLog ()
  : impl (new Impl)
{
}

ParseLog::ParseLog (const ParseLog& log)
  : impl (log.impl)
{
  addref (impl);
}

ParseLog::~ParseLog ()
{
  release (impl);
}

ParseLog& ParseLog::operator = (const ParseLog& log)
{
  ParseLog (log).Swap (*this);
  
  return *this;
}

/*
    ���������� ��������� / ���������� � ��������� ����������������
*/

size_t ParseLog::MessagesCount () const
{
  return impl->messages.size ();
}

bool ParseLog::HasErrors () const
{
  return impl->has_errors;
}

bool ParseLog::HasWarnings () const
{
  return impl->has_warnings;
}

/*
    ������� ���������
*/

const char* ParseLog::Message (size_t index) const
{
  if (index >= impl->messages.size ())
    throw xtl::make_range_exception ("common::ParseLog::Message", "index", index, impl->messages.size ());

  return impl->message_buffer.c_str () + impl->messages [index].offset;
}

ParseLogMessageType ParseLog::MessageType (size_t index) const
{
  if (index >= impl->messages.size ())
    throw xtl::make_range_exception ("common::ParseLog::MessageType", "index", index, impl->messages.size ());

  return impl->messages [index].type;
}

/*
    ���������� ��������� �� ������
*/

void ParseLog::Printf (ParseLogMessageType type, const char* format, ...)
{
  va_list list;
  
  va_start (list, format);
  VPrintf  (type, format, list);
}

void ParseLog::VPrintf (ParseLogMessageType type, const char* format, va_list list)
{
  switch (type)
  {
    case ParseLogMessageType_FatalError:
    case ParseLogMessageType_Error:
      impl->has_errors = true;
      break;
    case ParseLogMessageType_Warning:
      impl->has_warnings = true;
      break;
    default:
      throw xtl::make_argument_exception ("common::ParseLog::VPrintf", "type", type);
  }
  
  impl->InsertMessage (0, 0, 0, type, format, list, false);
}

void ParseLog::VFatalError (const char* format, va_list list)
{
  impl->has_errors = true;

  impl->InsertMessage ("__undefined__", 0, 0, ParseLogMessageType_FatalError, format, list);
}

void ParseLog::VError (const char* file_name, size_t line, const char* format, va_list list)
{
  impl->has_errors = true;

  impl->InsertMessage (file_name, line, 0, ParseLogMessageType_Error, format, list);
}

void ParseLog::VWarning (const char* file_name, size_t line, const char* format, va_list list)
{
  impl->has_warnings = true;

  impl->InsertMessage (file_name, line, 0, ParseLogMessageType_Warning, format, list);
}

void ParseLog::VError (const ParseNode& node, const char* format, va_list list)
{
  impl->has_errors = true;

  impl->InsertMessage (node.Source (), node.LineNumber (), node.Name (), ParseLogMessageType_Error, format, list);
}

void ParseLog::VWarning (const ParseNode& node, const char* format, va_list list)
{
  impl->has_warnings = true;

  impl->InsertMessage (node.Source (), node.LineNumber (), node.Name (), ParseLogMessageType_Warning, format, list);
}

void ParseLog::FatalError (const char* format, ...)
{
  va_list list;

  va_start    (list, format);  
  VFatalError (format, list);
}

void ParseLog::Error (const char* file_name, size_t line, const char* format, ...)
{
  va_list list;
  
  va_start (list, format);  
  VError   (file_name, line, format, list);
}

void ParseLog::Warning (const char* file_name, size_t line, const char* format, ...)
{
  va_list list;
  
  va_start (list, format);  
  VWarning (file_name, line, format, list);
}

void ParseLog::Error (const ParseNode& node, const char* format, ...)
{
  va_list list;
  
  va_start (list, format);  
  VError   (node, format, list);  
}

void ParseLog::Warning (const ParseNode& node, const char* format, ...)
{
  va_list list;
  
  va_start (list, format);  
  VWarning (node, format, list);
}

/*
    ������ ���������
*/

void ParseLog::Print (const LogFunction& fn)
{
  for (MessageArray::iterator iter=impl->messages.begin (), end=impl->messages.end (); iter!=end; ++iter)
  {
    try
    {
      fn (impl->message_buffer.c_str () + iter->offset);
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }
}

/*
    �����
*/

void ParseLog::Swap (ParseLog& log)
{
  stl::swap (impl, log.impl);
}

namespace common
{

void swap (ParseLog& log1, ParseLog& log2)
{
  log1.Swap (log2);
}

}
