/*
    ���������� ������ ��������� ����� ���������
*/

template <class T>
inline void xml_write (OutputTextStream& stream, const T& value)
{
  write (stream, value);
}

template <class T>
inline void xml_write (OutputTextStream& stream, const T& value, const char* format)
{
  write (stream, value, format);
}

template <class T>
inline void XmlWriter::WriteXmlTextData (const T& value)
{
  write (Stream (), value);
}

template <class T>
inline void XmlWriter::WriteXmlTextData (const T& value, const char* format)
{
  write (Stream (), value, format);
}

inline void XmlWriter::WriteXmlTextData (const char* value)
{
  if (strstr (value, " "))
  {
    CheckEndOfDocument ("common::XmlWriter::BeginCData");
    CloseAttributeList ();
    write              (Stream (), "<![CDATA[");

    try
    {
      xml_write (Stream (), value);
      write     (Stream (), "]]>");
    }
    catch (...)
    {
      write (Stream (), "]]>");
      throw;
    }
  }
  else
    write (Stream (), value);
}

inline void XmlWriter::WriteXmlTextData (const char* value, const char* format)
{
  WriteXmlTextData (value);
}

template <class Char, class Traits, class Allocator>
inline void XmlWriter::WriteXmlTextData (const stl::basic_string<Char, Traits, Allocator>& value)
{
  WriteXmlTextData (value.c_str ());
}

template <class Char, class Traits, class Allocator>
inline void XmlWriter::WriteXmlTextData (const stl::basic_string<Char, Traits, Allocator>& value, const char* format)
{
  WriteXmlTextData (value);
}

/*
    ������������ ���������
*/

template <class T>
inline void XmlWriter::WriteAttribute (const char* name, const T& value)
{
  BeginAttribute (name);
  
  try
  {
    xml_write    (Stream (), value);
    EndAttribute ();
  }
  catch (...)
  {
    EndAttribute ();
    throw;
  }
}

template <class T>
inline void XmlWriter::WriteAttribute (const char* name, const T& value, const char* format)
{
  BeginAttribute (name);

  try
  {
    xml_write    (Stream (), value, format);
    EndAttribute ();
  }
  catch (...)
  {
    EndAttribute ();
    throw;
  }  
}

/*
    ������ ������ � ����
*/

template <class T>
inline void XmlWriter::WriteData (const T& value)
{
  BeginData ();
  
  try
  {
    WriteXmlTextData (value);
    EndData   ();
  }
  catch (...)
  {
    EndData ();
    throw;
  }
}

template <class T>
inline void XmlWriter::WriteData (const T& value, const char* format)
{
  BeginData ();
  
  try
  {
    WriteXmlTextData (value, format);
    EndData   ();
  }
  catch (...)
  {
    EndData ();
    throw;
  }
}

template <class T>
inline void XmlWriter::WriteCData (const T& value)
{
  BeginCData ();
  
  try
  {
    xml_write (Stream (), value);
    EndCData  ();
  }
  catch (...)
  {
    EndCData ();
    throw;
  }
}

template <class T>
inline void XmlWriter::WriteCData (const T& value, const char* format)
{
  BeginCData ();
  
  try
  {
    xml_write (Stream (), value, format);
    EndCData  ();
  }
  catch (...)
  {
    EndCData ();
    throw;
  }
}

/*
    ���������� ������������ ����� (BeginNode+WriteData+EndNode)
*/

inline void XmlWriter::WriteNode (const char* name)
{
  BeginNode (name);
  EndNode ();
}

template <class T>
inline void XmlWriter::WriteNode (const char* name, const T& value)
{
  BeginNode (name);
  
  try
  {
    WriteData (value);
    EndNode   ();
  }
  catch (...)
  {
    EndNode ();
    throw;
  }  
}

template <class T>
inline void XmlWriter::WriteNode (const char* name, const T& value, const char* format)
{
  BeginNode (name);
  
  try
  {
    WriteData (value, format);
    EndNode   ();
  }
  catch (...)
  {
    EndNode ();
    throw;
  }  
}

template <class T>
inline void XmlWriter::WriteNodeCData (const char* name, const T& value)
{
  BeginNode (name);
  
  try
  {
    WriteCData (value);
    EndNode    ();
  }
  catch (...)
  {
    EndNode ();
    throw;
  }  
}

template <class T>
inline void XmlWriter::WriteNodeCData (const char* name, const T& value, const char* format)
{
  BeginNode (name);
  
  try
  {
    WriteCData (value, format);
    EndNode    ();
  }
  catch (...)
  {
    EndNode ();
    throw;
  }  
}

/*
    ���� ������������ ����� Xml
*/

inline XmlNodeScope::XmlNodeScope (XmlWriter& in_writer, const char* frame_tag)
  : writer (in_writer)
{
  writer.BeginNode (frame_tag);  
}

inline XmlNodeScope::~XmlNodeScope ()
{
  writer.EndNode ();
}
