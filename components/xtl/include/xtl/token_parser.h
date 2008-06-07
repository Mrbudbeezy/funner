#ifndef XTL_TOKEN_PARSER_HEADER
#define XTL_TOKEN_PARSER_HEADER

#include <ctype.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>

#include <typeinfo>
#include <exception>

namespace xtl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������: �������� ������������������ ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
class bad_lexical_cast: public std::bad_cast
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bad_lexical_cast ();
    bad_lexical_cast (const std::type_info& source_type, const std::type_info& target_type);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� � ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const std::type_info& source_type () const;
    const std::type_info& target_type () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    const char* what () const throw () { return "xtl::bad_lexical_cast"; }

  private:
    const std::type_info *source, *target;
};

namespace io
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������� ����� �� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
bool read (const char*    string, bool& result_value);
bool read (const char*    string, char& result_value);
bool read (const char*    string, signed char& result_value);
bool read (const char*    string, unsigned char& result_value);
bool read (const char*    string, wchar_t& result_value);
bool read (const char*    string, short& result_value);
bool read (const char*    string, unsigned short& result_value);
bool read (const char*    string, int& result_value);
bool read (const char*    string, unsigned int& result_value);
bool read (const char*    string, long& result_value);
bool read (const char*    string, unsigned long& result_value);
bool read (const char*    string, float& result_value);
bool read (const char*    string, double& result_value);
bool read (const char*    string, long double& result_value);
bool read (const wchar_t* string, bool& result_value);
bool read (const wchar_t* string, wchar_t& result_value);
bool read (const wchar_t* string, char& result_value);
bool read (const wchar_t* string, signed char& result_value);
bool read (const wchar_t* string, unsigned char& result_value);
bool read (const wchar_t* string, short& result_value);
bool read (const wchar_t* string, unsigned short& result_value);
bool read (const wchar_t* string, int& result_value);
bool read (const wchar_t* string, unsigned int& result_value);
bool read (const wchar_t* string, long& result_value);
bool read (const wchar_t* string, unsigned long& result_value);
bool read (const wchar_t* string, float& result_value);
bool read (const wchar_t* string, double& result_value);
bool read (const wchar_t* string, long double& result_value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������� � ����������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Base, class Char, class Value>
bool read_and_cast (const Char* string, Value& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������� � ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Value, class Char> Value get (const Char* string); //throws bad_lexical_cast
template <class Value, class Char> Value get (const Char* string, const Value& default_value); //nothrow

#include <xtl/detail/io/token_parser.inl>

}

#include <xtl/detail/bad_lexical_cast.inl>

}

#endif
