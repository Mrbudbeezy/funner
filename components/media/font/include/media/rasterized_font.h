#ifndef MEDIALIB_RASTERIZED_FONT_HEADER
#define MEDIALIB_RASTERIZED_FONT_HEADER

#include <cstdlib>

namespace media
{

//forward declarations
class Image;

//////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � ��������������� �����
//////////////////////////////////////////////////////////////////////////////////////////////////
struct RasterizedGlyphInfo
{
  size_t image_index; //������ ����������� � ��������������� ������
  size_t x_pos;       //X ���������� ������� ������ ���� �����
  size_t y_pos;       //Y ���������� ������� ������ ���� �����
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� �����
//////////////////////////////////////////////////////////////////////////////////////////////////
class RasterizedFont
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RasterizedFont  ();
    RasterizedFont  (const RasterizedFont& source);
    ~RasterizedFont ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RasterizedFont& operator = (const RasterizedFont&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    media::Font Font () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const RasterizedGlyphInfo* RasterizedGlyphs () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t ImagesCount () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* ImageName (size_t image_index) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BuildImage (size_t image_index, media::Image& out_image) const; //???? format

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (RasterizedFont&);

  private:
    RasterizedFontImpl* impl;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///�����
//////////////////////////////////////////////////////////////////////////////////////////////////
void swap (RasterizedFont&, RasterizedFont&);

}

#endif
