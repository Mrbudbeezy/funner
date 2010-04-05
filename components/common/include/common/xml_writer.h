#ifndef COMMONLIB_XML_WRITER_HEADER
#define COMMONLIB_XML_WRITER_HEADER

#include <common/streams.h>

namespace common
{

//forward declaration
class XmlNodeScope;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����, ���������� ������������ ������ � ����� XML �������
///////////////////////////////////////////////////////////////////////////////////////////////////
  //+wchar_t
class XmlWriter
{
  public:
    enum { 
      DEFAULT_NODE_INDENT = 2, //�������� ������� ���� (�� ���������)
    };
    
    typedef xtl::function<size_t (const void* buffer, size_t size)> WriteFunction;
    typedef XmlNodeScope                                            Scope;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    XmlWriter  (const char* file_name, size_t node_indent=DEFAULT_NODE_INDENT);
    XmlWriter  (const WriteFunction& writer, size_t node_indent=DEFAULT_NODE_INDENT);
    ~XmlWriter ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const OutputTextStream& Stream () const;
          OutputTextStream& Stream ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������� ������ ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t NodeIndent () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BeginNode (const char* frame_tag);
    void EndNode   ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> void WriteAttribute (const char* name, const T& value);
    template <class T> void WriteAttribute (const char* name, const T& value, const char* format);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������ � ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> void WriteData  (const T& value);
    template <class T> void WriteData  (const T& value, const char* format);
    template <class T> void WriteCData (const T& value);
    template <class T> void WriteCData (const T& value, const char* format);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������ ����� (BeginNode+WriteData/WriteCData+EndNode)
///////////////////////////////////////////////////////////////////////////////////////////////////
                       void WriteNode      (const char* name);
    template <class T> void WriteNode      (const char* name, const T& value);
    template <class T> void WriteNode      (const char* name, const T& value, const char* format);
    template <class T> void WriteNodeCData (const char* name, const T& value);
    template <class T> void WriteNodeCData (const char* name, const T& value, const char* format);    

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void WriteComment (const char* comment);
    void WriteComment (const wchar_t* comment);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Flush ();

  private:
    void WriteIndent (int offset=0);
    void WriteHeader ();
    void RaiseEndOfDocument (const char*);
    void CheckEndOfDocument (const char*);
    void BeginAttribute (const char* name);
    void EndAttribute ();
    void CloseAttributeList ();
    void BeginData ();
    void EndData ();
    void BeginCData ();
    void EndCData ();

    template <class T> void WriteXmlTextData (const T& value);
    template <class T> void WriteXmlTextData (const T& value, const char* format);
    template <class Char, class Traits, class Allocator>
    void WriteXmlTextData (const stl::basic_string<Char, Traits, Allocator>& value);
    template <class Char, class Traits, class Allocator>
    void WriteXmlTextData (const stl::basic_string<Char, Traits, Allocator>& value, const char* format);
    void WriteXmlTextData (const char* value);
    void WriteXmlTextData (const char* value, const char* format);


  private:
    XmlWriter (const XmlWriter&); //no impl
    XmlWriter& operator = (const XmlWriter&); //no impl

  private: 
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������ ��������� ����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> void xml_write (OutputTextStream&, const T& value);
template <class T> void xml_write (OutputTextStream&, const T& value, const char* format);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ������������ ����� Xml
///////////////////////////////////////////////////////////////////////////////////////////////////
class XmlNodeScope
{
  public:
    XmlNodeScope  (XmlWriter& writer, const char* frame_tag);
    ~XmlNodeScope ();
    
  private:
    XmlNodeScope (const XmlNodeScope&); //no impl
    XmlNodeScope& operator = (const XmlNodeScope&); //no impl

  private:
    XmlWriter& writer;
};

#include <common/detail/xml_writer.inl>

}

#endif
