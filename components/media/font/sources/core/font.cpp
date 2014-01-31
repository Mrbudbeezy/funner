#include "shared.h"

using namespace media;

/*
   ����������� / ���������� / �����������
*/

Font::Font (FontImpl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

Font::~Font ()
{
  release (impl);
}

Font::Font (const Font& source)
  : impl (source.impl)
{
  addref (impl);
}

Font& Font::operator = (const Font& source)
{
  Font (source).Swap (*this);

  return *this;
}

/*
   ��� �����
*/

const char* Font::Source () const
{
  return impl->Source ();
}

/*
   ��� ��������� / ��� ��������� / ��� �����
*/

const char* Font::Name () const
{
  return impl->Name ();
}

const char* Font::FamilyName () const
{
  return impl->FamilyName ();
}

const char* Font::StyleName () const
{
  return impl->StyleName ();
}

/*
   ������ ������� ������
*/

size_t Font::GlyphsCount () const
{
  return impl->GlyphsCount ();
}

/*
   ��� ������� �����
*/

size_t Font::FirstGlyphCode () const
{
  return impl->FirstGlyphCode ();
}

/*
   ������ ������
*/

size_t Font::FontSize () const
{
  return impl->FontSize ();
}

/*
   ������ � ������ � ������
*/

const GlyphInfo* Font::Glyphs () const
{
  return impl->Glyphs ();
}

/*
   ���������� � ��������
*/

KerningInfo Font::Kerning (size_t left_glyph_index, size_t right_glyph_index) const
{
  return impl->Kerning (left_glyph_index, right_glyph_index);
}

bool Font::HasKerning (size_t left_glyph_index, size_t right_glyph_index) const
{
  return impl->HasKerning (left_glyph_index, right_glyph_index);
}

/*
   �������� ���������������� ������
*/

RasterizedFont Font::CreateRasterizedFont (const RasterizedFontCreationParams& params) const
{
  return RasterizedFont (impl->Rasterizer () (*this, params));
}

/*
   �����
*/

void Font::Swap (Font& source)
{
  stl::swap (impl, source.impl);
}


namespace media
{

/*
   �����
*/

void swap (Font& font1, Font& font2)
{
  font1.Swap (font2);
}

}
