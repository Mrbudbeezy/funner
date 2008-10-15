#ifndef COMMONLIB_PARSER_WXF_SHARED_HEADER
#define COMMONLIB_PARSER_WXF_SHARED_HEADER

#include <cctype>
#include <cstddef>

#include <stl/string>
#include <stl/vector>

#include <xtl/function.h>

#include <common/component.h>
#include <common/file.h>
#include <common/parser.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum WxfLexem
{
  WxfLexem_Undefined,    //�� ����������
  WxfLexem_EndOfFile,    //����� �����
  WxfLexem_NewLine,      //����� ������
  WxfLexem_Value,        //�������� (�����)
  WxfLexem_Identifier,   //�������������
  WxfLexem_String,       //������
  WxfLexem_BeginBracket, //����������� ������ '('
  WxfLexem_EndBracket,   //����������� ������ ')'
  WxfLexem_BeginFrame,   //������ ������ '{'
  WxfLexem_EndFrame,     //����� ������ '}'
  WxfLexem_Directive,    //��������� '#...'

  WxfLexem_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������������ ����������� wxf-������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum WxfLexerStatus
{
  WxfLexerStatus_NoError,          //��� ������
  WxfLexerStatus_UnclosedComment,  //���������� ����������
  WxfLexerStatus_UnclosedString,   //���������� ������
  WxfLexerStatus_WrongChar,        //�������� ������
  WxfLexerStatus_WrongIdentifier,  //�������� �������������
  WxfLexerStatus_WrongValue,       //�������� ��������
  WxfLexerStatus_WrongDirective,   //�������� ���������

  WxfLexerStatus_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class WxfLexer
{
  public:      
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    WxfLexer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Reset (char* buffer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    WxfLexem NextLexem  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    WxfLexem       Lexem        () const { return current_lexem; }               //������� �������
    WxfLexerStatus Status       () const { return current_status; }              //������� ������
    size_t         LineNumber   () const { return line_number; }                 //������� ������
    size_t         LinePosition () const { return cursor - line_start + 1; }     //�������� �� ������ ������
    const char*    Token        () const { return current_token; }               //������� �����
    char           CurrentChar  () const { return cursor == erased_char_position ? erased_char : *cursor; } //������� ������

  private:
    WxfLexer (const WxfLexer&); //no impl
    WxfLexer& operator = (const WxfLexer&); //no impl
  
    void Skip ();
    void SkipBlockComment ();
    void SkipRestOfLine ();
    void SkipWrongToken ();
    void NextLine ();
    void ReadString (char border);
    void ReadIdentifier ();
    void ReadValue ();
    void ReadDirective ();
    void SetError (WxfLexerStatus status, char* position = 0);
    void SetErrorAndSkip (WxfLexerStatus status, char* position = 0);

  private:
    WxfLexem       current_lexem;
    WxfLexerStatus current_status;
    char           erased_char;
    char*          erased_char_position;
    char*          position;
    char*          current_token;
    char*          line_start;
    char*          cursor;
    size_t         line_number;
};

}

#endif
