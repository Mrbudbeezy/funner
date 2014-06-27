#include "shared.h"

using namespace render::scene::client;

//TODO: add prefetch parse params from string

/*
    ���������
*/

namespace
{

const char* DEFAULT_SEMANTIC = "font_mask"; //��� ��������� �� ���������

struct RasterizedFontCreationParamsImpl: public media::RasterizedFontCreationParams
{
  RasterizedFontCreationParamsImpl ()
  {
    max_image_size = 2048;
    pot            = true;
    glyph_margin   = 4;
    image_format   = media::PixelFormat_L8;
  }
};

RasterizedFontCreationParamsImpl rasterized_font_creation_params;

}

/*
    �������� ���������� ��������� �������
*/

namespace
{

/// ���������� ������
struct FontDesc
{
  Font*                font;       //�����
  xtl::auto_connection on_destroy; //���������� � ������������ �������� ������

  FontDesc (Font* in_font) : font (in_font) {}
};

typedef stl::hash_map<size_t, FontDesc> FontMap;

/// ���������� ��������� ������
struct FontMaterialDesc
{
  FontMaterial*        font_material; //�������� ������
  xtl::auto_connection on_destroy;    //���������� � ������������ �������� ��������� ������

  FontMaterialDesc (FontMaterial* in_font_material)
    : font_material (in_font_material)
  {
  }
};

typedef stl::hash_map<size_t, FontMaterialDesc> FontMaterialMap;

}

struct FontManager::Impl
{
  MaterialManager& material_manager; //�������� ����������
  FontMap          fonts;            //������
  FontMaterialMap  font_materials;   //��������� �������

  Impl (MaterialManager& in_material_manager) : material_manager (in_material_manager) {}

  void RemoveFont (size_t id)
  {
    fonts.erase (id);
  }

  void RemoveFontMaterial (size_t hash)
  {
    font_materials.erase (hash);
  }
};

/*
    ����������� / ����������
*/

FontManager::FontManager (MaterialManager& material_manager)
  : impl (new Impl (material_manager))
{
}

FontManager::~FontManager ()
{
}

/*
    �������� ������ / ��������� ������
*/

FontPtr FontManager::CreateFont (const media::Font& font)
{
  try
  {
    size_t id = font.Id ();

    FontMap::iterator iter = impl->fonts.find (id);

    if (iter != impl->fonts.end ())
      return iter->second.font;

    FontPtr new_font (new Font (impl->material_manager, font, rasterized_font_creation_params), false);

    xtl::auto_connection on_destroy = new_font->connect_tracker (xtl::bind (&Impl::RemoveFont, &*impl, id));

    iter = impl->fonts.insert_pair (id, &*new_font).first;

    iter->second.on_destroy.swap (on_destroy);

    return new_font;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::FontManager::CreateFont");
    throw;
  }
}

FontMaterialPtr FontManager::CreateFontMaterial (const media::Font& font, const char* material, const char* semantic)
{
  try
  {
    if (!material)
      throw xtl::make_null_argument_exception ("", "material");

    if (!semantic)
      throw xtl::make_null_argument_exception ("", "semantic");

    if (!*semantic)
      semantic = DEFAULT_SEMANTIC;

      //����� ���������

    static char SEPARATOR = '#';

    size_t id   = font.Id (),
           hash = common::crc32 (&id, sizeof (id), common::strhash (material, common::crc32 (&SEPARATOR, sizeof (SEPARATOR), common::strhash (semantic))));

    FontMaterialMap::iterator iter = impl->font_materials.find (hash);

    if (iter != impl->font_materials.end ())
      return iter->second.font_material;

      //�������� ������

    FontPtr new_font = CreateFont (font);

      //�������� ������ ���������

    FontMaterialPtr new_font_material (new FontMaterial (impl->material_manager, new_font, material, semantic), false);

    xtl::auto_connection on_destroy = new_font_material->connect_tracker (xtl::bind (&Impl::RemoveFontMaterial, &*impl, hash));

    iter = impl->font_materials.insert_pair (hash, &*new_font_material).first;

    iter->second.on_destroy.swap (on_destroy);

    return new_font_material;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::FontManager::CreateFontMaterial");
    throw;
  }
}

/*
    ������������� ���������� �������
*/

void FontManager::AttachFontLibrary (const media::FontLibrary&)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void FontManager::DetachFontLibrary (const media::FontLibrary&)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void FontManager::DetachAllFontLibraries ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ��������������� ��������� �������
*/

void FontManager::PrefetchFont (const char* name, const media::FontCreationParams& params, const char* material, const char* semantic)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
