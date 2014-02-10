#ifndef MEDIA_FONT_CORE_SHARED_HEADER
#define MEDIA_FONT_CORE_SHARED_HEADER

#include <stl/algorithm>
#include <stl/hash_map>
#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/iterator.h>
#include <xtl/uninitialized_storage.h>

#include <common/component.h>
#include <common/file.h>
#include <common/hash.h>
#include <common/log.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <media/charset_manager.h>
#include <media/font_library.h>
#include <media/font.h>
#include <media/rasterized_font.h>

namespace media
{

//////////////////////////////////////////////////////////////////////////////////////////////////
///���������
//////////////////////////////////////////////////////////////////////////////////////////////////
class FontImpl : public xtl::reference_counter
{
  public:
//////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
//////////////////////////////////////////////////////////////////////////////////////////////////
    FontImpl ();
    ~FontImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� / ��������� ����� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Source    () const;
    void        SetSource (const char* new_source);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� ����� / ��������� / ����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name          () const;
    const char* FamilyName    () const;
    const char* StyleName     () const;
    void        Rename        (const char* new_name);
    void        SetFamilyName (const char* new_family_name);
    void        SetStyleName  (const char* new_style_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� ������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetGlyphsCount (size_t new_glyphs_count);
    size_t GlyphsCount    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� ���� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetFirstGlyphCode (size_t new_first_glyph_code);
    size_t FirstGlyphCode    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetFontSize (size_t new_font_size);
    size_t FontSize    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������ � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const GlyphInfo* Glyphs () const;
          GlyphInfo* Glyphs ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������/���������/�������� ���������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        InsertKerning     (size_t left_glyph_index, size_t right_glyph_index, const KerningInfo& kerning_info);
    void        RemoveKerning     (size_t left_glyph_index, size_t right_glyph_index);
    void        RemoveAllKernings ();
    KerningInfo Kerning           (size_t left_glyph_index, size_t right_glyph_index) const;
    bool        HasKerning        (size_t left_glyph_index, size_t right_glyph_index) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                                  SetRasterizer (const FontBuilder::RasterizerHandler&);
    const FontBuilder::RasterizerHandler& Rasterizer    () const;

  private:
    FontImpl (const FontImpl&);             //no impl
    FontImpl& operator = (const FontImpl&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

#endif
