#include "shared.h"

using namespace common;

/*
    ��������� ������
*/

enum WriterState
{
  WriterState_DocumentStart,  //������ � ������ ���������
  WriterState_DocumentEnd,    //������ ���������
  WriterState_AttributeList,  //������ ������ ���������
  WriterState_AttributeValue, //����� �������� ��������
  WriterState_NodeScope,      //������ � ����� ����
};

/*
    �������� ���������� XmlWriter    
*/

typedef stl::stack<stl::string> NodeStack;

struct XmlWriter::Impl
{
  OutputTextStream stream;         //����� ������
  size_t           node_indent;    //������ ����� ������
  NodeStack        node_stack;     //���� ��� �����
  WriterState      state;          //��������� ������
  stl::string      escaped_string; //������ � ����������� ������ ����������� ��������
  
  Impl (const WriteFunction& writer, size_t in_node_indent) :
       stream (writer), node_indent (in_node_indent), state (WriterState_DocumentStart) {}
};

/*
    ������������
*/

XmlWriter::XmlWriter (const char* file_name, size_t node_indent)
  : impl (new Impl (FileWriter (file_name), node_indent))
{
  WriteHeader ();
}

XmlWriter::XmlWriter (const WriteFunction& writer, size_t node_indent)
  : impl (new Impl (writer, node_indent))
{
  WriteHeader ();
}

XmlWriter::~XmlWriter ()
{
  while (!impl->node_stack.empty ())
    EndNode ();
}

/*
    ����� ������
*/

const OutputTextStream& XmlWriter::Stream () const
{
  return impl->stream;
}

OutputTextStream& XmlWriter::Stream ()
{
  return impl->stream;
}

/*
    ������ ������� ������ ����
*/

size_t XmlWriter::NodeIndent () const
{
  return impl->node_indent;
}

/*
    ������ � ����������
*/

void XmlWriter::BeginAttribute (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("common::XmlWriter::WriteAttribute", "name");    
    
  if (impl->state != WriterState_AttributeList)
    throw xtl::format_operation_exception ("common::XmlWriter::WriteAttribute", "Can't write attribute. Attribute list not opened");
        
  write (impl->stream, " ");
  write (impl->stream, name);
  write (impl->stream, "=\"");

  impl->state = WriterState_AttributeValue;
}

void XmlWriter::EndAttribute ()
{
  if (impl->state != WriterState_AttributeValue)
    return;
    
  write (impl->stream, "\"");
    
  impl->state = WriterState_AttributeList;
}

void XmlWriter::CloseAttributeList ()
{
  EndAttribute ();

  if (impl->state != WriterState_AttributeList)
    return;

  write (impl->stream, ">\n");
  
  impl->state = WriterState_NodeScope;
}

/*
    ������ � ������
*/

void XmlWriter::BeginNode (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("common::XmlWriter::BeginNode", "name");

    //�������� ����������� ������

  CheckEndOfDocument ("common::XmlWriter::BeginNode");

    //�������� ������ ���������

  CloseAttributeList ();
  
    //������ �������
  
  WriteIndent ();  

    //��������� ����� ���� � ����

  impl->node_stack.push (name);

    //������ ��������� ����

  write (impl->stream, "<");
  write (impl->stream, name);

    //������� � ��������� ������ ������ ���������

  impl->state = WriterState_AttributeList;
}

void XmlWriter::EndNode ()
{
  if (impl->node_stack.empty ())
    return;

  switch (impl->state)
  {
    case WriterState_DocumentStart:      
      return;
    case WriterState_NodeScope:
      WriteIndent (-1);

      write (impl->stream, "</");
      write (impl->stream, impl->node_stack.top ());
      write (impl->stream, ">\n");
      
      impl->node_stack.pop ();

      if (impl->node_stack.empty ())
        impl->state = WriterState_DocumentEnd;          

      break;
    case WriterState_AttributeValue:
      write (impl->stream, "\"");
    case WriterState_AttributeList:    
      write (impl->stream, "/>\n");

      impl->node_stack.pop ();

      impl->state = impl->node_stack.empty () ? WriterState_DocumentEnd : WriterState_NodeScope;

      break;
    case WriterState_DocumentEnd:
      RaiseEndOfDocument ("common::XmlWriter::EndNode");
      break;
  }
}

/*
    ������ ������
*/

void XmlWriter::BeginData ()
{
  CheckEndOfDocument ("common::XmlWriter::BeginData");
  CloseAttributeList ();
  WriteIndent        ();
}

void XmlWriter::EndData ()
{
  write (impl->stream, "\n");
}

void XmlWriter::BeginCData ()
{
  CheckEndOfDocument ("common::XmlWriter::BeginCData");
  CloseAttributeList ();
  WriteIndent        ();
  write              (impl->stream, "<![CDATA[");
}

void XmlWriter::EndCData ()
{
  write (impl->stream, "]]>\n");
}

/*
    ������� �����������
*/

void XmlWriter::WriteComment (const char* comment)
{
  if (!comment)
    throw xtl::make_null_argument_exception ("common::XmlWriter::WriteComment", "comment");
    
    //�������� ����������� ������
    
  CheckEndOfDocument ("common::XmlWriter::WriteComment");
    
    //�������� ������ ���������
    
  CloseAttributeList ();

    //������ �����������

  WriteIndent ();

  write (impl->stream, "<!--");

  if (strchr (comment,'\n'))
  { 
    write (impl->stream, "\n");
    
    const char *line_start = comment, *current_char = comment;

    for (;*current_char; current_char++)
      switch (*current_char)
      {
        case '\n':
          WriteIndent        (1);
          impl->stream.Write (line_start, current_char - line_start);
          impl->stream.Write ("\n");

          line_start = current_char + 1;

          break;
        default:
          break;
      }

    if (line_start != current_char)
    {
      WriteIndent (1);
      write       (impl->stream, line_start);  
      write       (impl->stream, "\n");
    }

    WriteIndent ();
  }
  else
  {
    write (impl->stream, " ");
    write (impl->stream, comment);
    write (impl->stream, " ");
  }

  write (impl->stream, "-->\n");
}

/*
    ����� ������� ������
*/

void XmlWriter::Flush ()
{
  impl->stream.Buffer ().Flush ();  
}

/*
    ������ ���������
*/

void XmlWriter::WriteHeader ()
{
  impl->stream.Write ("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
}

/*
    ������ �������
*/

void XmlWriter::WriteIndent (int offset)
{
  write (impl->stream, (impl->node_stack.size () + offset) * impl->node_indent, ' ');
}

/*
    �������� ������������. ����������� ���������� � ������ ������ � ����� ���������
*/

void XmlWriter::RaiseEndOfDocument (const char* source)
{
  throw xtl::format_operation_exception (source, "Root node has been closed");
}

void XmlWriter::CheckEndOfDocument (const char* source)
{
  if (impl->state == WriterState_DocumentEnd)
    RaiseEndOfDocument (source);
}

/*
    ������������ ��������
*/

const char* XmlWriter::EscapeStringSymbols (const char* value)
{
  struct Marker
  {
    char        marker;
    const char* replacement;
  };

  static Marker       markers []    = {{'<', "&lt;"}, {'>', "&gt;"}, {'&', "&amp;"}, {'"', "&quot;"}};
  static const size_t markers_count = sizeof markers / sizeof *markers;

  size_t result_length = 0;

  for (const char* current_symbol = value; *current_symbol; current_symbol++)
  {
    const Marker* m       = markers;
    bool          escaped = false;

    for (size_t i = 0; i < markers_count; i++, m++)
    {
      if (*current_symbol == m->marker)
      {
        escaped = true;

        for (const char* replacement = m->replacement; *replacement; replacement++)
          result_length++;

        break;
      }
    }

    if (!escaped)
      result_length++;
  }

  impl->escaped_string.resize (result_length);

  size_t write_position = 0;

  for (const char* current_symbol = value; *current_symbol; current_symbol++)
  {
    const Marker* m       = markers;
    bool          escaped = false;

    for (size_t i = 0; i < markers_count; i++, m++)
    {
      if (*current_symbol == m->marker)
      {
        escaped = true;

        for (const char* replacement = m->replacement; *replacement; replacement++)
          impl->escaped_string [write_position++] = *replacement;

        break;
      }
    }

    if (!escaped)
      impl->escaped_string [write_position++] = *current_symbol;
  }

  return impl->escaped_string.c_str ();
}
