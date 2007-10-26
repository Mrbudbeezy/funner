#include <wchar.h>
#include <common/utf_converter.h>

namespace common
{
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������: �������� ��������� Unicode
///////////////////////////////////////////////////////////////////////////////////////////////////
//struct UtfExceptionTag;

//typedef DerivedException<Exception, UtfExceptionTag> UtfException;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� Utf -> wint_t
///////////////////////////////////////////////////////////////////////////////////////////////////
EncodingResult utf_decode (const void* source_buffer,            //�����-�������� � utf-�������
                           size_t      source_buffer_size,       //������ ������-��������� � ������
                           Encoding    source_buffer_encoding,   //��������� ������-���������
                           void*       destination_buffer,       //�����-������� ��� ������������ ������ (wint_t)
                           size_t      destination_buffer_size)  //������ ������-�������� � ������
{
   EncodingResult res;
   if (source_buffer_size>=1)
   {
      uint8_t first = *src++;
      if ( 0 == (first & 0x80) )
      {
         // ok, single byte ASCII-7 (US-ASCII) code
         cp = first;
      }
      else
      {
         // multibyte character code
         // read remaining byte count and 
         // most signifigant bits of the character code
         int bytes = 1;
         uint8_t ByteCountMask = 0x40;
         Char32 codeMask = 0x3F;
         while ( first & ByteCountMask )
         {
            ++bytes;
            ByteCountMask >>= 1;
            codeMask >>= 1;
         }
         if ( bytes < 2 || bytes > 4 )
         {
            // ERROR: Invalid number of following bytes
            err = 1;
         }
         else
         {
            if ( srcSize < bytes )
      {
               // ERROR: Not enough encoded bytes available
               err = 4;
            }
            else
            {
               // read remaining bytes of the character code
               cp = first & codeMask;
               for ( int i = 1 ; i < bytes ; ++i )
               {
                  cp <<= 6;
                  cp |= ( 0x3F & (Char32)*src++ );
               }
            }
         }
      }
   }
   else
   {
      // ERROR: Not enough encoded bytes available
      err = 4;
   }

   if ( !err )
      *buffer = cp;
      *srcBytes = src-src0;
      return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� wint_t -> Utf
///////////////////////////////////////////////////////////////////////////////////////////////////
EncodingResult utf_encode (const void* source_buffer,                //�����-�������� � wint_t - �������
                           size_t      source_buffer_size,           //������ ������-��������� � ������
                           void*       destination_buffer,           //�����-������� � ���������� ������ (utf)
                           size_t      destination_buffer_size,      //������ ������-�������� � ������
                           Encoding    destination_buffer_encoding) //��������� ������-��������
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� wchar_t -> utf8
///////////////////////////////////////////////////////////////////////////////////////////////////
EncodingResult convert_to_utf8 (const wchar_t* source_buffer,            //�����-��������
                                size_t         source_buffer_size,       //������� ������-���������
                                char*          destination_buffer,       //�����-�������
                                size_t         destination_buffer_size) //������ ������-��������
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� utf8 -> wchar_t
///////////////////////////////////////////////////////////////////////////////////////////////////
EncodingResult convert_to_utf16 (const char* source_buffer,            //�����-��������
                                 size_t      source_buffer_size,       //������� ������-���������
                                 wchar_t*    destination_buffer,       //�����-�������
                                 size_t      destination_buffer_size) //������ ������-��������
{
}

}
