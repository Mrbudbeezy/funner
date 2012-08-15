#ifndef MEDIALIB_FONT_CONVERTER_HEADER
#define MEDIALIB_FONT_CONVERTER_HEADER

#include <media/font.h>

namespace media
{

//forward declaration
class Image;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct FontDesc
{
  const char*    file_name;        //��� ����� �� �������
  const size_t*  char_codes;       //������ � ��������� ��� ���������
  size_t         char_codes_count; //���������� �������� ��� ���������
  size_t         glyph_size;       //������������ ������ ������� �������
  float          stroke_width;     //������ ����� �������
  size_t         first_glyph_code; //��� ������� �����
  size_t         glyph_interval;   //������� ����� ��������� �������
  bool           fast_convert;     //������� ��������� (����� ������������)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
void convert (const FontDesc& font_desc, Font& result_font, Image& result_image);

}

#endif
