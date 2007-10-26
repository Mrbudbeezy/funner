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
   return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� wint_t -> Utf8
///////////////////////////////////////////////////////////////////////////////////////////////////
EncodingResult utf_encode (const void* source_buffer,                //�����-�������� � wint_t - �������
                           size_t      source_buffer_size,           //������ ������-��������� � ������
                           void*       destination_buffer,           //�����-������� � ���������� ������ (utf)
                           size_t      destination_buffer_size,      //������ ������-�������� � ������
                           Encoding    destination_buffer_encoding) //��������� ������-��������
{
   EncodingResult res;
   return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� wchar_t -> utf8
///////////////////////////////////////////////////////////////////////////////////////////////////
EncodingResult convert_to_utf8 (const wchar_t* source_buffer,            //�����-��������
                                size_t         source_buffer_size,       //������� ������-���������
                                char*          destination_buffer,       //�����-�������
                                size_t         destination_buffer_size) //������ ������-��������
{
   EncodingResult res;
   return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� utf8 -> utf16
///////////////////////////////////////////////////////////////////////////////////////////////////
EncodingResult convert_to_utf16 (const char* source_buffer,            //�����-��������
                                 size_t      source_buffer_size,       //������� ������-���������
                                 wchar_t*    destination_buffer,       //�����-�������
                                 size_t      destination_buffer_size) //������ ������-��������
{
   unsigned int *cp=(unsigned int*)destination_buffer;
   memset(cp,0,destination_buffer_size);
   char *src=(char*)source_buffer;
   EncodingResult res;
   unsigned int i;
   for(i=0;i<source_buffer_size;)
   {
      char first = *src++;
      if ( 0 == (first & 0x80) )
      {
         // ok, single byte ASCII-7 (US-ASCII) code
         *cp = first;
         i++;
         cp++;
      }
      else
      {
         // multibyte character code
         // read remaining byte count and 
         // most signifigant bits of the character code
         int bytes = 1;
         char ByteCountMask = 0x40;
         unsigned int codeMask = 0x3F;
         while ( first & ByteCountMask )
         {
            ++bytes;
            ByteCountMask >>= 1;
            codeMask >>= 1;
         }
/*         if (!(bytes<2 || bytes>4))
         {
            if (!(srcSize<bytes))
            {
               // read remaining bytes of the character code
               cp = first & codeMask;
               for ( int i = 1 ; i < bytes ; ++i )
               {
                  cp <<= 6;
                  cp |= ( 0x3F & (unsigned int)*src++ );
               }
            }
         }*/
      i+=bytes;
      }
   }
   res.source_buffer_processed_size=i;
   res.destination_buffer_processed_size=0;
   return res;
}

}
