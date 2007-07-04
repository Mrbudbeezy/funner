#include <stddef.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class WXFLexer
{
  public:      
    enum Lexem {     //�������
      UNDEFINED,     //�� ����������
      END_OF_FILE,   //����� �����
      NEW_LINE,      //����� ������
      VALUE,         //�������� (�����)
      IDENTIFIER,    //�������������
      STRING,        //������
      BEGIN_BRACKET, //����������� ������ '('
      END_BRACKET,   //����������� ������ ')'
      BEGIN_FRAME,   //������ ������ '{'
      END_FRAME,     //����� ������ '}'
      DIRECTIVE      //��������� '#...'
    };
      
    enum Error {        //��� ������
      NO_ERROR,         //��� ������
      UNCLOSED_COMMENT, //���������� ����������
      UNCLOSED_STRING,  //���������� ������
      WRONG_CHAR,       //�������� ������
      WRONG_IDENTIFIER, //�������� �������������
      WRONG_VALUE,      //�������� ��������
      WRONG_DIRECTIVE   //�������� ���������
    };

    WXFLexer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������. ��������� ������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void reset (char* buf);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Lexem next  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Lexem  lexem () const { return cur_lexem; }               //������� �������
    Error  error () const { return cur_error; }               //������� ������
    size_t line  () const { return line_number; }             //������� ������
    size_t offs  () const { return cursor - line_start + 1; } //�������� �� ������ ������

    const char* token   () const { return cur_token; }    //������� �����
    char        curchar () const { return cursor == erased_char_pos ? erased_char : *cursor; } //������� ������

  private:
    void skip ();
    void skip_block_comment ();
    void skip_rest_of_line ();
    void skip_wrong_token ();
    void nextline ();
    void read_string (char border);
    void read_identifier ();
    void read_value ();
    void read_directive ();

    void SetError        (Error error,char* position = NULL);
    void SetErrorAndSkip (Error error,char* position = NULL);

  private:    
    static char* null_ptr;
    static char  null_char;

    Lexem  cur_lexem;
    Error  cur_error;
    char   erased_char, *erased_char_pos;
    char*  pos;
    char*  buf;
    char*  cur_token;
    char*  line_start;
    char*  cursor;
    size_t line_number;
};

}
