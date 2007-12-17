#ifndef COMMONLIB_UTF_CONVERTER_HEADER
#define COMMONLIB_UTF_CONVERTER_HEADER

#include <wchar.h>
#include <stl/string_fwd>

namespace common
{

typedef unsigned int char32;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Unicode-���������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum Encoding
{
  Encoding_Ascii7,
  Encoding_Utf8,
  Encoding_Utf16BE,
  Encoding_Utf16LE,
  Encoding_Utf32BE,
  Encoding_Utf32LE
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������: �������� ��������� Unicode
///////////////////////////////////////////////////////////////////////////////////////////////////
//struct UtfExceptionTag;

//typedef DerivedException<Exception, UtfExceptionTag> UtfException;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct EncodingResult
{
  size_t source_buffer_processed_size;      //���������� ������� ������������ ������ ������-���������
  size_t destination_buffer_processed_size; //���������� ������� ������������ ������ ������-��������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� Utf -> wint_t
///////////////////////////////////////////////////////////////////////////////////////////////////
EncodingResult utf_decode (const void* source_buffer,            //�����-�������� � utf-�������
                           size_t      source_buffer_size,       //������ ������-��������� � ������
                           Encoding    source_buffer_encoding,   //��������� ������-���������
                           void*       destination_buffer,       //�����-������� ��� ������������ ������ (wint_t)
                           size_t      destination_buffer_size); //������ ������-�������� � ������

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� wint_t -> Utf
///////////////////////////////////////////////////////////////////////////////////////////////////
EncodingResult utf_encode (const void* source_buffer,                //�����-�������� � wint_t - �������
                           size_t      source_buffer_size,           //������ ������-��������� � ������
                           void*       destination_buffer,           //�����-������� � ���������� ������ (utf)
                           size_t      destination_buffer_size,      //������ ������-�������� � ������
                           Encoding    destination_buffer_encoding); //��������� ������-��������

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� char <-> wchar_t
///////////////////////////////////////////////////////////////////////////////////////////////////
stl::string  tostring  (const wchar_t* string, int length);
stl::string  tostring  (const wchar_t* string);
stl::string  tostring  (const stl::wstring&);
stl::wstring towstring (const char* string, int length);
stl::wstring towstring (const char* string);
stl::wstring towstring (const stl::string&);

};

#endif 
