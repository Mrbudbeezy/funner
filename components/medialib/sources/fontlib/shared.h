#ifndef __IMAGELIB_SHARED__
#define __IMAGELIB_SHARED__

#include <common/singleton.h>
#include <stl/string>
#include <stl/hash_set>
#include <stl/hash_map>
#include <stl/memory>
#include <media/font.h>

namespace medialib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class FontFaceImpl
{
  public:
    FontFaceImpl () {glyphs_count = 0;}
    FontFaceImpl (size_t first_char, size_t glyph_table_size, GlyphInfo* glyph, KerningInfo* kerning, const char* font_file_name);

    size_t                     first_char_index;  //char-��� ������� �����
    size_t                     glyphs_count;      //���������� ������
    stl::auto_ptr<GlyphInfo>   glyphs;            //�����
    stl::auto_ptr<KerningInfo> kerning_table;     //������� ����������� ����������
    stl::string                file_name;         //��� ����� � ���������� ���������
    stl::string                str_name;          //��� ���������
};

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class FontSystemImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FontSystemImpl ();
    ~FontSystemImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �������� �������� / �������� ���� �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterFontFace   (medialib::FontFace&);
    void UnregisterFontFace (medialib::FontFace&);
    void CloseAllFontFaces  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� � ��������� ���������������� ������� �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool               RegisterLoadFunc   (const char* extension, const medialib::FontSystem::CodecLoadFunc& codec);
    void               UnRegisterLoadFunc (const char* extension);
    void               UnRegisterAllFuncs ();
    medialib::FontSystem::CodecLoadFunc* GetLoadFunc (const char* extension);

  private:
    typedef stl::hash_set<medialib::FontFace*> OpenFontFacesSet;
    typedef stl::hash_map<stl::string, medialib::FontSystem::CodecLoadFunc> LoadCodecs;

  private:    
    OpenFontFacesSet open_font_faces;  //������ �������� ��������
    LoadCodecs       load_codecs;      //������ ���������������� ������� ��������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::Singleton<FontSystemImpl> FontSystemSingleton;

#endif
