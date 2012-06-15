#ifndef COMMONLIB_PARSER_JSON_SHARED_HEADER
#define COMMONLIB_PARSER_JSON_SHARED_HEADER

#include <stl/vector>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/string.h>

#include <common/component.h>
#include <common/parser.h>
#include <common/strconv.h>
#include <common/utf_converter.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������������� ������� Json-������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum JsonLexem
{
  JsonLexem_Undefined,               //�� ����������
  JsonLexem_Colon,                   //���������
  JsonLexem_Comma,                   //�������
  JsonLexem_ObjectBeginBracket,      //����������� ������ �������
  JsonLexem_ObjectEndBracket,        //����������� ������ �������
  JsonLexem_ArrayBeginBracket,       //����������� ������ �������
  JsonLexem_ArrayEndBracket,         //����������� ������ �������
  JsonLexem_EndOfFile,               //����� �����
  JsonLexem_String,                  //������
  JsonLexem_Number,                  //�������� ��������
  JsonLexem_Boolean,                 //������� ��������
  JsonLexem_Null,                    //null-�������� ����

  JsonLexem_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������ ����������� Json-������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum JsonLexerStatus
{
  JsonLexerStatus_NoError,                    //��� ������
  JsonLexerStatus_UnclosedString,             //���������� ������
  JsonLexerStatus_WrongChar,                  //�������� ������
  JsonLexerStatus_WrongNumber,                //�������� �������� ��������
  JsonLexerStatus_WrongKeyword,               //�������� �������� ��������
  JsonLexerStatus_InvalidCharacterReference,  //�������� ������ �� ������

  JsonLexerStatus_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class JsonLexer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    JsonLexer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Reset (char* buf);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    JsonLexem NextLexem ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���� �� � ������� ������� ��������� ������� ��� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool ArrayHasObjects ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    JsonLexem       Lexem        () const { return current_lexem; }           //������� �������
    JsonLexerStatus Status       () const { return current_status; }          //������� ������ �������
    size_t          LineNumber   () const { return line_number; }             //������� ������
    size_t          LinePosition () const { return cursor - line_start + 1; } //�������� �� ������ ������
    const char*     Token        () const { return current_token; }           //������� �����
    char            CurrentChar  () const { return cursor == erased_char_position ? erased_char : *cursor; } //������� ������

  private:
    void Skip ();
    void NextLine ();
    void ReadString ();
    void ReadSymbolReference (char*& write_position);
    void ReadNumber ();
    void ReadKeyword (JsonLexem lexem, const char* keyword);
    void SetError (JsonLexerStatus error, const char* position);

  private:
    JsonLexem       current_lexem;
    JsonLexerStatus current_status;
    char            erased_char;
    char*           erased_char_position;
    char*           position;
    const char*     current_token;
    char*           line_start;
    const char*     cursor;
    size_t          line_number;
};

}

#endif
