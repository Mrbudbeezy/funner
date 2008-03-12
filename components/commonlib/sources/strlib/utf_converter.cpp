#include <wchar.h>
#include <string.h>
#include <common/utf_converter.h>
#include <common/exception.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////

class CharConverter
{
public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������
///////////////////////////////////////////////////////////////////////////////////////////////////
  static bool decode_ASCII7 (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
  {
    const unsigned char* src_ptr = (const unsigned char*)src_char;
    size_t* dst_ptr = (size_t*)dst_char;
    
    if (src_size < sizeof(char))
      return false;
    
    (*dst_ptr) = (*src_ptr > 0x7F) ? 0x3F : (*src_ptr);
    
    src_ptr++;
    src_size -= sizeof(char);
    
    src_char = src_ptr;
    return true;
  }

  static bool decode_UTF8 (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
  {
    const unsigned char* src_ptr = (const unsigned char*)src_char;
    size_t* dst_ptr = (size_t*)dst_char;
    
    if (src_size < sizeof(char))
      return false;
    
    if (*src_ptr >= 0xF0)
    {
      if (src_size < sizeof(char)*4)
        return false;
      
      *dst_ptr =   (*(src_ptr+3) & 0x3F) +
                 ( (*(src_ptr+2) & 0x3F) << 6 ) +
                 ( (*(src_ptr+1) & 0x3F) << 12 ) +
                 ( (*(src_ptr) & 0x07) << 18 );
      
      src_ptr += 4;
      src_size -= sizeof(char) * 4;
    }
    else if (*src_ptr >= 0xE0)
    {
      if (src_size < sizeof(char)*3)
        return false;

      *dst_ptr =   (*(src_ptr+2) & 0x3F) +
                 ( (*(src_ptr+1) & 0x3F) << 6 ) +
                 ( (*(src_ptr) & 0x0F) << 12 );
      
      src_ptr += 3;
      src_size -= sizeof(char) * 3;
    }
    else if (*src_ptr >= 0xC0)
    {
      if (src_size < sizeof(char)*2)
        return false;

      *dst_ptr =   (*(src_ptr+1) & 0x3F) +
                 ( (*(src_ptr) & 0x1F) << 6 );
      
      src_ptr += 2;
      src_size -= sizeof(char) * 2;
    }
    else
    {
      *dst_ptr = (size_t)(*src_ptr > 0x7F ? 0x3F : *src_ptr);

      src_ptr++;
      src_size -= sizeof(char);
    }
    
    src_char = src_ptr;
    return true;
  }

  static bool decode_UTF16LE (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
  {
    const wchar_t* src_ptr = (const wchar_t*) src_char;
    size_t* dst_ptr = (size_t*) dst_char;
    
    if (src_size < sizeof(wchar_t))
      return false;
    
    if ( (*src_ptr < 0xD800) || (*src_ptr > 0xDFFF) )
    {
      *dst_ptr = *src_ptr;
      src_ptr++;
      src_size -= sizeof(wchar_t);
    }
    else
    {
      if (src_size < sizeof(wchar_t)*2)
        return false;

      if ( (*src_ptr > 0xDBFF ) || (*(src_ptr+1) < 0xDC00) || (*(src_ptr+1) > 0xDFFF) )
        *dst_ptr = 0x3F;
      else
        *dst_ptr = (*src_ptr & 0x03FF) + ( (*(src_ptr+1) & 0x03FF) << 10) + 0x10000;
      
      src_ptr += 2;
      src_size += sizeof(wchar_t)*2;
    }
    
    src_char = src_ptr;
    return true;
  }

  static bool decode_UTF16BE (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
  {
    const wchar_t* src_ptr = (const wchar_t*) src_char;
    size_t* dst_ptr = (size_t*) dst_char;
    wchar_t tmp0, tmp1;
    
    if (src_size < sizeof(wchar_t))
      return false;
    
    tmp0 = ((*src_ptr >> 8) & 0xFF) + ((*src_ptr & 0xFF) << 8);
    
    if ( (tmp0 < 0xD800) || (tmp0 > 0xDFFF) )
    {
      *dst_ptr = tmp0;
      src_ptr++;
      src_size -= sizeof(wchar_t);
    }
    else
    {
      if (src_size < sizeof(wchar_t)*2) 
        return false;
      
      tmp1 = ((*(src_ptr+1) >> 8) & 0xFF) + ((*(src_ptr+1) & 0xFF) << 8);
      
      if ( (tmp0 > 0xDBFF ) || (tmp1 < 0xDC00) || (tmp1 > 0xDFFF) )
        *dst_ptr = 0x3F;
      else
        *dst_ptr = ( tmp0 & 0x03FF) + ( ( tmp1 & 0x03FF) << 10) + 0x10000;
      
      src_ptr += 2;
      src_size -= sizeof(wchar_t)*2;
    }
    
    src_char = src_ptr;
    return true;
  }

  static bool decode_UTF32LE (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
  {
    const size_t* src_ptr = (const size_t*) src_char;
    size_t* dst_ptr = (size_t*) dst_char;

    if (src_size < sizeof(size_t))
      return false;
    
    *dst_ptr = *src_ptr;
    src_ptr++;
    src_size -= sizeof(size_t);
    
    src_char = src_ptr;
    return true;
  }

  static bool decode_UTF32BE (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
  {
    const size_t* src_ptr = (const size_t*) src_char;
    size_t* dst_ptr = (size_t*) dst_char;

    if (src_size < sizeof(size_t))
      return false;
    
    *dst_ptr = ((*src_ptr & 0xFF) << 24) +
               ((*src_ptr & 0xFF00) << 8) +
               ((*src_ptr & 0xFF0000) >> 8) +
               ((*src_ptr & 0xFF000000) >> 24);
    
    src_ptr++;
    src_size -= sizeof(size_t);
    
    src_char = src_ptr;
    return true;
  }
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
  static bool encode_ASCII7 (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
  {
    const size_t* src_ptr = (const size_t*) src_char;
    unsigned char* dst_ptr = (unsigned char*) dst_char;
    
    if (dst_size < sizeof(char))
      return false;
      
    *dst_ptr = (char)((*src_ptr) > 0x7F ? 0x3F : (*src_ptr));
    
    dst_ptr++;
    dst_size -= sizeof(char);

    dst_char = dst_ptr;
    return true;
  }

  static bool encode_UTF8 (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
  {
    const size_t* src_ptr = (const size_t*) src_char;
    unsigned char* dst_ptr = (unsigned char*) dst_char;
    
    if (*src_ptr > 0xFFFF)
    {
      if (dst_size < sizeof(char)*4)
        return false;
      
      *(dst_ptr+3) = ((*src_ptr) & 0x3F) | 0x80;
      *(dst_ptr+2) = ( ( (*src_ptr) >> 6) & 0x3F) | 0x80;
      *(dst_ptr+1) = ( ( (*src_ptr) >> 12) & 0x3F) | 0x80;
      *(dst_ptr) = ( ( (*src_ptr) >> 18) & 0x07) | 0xF0;
      
      dst_ptr += 4;
      dst_size -= sizeof(char)*4;
    }
    else if (*src_ptr > 0x7FF)
    {
      if (dst_size < sizeof(char)*3)
        return false;

      *(dst_ptr+2) = ((*src_ptr) & 0x3F) | 0x80;
      *(dst_ptr+1) = ( ( (*src_ptr) >> 6) & 0x3F) | 0x80;
      *(dst_ptr) = ( ( (*src_ptr) >> 12) & 0x0F) | 0xE0;
      
      dst_ptr += 3;
      dst_size -= sizeof(char)*3;
    }
    else if (*src_ptr > 0x7F)
    {
      if (dst_size < sizeof(char)*2)
        return false;

      *(dst_ptr+1) = ((*src_ptr) & 0x3F) | 0x80;
      *(dst_ptr) = ( ( (*src_ptr) >> 6) & 0x1F) | 0xC0;
      
      dst_ptr += 2;
      dst_size -= sizeof(char)*2;
    }
    else
    {
      if (dst_size < sizeof(char))
        return false;

      *(dst_ptr) = (*src_ptr) & 0x7F;
      
      dst_ptr ++;
      dst_size -= sizeof(char);
    }
    
    dst_char = dst_ptr;
    return true;
  }

  static bool encode_UTF16LE (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
  {
    const size_t* src_ptr = (const size_t*) src_char;
    wchar_t* dst_ptr = (wchar_t*) dst_char;
    size_t tmp;
    
    if (*src_ptr > 0x10000)
    {
      if (dst_size < sizeof(wchar_t)*2)
        return false;
      
      tmp = *src_ptr - 0x10000;
      *dst_ptr = (wchar_t) ( ( (tmp >> 10) & 0x03FF ) + 0xD800);
      *(dst_ptr+1) = (wchar_t) ( ( tmp & 0x03FF ) + 0xDC00);
      
      dst_ptr +=2;
      dst_size -= sizeof(wchar_t)*2;
    }
    else
    {
      if (dst_size < sizeof(wchar_t))
        return false;
      
      *dst_ptr = (wchar_t)*src_ptr;
      
      dst_ptr++;
      dst_size -= sizeof(wchar_t);
    }
    
    dst_char = dst_ptr;
    return true;
  }

  static bool encode_UTF16BE (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
  {
    const size_t* src_ptr = (const size_t*) src_char;
    wchar_t* dst_ptr = (wchar_t*) dst_char;
    wchar_t tmp0, tmp1;
    
    if (*src_ptr > 0x10000)
    {
      if (dst_size < sizeof(wchar_t)*2)
        return false;
      
      tmp0 = (wchar_t) ( (((*src_ptr - 0x10000) >> 10) & 0x3FF) + 0xD800 );
      tmp1 = (wchar_t) ( ((*src_ptr - 0x10000) & 0x3FF) + 0xDC00 );
      *dst_ptr = (wchar_t) ( ((tmp0 & 0xFF) << 8) + ((tmp0 >> 8) & 0xFF));
      *(dst_ptr+1) = (wchar_t) ( ((tmp1 & 0xFF) << 8) + ((tmp1 >> 8) & 0xFF));
      
      dst_ptr +=2;
      dst_size -= sizeof(wchar_t)*2;
    }
    else
    {
      if (dst_size < sizeof(wchar_t))
        return false;
      
      *dst_ptr = (wchar_t) ( ((*src_ptr & 0xFF) << 8) + ((*src_ptr >> 8) & 0xFF));
      
      dst_ptr++;
      dst_size -= sizeof(wchar_t);
    }
    
    dst_char = dst_ptr;
    return true;
  }

  static bool encode_UTF32LE (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
  {
    const size_t* src_ptr = (const size_t*) src_char;
    size_t* dst_ptr = (size_t*) dst_char;

    if (dst_size < sizeof(size_t))
      return false;
    
    *dst_ptr = *src_ptr;
    
    dst_ptr++;
    dst_size -= sizeof(size_t);
    
    dst_char = dst_ptr;
    return true;
  }

  static bool encode_UTF32BE (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
  {
    const size_t* src_ptr = (const size_t*) src_char;
    size_t* dst_ptr = (size_t*) dst_char;

    if (dst_size < sizeof(size_t))
      return false;
    
    *dst_ptr = ((*src_ptr & 0xFF) << 24) +
               ((*src_ptr & 0xFF00) << 8) +
               ((*src_ptr & 0xFF0000) >> 8) +
               ((*src_ptr & 0xFF000000) >> 24);
    
    dst_ptr++;
    dst_size -= sizeof(size_t);
    
    dst_char = dst_ptr;
    return true;
  }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef bool (*CharConverterFn)(const void*&, size_t&, void*&, size_t&);

void ConvertEncoding(Encoding       source_encoding,
                     const void*&   source,
                     size_t&        source_size,
                     Encoding       destination_encoding,
                     void*&         destination,
                     size_t&        destination_size)
{
  if (!source)
    RaiseNullArgument("common::ConvertEncoding (const char*, const void*&, size_t&, const char*, void*&, size_t&)", "source");

  if (!destination)
    RaiseNullArgument("common::ConvertEncoding (const char*, const void*&, size_t&, const char*, void*&, size_t&)", "destination");

  CharConverterFn decoder, encoder;
  
  switch (source_encoding)
  {
    case Encoding_ASCII7:
      decoder = CharConverter::decode_ASCII7;
      break;
    case Encoding_UTF8:
      decoder = CharConverter::decode_UTF8;
      break;
    case Encoding_UTF16LE:
      decoder = CharConverter::decode_UTF16LE;
      break;
    case Encoding_UTF16BE:
      decoder = CharConverter::decode_UTF16BE;
      break;
    case Encoding_UTF32LE:
      decoder = CharConverter::decode_UTF32LE;
      break;
    case Encoding_UTF32BE:
      decoder = CharConverter::decode_UTF32BE;
      break;
    default:
      RaiseInvalidArgument("common::ConvertEncoding (const char*, const void*&, size_t&, const char*, void*&, size_t&)", "source_encoding");
  }
  
  switch (destination_encoding)
  {
    case Encoding_ASCII7:
      encoder = CharConverter::encode_ASCII7;
      break;
    case Encoding_UTF8:
      encoder = CharConverter::encode_UTF8;
      break;
    case Encoding_UTF16LE:
      encoder = CharConverter::encode_UTF16LE;
      break;
    case Encoding_UTF16BE:
      encoder = CharConverter::encode_UTF16BE;
      break;
    case Encoding_UTF32LE:
      encoder = CharConverter::encode_UTF32LE;
      break;
    case Encoding_UTF32BE:
      encoder = CharConverter::encode_UTF32BE;
      break;
    default:
      RaiseInvalidArgument("common::ConvertEncoding (const char*, const void*&, size_t&, const char*, void*&, size_t&)", "destination_encoding");
  }

  size_t buffer;
  void* buffer_ptr = &buffer;
  const void* buffer_cptr = &buffer;
  size_t buffer_size = 4;
  
  for (; source_size && destination_size;)
  {
    if (!decoder(source, source_size, buffer_ptr, buffer_size))
      return;
    
    if (!encoder(buffer_cptr, buffer_size, destination, destination_size))
      return;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
stl::wstring towstring (const char* string, int length)
{
   if(string==NULL)
      throw ArgumentNullException("towstring","string is null");
  if (!string)
    return L"";

  if (length == -1)
    length = strlen (string);

  stl::wstring result;
  
  int result_size = (int)mbstowcs (0, string, length);

  if (result_size == -1)
    return L"(common::towstring error)";

  if (!result_size)
    return L"";

  result.fast_resize (result_size);
  
  mbstowcs (&result [0], string, length);

  return result;
}

stl::wstring towstring (const char* string)
{
   if(string==NULL)
      throw ArgumentNullException("towstring","string is null");
  return towstring (string, -1);
}

stl::wstring towstring (const stl::string& string)
{
  if (string.empty ())
    return L"";

  return towstring (&string [0], string.size ());
}

stl::string tostring (const wchar_t* string, int length)
{
   if(string==NULL)
      throw ArgumentNullException("tostring","string is null");
  if (!string)
    return "";

  if (length == -1)
    length = wcslen (string);

  stl::string result;
  
  int result_size = (int)wcstombs (0, string, length);
  
  if (result_size == -1)
    return "(common::tostring error)";
    
  if (!result_size)
    return "";
  
  result.fast_resize (result_size);
  
  wcstombs (&result [0], string, length);
  
  return result;
}

stl::string tostring (const wchar_t* string)
{
   if(string==NULL)
      throw ArgumentNullException("tostring","string is null");
  return tostring (string, -1);
}

stl::string tostring (const stl::wstring& string)
{
  if (string.empty ())
    return "";

  return tostring (&string [0], string.size ());
}

} //namespace common
