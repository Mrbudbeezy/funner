#include "shared.h"

using namespace media;

namespace
{

struct KerningsMapKey
{
  KerningsMapKey (size_t in_left_glyph_index, size_t in_right_glyph_index)
    : left_glyph_index (in_left_glyph_index), right_glyph_index (in_right_glyph_index)
    {}

  bool operator == (const KerningsMapKey& right) const
  {
    return (left_glyph_index == right.left_glyph_index) && (right_glyph_index == right.right_glyph_index);
  }

  size_t left_glyph_index;
  size_t right_glyph_index;
};

size_t hash (const KerningsMapKey& key)
{
  size_t data_to_hash [2] = {key.left_glyph_index, key.right_glyph_index};

  return common::crc32 (data_to_hash, sizeof (data_to_hash));
}

}

namespace media
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct FontImpl::Impl
{
  Impl ()
    : first_glyph_code (0)
    , font_size (0)
    {}

  typedef xtl::uninitialized_storage<GlyphInfo>      GlyphsArray;
  typedef stl::hash_map<KerningsMapKey, KerningInfo> KerningsMap;

  size_t                         first_glyph_code;  //char-��� ������� �����
  size_t                         font_size;         //������ ������
  stl::string                    file_name;         //��� ��������� �����
  stl::string                    name;              //��� ���������
  stl::string                    family_name;       //��� ��������� ���������
  stl::string                    style_name;        //��� ����� ���������
  GlyphsArray                    glyphs;            //�����
  KerningsMap                    kernings;          //��������
  FontBuilder::RasterizerHandler rasterizer;        //������������
};

}

/*
   ����������� / ����������
*/

FontImpl::FontImpl ()
  : impl (new Impl)
{
}

FontImpl::~FontImpl ()
{
}

/*
   ��������� / ��������� ����� ��������� �����
*/

const char* FontImpl::Source () const
{
  return impl->file_name.c_str ();
}

void FontImpl::SetSource (const char* new_source)
{
  if (!new_source)
    throw xtl::make_null_argument_exception ("media::FontImpl::SetSource", "new_source");

  impl->file_name = new_source;
}

/*
   ���������/��������� ����� / ��������� / ����� ������
*/

const char* FontImpl::Name () const
{
  return impl->name.c_str ();
}

const char* FontImpl::FamilyName () const
{
  return impl->family_name.c_str ();
}

const char* FontImpl::StyleName () const
{
  return impl->style_name.c_str ();
}

void FontImpl::Rename (const char* new_name)
{
  if (!new_name)
    throw xtl::make_null_argument_exception ("media::FontImpl::Rename", "new_name");

  impl->name = new_name;
}

void FontImpl::SetFamilyName (const char* new_family_name)
{
  if (!new_family_name)
    throw xtl::make_null_argument_exception ("media::FontImpl::SetFamilyName", "new_family_name");

  impl->family_name = new_family_name;
}

void FontImpl::SetStyleName (const char* new_style_name)
{
  if (!new_style_name)
    throw xtl::make_null_argument_exception ("media::FontImpl::SetStyleName", "new_style_name");

  impl->style_name = new_style_name;
}

/*
   ���������/��������� ������� ������� ������
*/

void FontImpl::SetGlyphsCount (size_t new_glyphs_count)
{
  impl->glyphs.resize (new_glyphs_count);
}

size_t FontImpl::GlyphsCount () const
{
  return impl->glyphs.size ();
}

/*
   ���������/��������� ���� ������� �����
*/

void FontImpl::SetFirstGlyphCode (size_t new_first_glyph_code)
{
  impl->first_glyph_code = new_first_glyph_code;
}

size_t FontImpl::FirstGlyphCode () const
{
  return impl->first_glyph_code;
}

/*
   ���������/��������� ������� ������
*/

void FontImpl::SetFontSize (size_t new_font_size)
{
  impl->font_size = new_font_size;
}

size_t FontImpl::FontSize () const
{
  return impl->font_size;
}

/*
   ������ � ������ � ������
*/

const GlyphInfo* FontImpl::Glyphs () const
{
  return impl->glyphs.data ();
}

GlyphInfo* FontImpl::Glyphs ()
{
  return const_cast<GlyphInfo*> (const_cast<const FontImpl&> (*this).Glyphs ());
}

/*
   ����������/���������/�������� ���������� � ��������
*/

void FontImpl::InsertKerning (size_t left_glyph_index, size_t right_glyph_index, const KerningInfo& kerning_info)
{
  impl->kernings[KerningsMapKey (left_glyph_index, right_glyph_index)] = kerning_info;
}

void FontImpl::RemoveKerning (size_t left_glyph_index, size_t right_glyph_index)
{
  impl->kernings.erase (KerningsMapKey (left_glyph_index, right_glyph_index));
}

void FontImpl::RemoveAllKernings ()
{
  impl->kernings.clear ();
}

KerningInfo FontImpl::Kerning (size_t left_glyph_index, size_t right_glyph_index) const
{
  Impl::KerningsMap::iterator iter = impl->kernings.find (KerningsMapKey (left_glyph_index, right_glyph_index));

  if (iter == impl->kernings.end ())
  {
    KerningInfo return_value = {0.f, 0.f};

    return return_value;
  }

  return iter->second;
}

bool FontImpl::HasKerning (size_t left_glyph_index, size_t right_glyph_index) const
{
  Impl::KerningsMap::iterator iter = impl->kernings.find (KerningsMapKey (left_glyph_index, right_glyph_index));

  if (iter == impl->kernings.end ())
    return false;

  return true;
}


/*
   ��������� �������������
*/

void FontImpl::SetRasterizer (const FontBuilder::RasterizerHandler& new_rasterizer)
{
  impl->rasterizer = new_rasterizer;
}

const FontBuilder::RasterizerHandler& FontImpl::Rasterizer () const
{
  return impl->rasterizer;
}
