#include <stddef.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class XMLLexer
{
  public:      
    enum Lexem {                 //�������
      UNDEFINED,                 //�� ����������
      EQUAL,                     //���������
      KEYWORD,                   //�������� �����
      CDATA,                     //������ CDATA
      SECTOR_BEGIN_BRACKET,      //����������� ������ �������
      SECTOR_END_BRACKET,        //����������� ������ �������
      TAG_BEGIN_BRACKET,         //����������� ������ ����
      TAG_END_BRACKET,           //����������� ������ ����
      TAG_CLOSE,                 //�������� ����
      END_OF_FILE,               //����� �����
      TOKEN,                     //�������������, ������������ � �����
      IDENTIFIER,                //�������������
      STRING,                    //������
      INSTRUCTION_BEGIN_BRACKET, //����������� ������ ���������� �� ���������
      INSTRUCTION_END_BRACKET    //����������� ������ ���������� �� ���������
    };
      
    enum Error {        //��� ������
      NO_ERROR,         //��� ������
      UNCLOSED_COMMENT, //���������� ����������
      UNCLOSED_STRING,  //���������� ������
      UNCLOSED_CDATA,   //���������� ������ CDATA
      WRONG_CHAR,       //�������� ������
      WRONG_IDENTIFIER, //�������� �������������
    };

    XMLLexer ();

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
    void nextline ();
    void read_string (char border);
    void read_shifted_string (char border);
    void read_cdata ();
    void read_identifier (bool identifier);

    void SetError (Error,char*);

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
    bool   name_char_map [256];
};

}
