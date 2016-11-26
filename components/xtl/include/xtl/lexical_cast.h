#ifndef XTL_LEXICAL_CAST_HEADER
#define XTL_LEXICAL_CAST_HEADER

#include <cstdio>

#include <stl/string>

#include <xtl/string.h>
#include <xtl/token_parser.h>

namespace math
{

//forward declarations
template <class T, unsigned int Size> class vector;
template <class T, unsigned int Size> class matrix;
template <class T>                    class quat;

}

namespace xtl
{

/*
    В отличие от boost, lexical_cast работает через двойное приведение source_type->stl::string->destination_type
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///Преобразования тип -> строка
///////////////////////////////////////////////////////////////////////////////////////////////////
void to_string (stl::string& buffer, const stl::string&);
void to_string (stl::string& buffer, const char* value);
void to_string (stl::string& buffer, const wchar_t* value);
void to_string (stl::string& buffer, const stl::wstring&);
void to_string (stl::string& buffer, const signed char* value);
void to_string (stl::string& buffer, const unsigned char* value);
void to_string (stl::string& buffer, char value);
void to_string (stl::string& buffer, signed char value);
void to_string (stl::string& buffer, unsigned char value);
void to_string (stl::string& buffer, wchar_t value);
void to_string (stl::string& buffer, short value);
void to_string (stl::string& buffer, unsigned short value);
void to_string (stl::string& buffer, int value);
void to_string (stl::string& buffer, unsigned int value);
void to_string (stl::string& buffer, long value);
void to_string (stl::string& buffer, unsigned long value);
void to_string (stl::string& buffer, long long value);
void to_string (stl::string& buffer, unsigned long long value);
void to_string (stl::string& buffer, const volatile bool& value); //const volatile& для избежания неявного преобразования к bool в xtl::any
void to_string (stl::string& buffer, float value);
void to_string (stl::string& buffer, const double& value);
void to_string (stl::string& buffer, const long double& value);
void to_string (stl::string& buffer, const void* pointer);

template <class T> stl::string to_string (const T& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Преобразования строка -> тип
///////////////////////////////////////////////////////////////////////////////////////////////////
void to_value (const stl::string& buffer, stl::string&);
void to_value (const stl::string& buffer, stl::wstring&);
void to_value (const stl::string& buffer, char& value);
void to_value (const stl::string& buffer, signed char& value);
void to_value (const stl::string& buffer, unsigned char& value);
void to_value (const stl::string& buffer, wchar_t& value);
void to_value (const stl::string& buffer, short& value);
void to_value (const stl::string& buffer, unsigned short& value);
void to_value (const stl::string& buffer, int& value);
void to_value (const stl::string& buffer, unsigned int& value);
void to_value (const stl::string& buffer, long& value);
void to_value (const stl::string& buffer, unsigned long& value);
void to_value (const stl::string& buffer, long long& value);
void to_value (const stl::string& buffer, unsigned long long& value);
void to_value (const stl::string& buffer, bool& value);
void to_value (const stl::string& buffer, float& value);
void to_value (const stl::string& buffer, double& value);
void to_value (const stl::string& buffer, long double& value);
void to_value (const stl::string& buffer, void*& pointer);

template <class T> T to_value (const stl::string&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Лексикографическое преобразование
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class DstT, class SrcT> DstT lexical_cast (const SrcT&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вывод диапазонов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Iter>
void to_string (stl::string& buffer, Iter first, Iter last, const char* separator);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Приведение к строке математических типов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class String, class T, unsigned int Size>
void to_string (String& buffer, const math::vector<T, Size>& value);

template <class String, class T, unsigned int Size>
void to_string (String& buffer, const math::matrix<T, Size>& value);

template <class String, class T>
void to_string (String& buffer, const math::quat<T>& value);

#include <xtl/detail/lexical_cast.inl>

}

#endif
