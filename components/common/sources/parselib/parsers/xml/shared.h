#ifndef COMMONLIB_PARSER_XML_SHARED_HEADER
#define COMMONLIB_PARSER_XML_SHARED_HEADER

#include <cctype>
#include <cstring>

#include <stl/vector>

#include <xtl/function.h>
#include <xtl/string.h>

#include <common/component.h>
#include <common/parser.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������������� ������� Xml-������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum XmlLexem
{
  XmlLexem_Undefined,               //�� ����������
  XmlLexem_Equal,                   //���������
  XmlLexem_Keyword,                 //�������� �����
  XmlLexem_CData,                   //������ CDATA
  XmlLexem_SectorBeginBracket,      //����������� ������ �������
  XmlLexem_SectorEndBracket,        //����������� ������ �������
  XmlLexem_TagBeginBracket,         //����������� ������ ����
  XmlLexem_TagEndBracket,           //����������� ������ ����
  XmlLexem_TagClose,                //�������� ���� '/>'
  XmlLexem_TagCloseFrame,           //�������� ���� '</'
  XmlLexem_EndOfFile,               //����� �����
  XmlLexem_Token,                   //�������������, ������������ � �����
  XmlLexem_Identifier,              //�������������
  XmlLexem_String,                  //������
  XmlLexem_InstructionBeginBracket, //����������� ������ ���������� �� ���������
  XmlLexem_InstructionEndBracket,   //����������� ������ ���������� �� ���������

  XmlLexem_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������ ����������� Xml-������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum XmlLexerStatus
{
  XmlLexerStatus_NoError,                    //��� ������
  XmlLexerStatus_UnclosedComment,            //���������� ����������
  XmlLexerStatus_UnclosedString,             //���������� ������
  XmlLexerStatus_UnclosedCData,              //���������� ������ CDATA
  XmlLexerStatus_WrongChar,                  //�������� ������
  XmlLexerStatus_WrongIdentifier,            //�������� �������������
  XmlLexerStatus_InvalidCharacterReference,  //�������� ������ �� ������

  XmlLexerStatus_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class XmlLexer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    XmlLexer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Reset (char* buf);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    XmlLexem NextLexem (bool content = false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    XmlLexem       Lexem        () const { return current_lexem; }           //������� �������
    XmlLexerStatus Status       () const { return current_status; }          //������� ������ �������
    size_t         LineNumber   () const { return line_number; }             //������� ������
    size_t         LinePosition () const { return cursor - line_start + 1; } //�������� �� ������ ������
    const char*    Token        () const { return current_token; }           //������� �����
    char           CurrentChar  () const { return cursor == erased_char_position ? erased_char : *cursor; } //������� ������

  private:
    void Skip ();
    void SkipBlockComment ();
    void NextLine ();
    void ReadContentString ();
    void ReadString (char border, char* terminators = 0, size_t terminators_count = 0);  //���� ����������� border - �� ���������� ����� � ������� ������� ����������� �� ��������� ������, ���� ����������� ���������� - ������ ������ ���������������
    void ReadSymbolReference (char* write_position);
    void ReadCData ();
    void ReadIdentifier (bool identifier);
    void SetError (XmlLexerStatus error, const char* position);
    void ProcessTagBeginBracket (bool process_cdata);
    void ReadTextString ();

  private:
    XmlLexem       current_lexem;
    XmlLexerStatus current_status;
    char           erased_char;
    char*          erased_char_position;
    char*          position;
    const char*    current_token;
    char*          line_start;
    const char*    cursor;
    size_t         line_number;
};

}

#endif
