#ifndef MEDIALIB_FONT_HEADER
#define MEDIALIB_FONT_HEADER

#include <tr1/memory>
#include <tr1/functional_fwd>

namespace medialib
{

//implementation forwards
class FontFaceImpl;

//////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � �����
//////////////////////////////////////////////////////////////////////////////////////////////////
struct GlyphInfo
{
  size_t x_pos;      //X ���������� ������� ������ ���� �����
  size_t y_pos;      //Y ���������� ������� ������ ���� �����
  size_t width;      //������ �����
  size_t heigth;     //������ �����
  int    bearingX;   //���������� �� ��������� ���� �� ����� ������� �����
  int    bearingY;   //���������� �� ��������� ���� �� ������� ������� �����
  int    advanceX;   //������ �� ��� X �� �������� ��������� ���� �� ���������� ��������� ����
  int    advanceY;   //������ �� ��� Y �� �������� ��������� ���� �� ���������� ��������� ����
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � ����������� ����������� ����������
//////////////////////////////////////////////////////////////////////////////////////////////////
struct KerningInfo
{
  int x_kerning;     //������ �� ��� X �� ��������� ���� ��� ����������� ������ ����� �� ��������� ���� ��� ����������� ������� �����
  int y_kerning;     //������ �� ��� Y �� ��������� ���� ��� ����������� ������ ����� �� ��������� ���� ��� ����������� ������� �����
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///���������
//////////////////////////////////////////////////////////////////////////////////////////////////
class FontFace
{
  public:
             FontFace  ();
             FontFace  (size_t first_char_index, size_t glyph_table_size, GlyphInfo* glyph, KerningInfo* kerning_table, const char* font_file_name);
             FontFace  (const FontFace& source);
    explicit FontFace  (const char* file_name);
             ~FontFace ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Load (const char* file_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FontFace& operator = (const FontFace&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name   () const;
    void        Rename (const char* new_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char*                ImageName      () const;
    size_t                     GlyphsCount    () const;
    size_t                     FirstGlyphCode () const;
    GlyphInfo*                 GlyphData      (size_t glyph_index) const;
    GlyphInfo*                 GlyphData      () const;
    KerningInfo*               Kerning        (size_t left_glyph_index, size_t right_glyph_index) const;
    KerningInfo*               Kerning        () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (FontFace&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� �������� �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static FontFace* DefaultBFSLoader (const char* file_name);

  private:
    FontFace (FontFaceImpl*);

    tr1::shared_ptr<FontFaceImpl> impl;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///�����
//////////////////////////////////////////////////////////////////////////////////////////////////
void swap (FontFace&,FontFace&);

//////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������� ��������
//////////////////////////////////////////////////////////////////////////////////////////////////
class FontSystem
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ���������������� ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef tr1::function<FontFace* (const char* file_name)> CodecLoadFunc;

    static bool RegisterCodec       (const char* extension, const CodecLoadFunc& loader);  //���������� true ��� �������� �����������, � false, ���� ����� ���������� ��� ����������������
    static void UnregisterCodec     (const char* extension);
    static void UnregisterAllCodecs ();
};

}

#endif
