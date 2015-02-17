#ifndef MEDIALIB_RASTERIZED_FONT_HEADER
#define MEDIALIB_RASTERIZED_FONT_HEADER

#include <cstdlib>

namespace media
{

//forward declarations
class Font;
class Image;
class IFontRasterizer;

//////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � ��������������� �����
//////////////////////////////////////////////////////////////////////////////////////////////////
struct RasterizedGlyphInfo
{
  unsigned int image_index; //������ ����������� � ��������������� ������
  unsigned int x_pos;       //X ���������� ������� ������ ���� �����
  unsigned int y_pos;       //Y ���������� ������� ������ ���� �����
  unsigned int width;       //������ �����
  unsigned int height;      //������ �����
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� �����
//////////////////////////////////////////////////////////////////////////////////////////////////
class RasterizedFont
{
  friend class Font;

  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RasterizedFont  (const RasterizedFont& source);
    ~RasterizedFont ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RasterizedFont& operator = (const RasterizedFont&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const RasterizedGlyphInfo* RasterizedGlyphs () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int ImagesCount () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BuildImage (unsigned int image_index, media::Image& out_image) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (RasterizedFont&);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RasterizedFont (IFontRasterizer*);

  private:
    struct Impl;
    Impl* impl;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///�����
//////////////////////////////////////////////////////////////////////////////////////////////////
void swap (RasterizedFont&, RasterizedFont&);

//////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ������
//////////////////////////////////////////////////////////////////////////////////////////////////
class IFontRasterizer
{
  public:
//////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� � �������������� ��������
//////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const RasterizedGlyphInfo* RasterizedGlyphs () = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������
//////////////////////////////////////////////////////////////////////////////////////////////////
    virtual unsigned int ImagesCount () = 0;
    virtual void         BuildImage  (unsigned int image_index, media::Image& out_image) = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
//////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AddRef () = 0;
    virtual void Release () = 0;

  protected:
    virtual ~IFontRasterizer () {}
};

}

#endif
