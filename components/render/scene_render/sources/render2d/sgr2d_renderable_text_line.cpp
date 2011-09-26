#include "shared.h"

#undef min
#undef max

using namespace render::render2d;
using namespace scene_graph;

namespace
{

//������� ������
struct TextDimensions
{
  math::vec2f min;
  math::vec2f max;
};

//����������� ������������ ���� �������
inline bool is_symbol_valid (size_t symbol_code, size_t first_code, size_t last_code)
{
  return symbol_code >= first_code && symbol_code < last_code;
}

typedef xtl::uninitialized_storage <math::vec4f> CharsColors;

}

/*
    �������� ���������� ������ ������
*/

struct RenderableTextLine::Impl
{
  Render&                        render;                       //������ �� ������
  scene_graph::TextLine*         text_line;                    //�������� ����� ������
  PrimitivePtr                   primitive;                    //��������������� ��������
  SpritesBuffer                  sprites_buffer;               //����� ��������
  CharsColors                    current_chars_colors;         //������� ����� ����
  RenderableFont*                current_renderable_font;      //������� �����
  size_t                         current_text_hash;            //������� ��� ������
  TextDimensions                 current_text_dimensions;      //������� ������� ������
  scene_graph::TextLineAlignment current_horizontal_alignment; //������� �������������� ������������
  scene_graph::TextLineAlignment current_vertical_alignment;   //������� ������������ ������������
  math::vec3f                    current_offset;               //������� �������� ������, ������������ ��������������
  size_t                         current_world_tm_hash;        //��� ������� ������� ��������������
  bool                           need_full_update;             //���������� ��������� �������� ��� ���������
  bool                           wrong_state;                  //������� ��������� � ������������ ���������

///�����������
  Impl (scene_graph::TextLine* in_text_line, Render& in_render)
    : render (in_render),
      text_line (in_text_line),
      primitive (render.Renderer ()->CreatePrimitive (), false),
      need_full_update (true),
      wrong_state (true)
  {
    primitive->SetBlendMode (render::mid_level::renderer2d::BlendMode_Translucent);
  }

///���������� ���������� ������ ������
  void Update ()
  {
    try
    {
      if (wrong_state)
        need_full_update = true;

      wrong_state = true;

        //��������� ������

      RenderableFont* renderable_font = render.GetFont (text_line->Font ());

        //������������� ������ ����������

      bool need_update_sprites = false;

        //����������� ������������� ���������� ������

      bool need_update_text = renderable_font != current_renderable_font || text_line->TextUtf32Hash () != current_text_hash;

        //����������� ������������� ���������� �������� ������

      bool need_update_offset = current_horizontal_alignment != text_line->HorizontalAlignment () ||
                                current_vertical_alignment != text_line->VerticalAlignment ();

        //�������� ������������� ���������� ������� �������������� ������

      const math::mat4f& world_tm      = text_line->WorldTM ();
      size_t             world_tm_hash = common::crc32 (&world_tm, sizeof (math::mat4f));

      bool need_update_tm = world_tm_hash != current_world_tm_hash;

        //����������� ������������� ���������� ����� ������

      size_t text_length = text_line->TextLength ();

      if (need_update_text)
        current_chars_colors.resize (text_length);

      CharsColors actual_chars_colors (text_length);

      text_line->CharsColors (0, text_length, actual_chars_colors.data ());

      bool need_update_color = need_update_text || memcmp (current_chars_colors.data (), actual_chars_colors.data (), current_chars_colors.size () * sizeof (CharsColors::value_type));

      current_chars_colors.swap (actual_chars_colors);

        //����������� ������������� ���������� ���� �����

      if (need_full_update)
      {
        need_update_text    = true;
        need_update_offset  = true;
        need_update_color   = true;
        need_update_tm      = true;
        need_update_sprites = true;
      }

        //���������� ������

      if (need_update_text)
      {
          //������������� ���������� ���������� ������

        const unsigned int*     text_unicode      = text_line->TextUtf32 ();
        const media::Font&      font              = renderable_font->GetFont ();
        const media::GlyphInfo* glyphs            = font.Glyphs ();
        size_t                  glyphs_count      = font.GlyphsTableSize (),
                                first_glyph_code  = font.FirstGlyphCode (),
                                last_glyph_code   = first_glyph_code + glyphs_count;
        float                   current_pen_x     = 0.0f,
                                current_pen_y     = 0.0f,
                                max_glyph_side    = (float)renderable_font->GetMaxGlyphSide ();
        TextDimensions          text_dimensions;

          //�������������� ����� ��� ������ ��������

        sprites_buffer.resize (text_length, false);

          //������������ ������� ��������

        const unsigned int*            pos              = text_unicode;
        size_t                         prev_glyph_index = 0;
        mid_level::renderer2d::Sprite* dst_sprite       = sprites_buffer.data ();

        for (size_t i=0; i<text_length; i++, pos++)
        {
            //�������� ������� ���� ������� � ������

          size_t current_symbol_code = *pos;

          if (!is_symbol_valid (current_symbol_code, first_glyph_code, last_glyph_code))
          {
            if (!is_symbol_valid ('?', first_glyph_code, last_glyph_code))
              continue;

            current_symbol_code = '?';
          }

            //��������� �����

          size_t                  glyph_index = current_symbol_code - first_glyph_code;
          const media::GlyphInfo& glyph       = glyphs [glyph_index];

            //������� ����

          if (dst_sprite != sprites_buffer.data ()) //������������ ������ ���� ���� ���������� ������
          {
            if (font.HasKerning (prev_glyph_index, glyph_index))
            {
              media::KerningInfo kerning_info = font.Kerning (prev_glyph_index, glyph_index);

              current_pen_x += kerning_info.x_kerning / max_glyph_side;
              current_pen_y += kerning_info.y_kerning / max_glyph_side;
            }
          }

            //������������� �������

              //bearing ������ � ������������� �������� � RenderableFont!!!!

          float bearing_x = glyph.bearing_x / max_glyph_side, bearing_y = glyph.bearing_y / max_glyph_side;

          const mid_level::renderer2d::Sprite& src_sprite = renderable_font->GetSprite (glyph_index);

          dst_sprite->position   = math::vec3f (current_pen_x + bearing_x + src_sprite.size.x / 2.f,
                                                current_pen_y + bearing_y - src_sprite.size.y / 2.f, 0.f);
          dst_sprite->color      = current_chars_colors.data () [i];
          dst_sprite->size       = src_sprite.size;
          dst_sprite->tex_offset = src_sprite.tex_offset;
          dst_sprite->tex_size   = src_sprite.tex_size;

            //������� ����

          current_pen_x += glyph.advance_x / max_glyph_side;
          current_pen_y += glyph.advance_y / max_glyph_side;

            //������������� ������ ������

          TextDimensions glyph_dimensions;

          glyph_dimensions.min.x = dst_sprite->position.x - dst_sprite->size.x * 0.5f;
          glyph_dimensions.min.y = dst_sprite->position.y - dst_sprite->size.y * 0.5f;
          glyph_dimensions.max.x = glyph_dimensions.min.x + dst_sprite->size.x;
          glyph_dimensions.max.y = glyph_dimensions.min.y + dst_sprite->size.y;

          if (glyph_dimensions.min.x < text_dimensions.min.x) text_dimensions.min.x = glyph_dimensions.min.x;
          if (glyph_dimensions.min.y < text_dimensions.min.y) text_dimensions.min.y = glyph_dimensions.min.y;
          if (glyph_dimensions.max.x > text_dimensions.max.x) text_dimensions.max.x = glyph_dimensions.max.x;
          if (glyph_dimensions.max.y > text_dimensions.max.y) text_dimensions.max.y = glyph_dimensions.max.y;

            //������� � ���������� �������

          dst_sprite++;
          prev_glyph_index = glyph_index;
        }

          //������������� ���������� ��������

        sprites_buffer.resize (dst_sprite - sprites_buffer.data ());

          //���������� ��������

        primitive->SetTexture (renderable_font->GetTexture ());

          //���������� ����������

        current_text_dimensions = text_dimensions;
        current_renderable_font = renderable_font;
        current_text_hash       = text_line->TextUtf32Hash ();

          //���������� ������

        need_update_offset  = true;
        need_update_sprites = true;
        need_update_color   = false;
      }

        //���������� ��������� ������

      if (need_update_offset)
      {
          //������ ������� �������� �������

        scene_graph::TextLineAlignment halign = text_line->HorizontalAlignment (),
                                       valign = text_line->VerticalAlignment ();
        math::vec2f                    size   = current_text_dimensions.max - current_text_dimensions.min,
                                       offset = -current_text_dimensions.min;

        switch (halign)
        {
          default:
          case TextLineAlignment_Center:
            offset.x -= 0.5f * size.x;
            break;
          case TextLineAlignment_Right:
            offset.x -= size.x;
            break;
          case TextLineAlignment_BaseLine:
            offset.x = 0.f;
            break;
          case TextLineAlignment_Left:
            break;
        }

        switch (valign)
        {
          default:
          case TextLineAlignment_Center:
            offset.y -= 0.5f * size.y;
            break;
          case TextLineAlignment_Top:
            offset.y -= size.y;
            break;
          case TextLineAlignment_BaseLine:
            offset.y = 0.f;
            break;
          case TextLineAlignment_Bottom:
            break;
        }

          //���������� ����������

        current_horizontal_alignment = halign;
        current_vertical_alignment   = valign;
        current_offset               = math::vec3f (offset, 0.0f);

          //���������� ������

        need_update_tm = true;
      }

        //���������� ������� ��������������

      if (need_update_tm)
      {
        primitive->SetTransform (world_tm * math::translate (current_offset));

          //���������� ����������

        current_world_tm_hash = world_tm_hash;
      }

        //���������� ����� ������

      if (need_update_color)
      {
          //���������� ����� � ��������

        mid_level::renderer2d::Sprite* sprite = sprites_buffer.data ();

        for (size_t i=0, count=sprites_buffer.size (); i<count; i++, sprite++)
          sprite->color = current_chars_colors.data () [i];

          //���������� ������

        need_update_sprites = true;
      }

        //���������� ��������

      if (need_update_sprites)
      {
        primitive->RemoveAllSprites ();
        primitive->AddSprites       (sprites_buffer.size (), sprites_buffer.data ());
      }

        //���������� ������������ ��������� �������

      wrong_state = false;
    }
    catch (std::exception& exception)
    {
      render.LogPrintf ("%s\n    at render::render2d::RenderableTextLine::Update(text_line='%s')", exception.what (), text_line->Name ());
    }
    catch (...)
    {
      render.LogPrintf ("Unknown exception\n    at render::render2d::RenderableTextLine::Update(text_line='%s')", text_line->Name ());
    }
  }
};

/*
    �����������
*/

RenderableTextLine::RenderableTextLine (scene_graph::TextLine* text_line, Render& render)
  : Renderable (text_line),
    impl (new Impl (text_line, render))
{
}

RenderableTextLine::~RenderableTextLine ()
{
}

/*
    ���������� �� ����������
*/

void RenderableTextLine::Update ()
{
  impl->Update ();
}

/*
    ���������
*/

void RenderableTextLine::DrawCore (IFrame& frame)
{
  if (!impl->wrong_state && impl->primitive->GetSpritesCount ())
    frame.AddPrimitive (impl->primitive.get ());
}
