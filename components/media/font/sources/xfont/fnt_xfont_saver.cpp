#include "shared.h"

using namespace media;
using namespace common;

namespace
{

/*
    ���������
*/

const char* FLOAT_FORMAT = ".###"; //���������� ������ ����� ������� ��� ������ ������������ �����

/*
    ��������������� ����� ���������� �������
*/

class XmlFontSaver
{
  public:
    /*
        �����������
    */
  
    XmlFontSaver (const char* file_name, const Font& in_font)
      : writer (file_name), font (in_font)
    {
      SaveFont ();
    }

  private:  
    /*
        ���������� ������
    */
    
    void SaveFont ()
    {
      XmlWriter::Scope scope (writer, "Font");
      
      writer.WriteAttribute ("Name", font.Name ());
      writer.WriteAttribute ("FontFile", font.ImageName ());
      writer.WriteAttribute ("FirstCharCode", font.FirstGlyphCode ());

      SaveGlyphs ();
      SaveKernings ();
    }

    /*
        ���������� ������
    */

    void SaveGlyphs ()
    {
      XmlWriter::Scope scope (writer, "Glyphs");

      for (size_t i = 0; i < font.GlyphsTableSize (); i++)
        SaveGlyph (i);
    }

    void SaveGlyph (size_t glyph_index)
    {
      XmlWriter::Scope scope (writer, "Glyph");
      const GlyphInfo& glyph_info = font.Glyphs ()[glyph_index];

      writer.WriteAttribute ("XPos", glyph_info.x_pos);
      writer.WriteAttribute ("YPos", glyph_info.y_pos);
      writer.WriteAttribute ("Width", glyph_info.width);
      writer.WriteAttribute ("Height", glyph_info.height);
      writer.WriteAttribute ("BearingX", glyph_info.bearing_x);
      writer.WriteAttribute ("BearingY", glyph_info.bearing_y);
      writer.WriteAttribute ("AdvanceX", glyph_info.advance_x);
      writer.WriteAttribute ("AdvanceY", glyph_info.advance_y);
    }
    
    /*
        ���������� ���������
    */

    void SaveKernings ()
    {
      XmlWriter::Scope scope (writer, "Kernings");
      
      for (size_t i = 0; i < font.GlyphsTableSize (); i++)
        for (size_t j = 0; j < font.GlyphsTableSize (); j++)
          if (font.HasKerning (i, j))
            SaveKerning (i, j);
    }

    void SaveKerning (size_t left_glyph_index, size_t right_glyph_index)
    {
      XmlWriter::Scope scope (writer, "Kerning");
      KerningInfo      kerning_info (font.Kerning (left_glyph_index, right_glyph_index));

      writer.WriteAttribute ("LeftGlyph", left_glyph_index);
      writer.WriteAttribute ("RightGlyph", right_glyph_index);
      writer.WriteAttribute ("XKerning", kerning_info.x_kerning, FLOAT_FORMAT);
      writer.WriteAttribute ("YKerning", kerning_info.y_kerning, FLOAT_FORMAT);
    }
    
  private:
    XmlWriter   writer;  //������������ XML
    const Font& font;    //�����    
};

/*
    ���������� ������
*/

void xfont_save (const char* file_name, const Font& font)
{
  XmlFontSaver (file_name, font);
}

/*
   ��������� ���������� �������
*/

class XFontSaverComponent
{
  public:
    //�������� ����������
    XFontSaverComponent () 
    {
      FontManager::RegisterSaver ("xfont", &xfont_save);
    }
};

extern "C"
{

ComponentRegistrator<XFontSaverComponent> XFontSaver ("media.font.savers.xfont");

}

}
