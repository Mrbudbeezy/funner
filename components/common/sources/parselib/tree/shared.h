#ifndef COMMONLIB_PARSE_TREE_SHARED_HEADER
#define COMMONLIB_PARSE_TREE_SHARED_HEADER

#include <cstring>

#include <stl/auto_ptr.h>
#include <stl/vector>

#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/uninitialized_storage.h>

#include <common/hash.h>
#include <common/parser.h>
#include <common/singleton.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������ ��������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef xtl::uninitialized_storage<char> ParseTreeBuffer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���� ������ ��������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ParseNodeFlags
{
  ParseNodeFlag_AttributesAsPointers = 1, //�������� �������������� � ���������
};

#ifdef _MSC_VER
  #pragma pack (push)
#endif

#pragma pack (1)

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���� ������ ��������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ParseNodeImpl
{
  size_t attributes_count;   //���������� ��������� ��������� � �����
  size_t name_hash;          //��� �� ����� ����
  size_t attributes_offset;  //�������� �� ������� ���������
  size_t flags;              //�����
  size_t next_offset;        //�������� � ���������� ���� �� ������ ������ ��������
  size_t first_offset;       //�������� �� ������� ������
  size_t source_back_offset; //������������� �������� �� ������ � ������������ ����� ��������� ������
  size_t line_number;        //����� ������
};

#ifdef _MSC_VER
  #pragma pack (pop)
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ParseTree: public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ParseTree  (ParseTreeBuffer& buffer, const ParseLog* log = 0);
    ~ParseTree ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void*       Buffer     ()       { return buffer.data (); }    
    const void* Buffer     () const { return buffer.data (); }
    size_t      BufferSize () const { return buffer.size (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ParseLog& Log ();

  private:
    ParseTree (const ParseTree&); //no impl
    ParseTree& operator = (const ParseTree&); //no impl

  private:
    ParseTreeBuffer         buffer; //����� ������ �������
    stl::auto_ptr<ParseLog> log;    //�������� �������    
};

}

#endif

