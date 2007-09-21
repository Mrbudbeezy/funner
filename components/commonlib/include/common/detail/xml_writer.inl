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
    xml_write (Stream (), value);
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
    xml_write (Stream (), value, format);
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
