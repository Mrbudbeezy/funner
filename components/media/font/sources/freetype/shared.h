#ifndef MEDIA_FONT_FREETYPE_SHARED_HEADER
#define MEDIA_FONT_FREETYPE_SHARED_HEADER

#include <stl/string>
#include <stl/vector>

#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/shared_ptr.h>

#include <common/component.h>
#include <common/file.h>
#include <common/lockable.h>
#include <common/log.h>
#include <common/strlib.h>
#include <common/utf_converter.h>

#include <media/charset_manager.h>
#include <media/font_library.h>
#include <media/rasterized_font.h>

#include <ft2build.h>
#include <freetype.h>

namespace media
{

namespace freetype
{

//////////////////////////////////////////////////////////////////////////////////////////////////
///���������� freetype
//////////////////////////////////////////////////////////////////////////////////////////////////
class FreetypeLibrary
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ���������� / �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FreetypeLibrary ();
    FreetypeLibrary (const FreetypeLibrary&);
    ~FreetypeLibrary ();

    FreetypeLibrary& operator = (const FreetypeLibrary&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Freetype API
///////////////////////////////////////////////////////////////////////////////////////////////////
    void    FT_Done_Face       (FT_Face face);
    FT_UInt FT_Get_Char_Index  (FT_Face face, FT_ULong charcode);
    bool    FT_Load_Char       (FT_Face face, FT_ULong charcode, FT_Int32 load_flags, bool nothrow = false);
    void    FT_New_Memory_Face (const FT_Byte* file_base, FT_Long file_size, FT_Long face_index, FT_Face *aface);
    void    FT_Select_Charmap  (FT_Face face, FT_Encoding encoding);
    void    FT_Set_Char_Size   (FT_Face face, FT_F26Dot6 char_width, FT_F26Dot6 char_height, FT_UInt horz_resolution, FT_UInt vert_resolution);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (FreetypeLibrary&);

  private:
    struct Impl;
    Impl* impl;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///�����
//////////////////////////////////////////////////////////////////////////////////////////////////
class FreetypeFontDesc : public IFontDesc, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FreetypeFontDesc (const char* file_name);
    ~FreetypeFontDesc ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t FontsCount ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ��������� / ��� ��������� / ��� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* FamilyName (size_t index);
    const char* StyleName (size_t index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Font CreateFont    (size_t index, const FontCreationParams& params);
    bool CanCreateFont (size_t index, const FontCreationParams& params);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef () { addref (this); }
    void Release () { release (this); }

 private:
    struct Impl;
    Impl* impl;
};

//��������� ����� ������ free type
const char* get_free_type_error_name (FT_Error error);

//�������� ������ free type
void check_free_type_error (FT_Error error, const char* source);

}

}

#endif
