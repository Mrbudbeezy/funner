#include "shared.h"

using namespace scene_graph;
using namespace common;

/*
    �������� ���������� TextLine
*/

struct TextLine::Impl
{
  stl::string text;
  stl::string font_name;

};

/*
    ����������� / ����������
*/

TextLine::TextLine ()
  : impl (new Impl)
{
}

TextLine::~TextLine ()
{
}

/*
    �������� ����� ������
*/

TextLine::Pointer TextLine::Create ()
{
  return Pointer (new TextLine, false);
}

/*
   ���������/��������� ������
*/

void TextLine::SetText (const char* text)
{
  if (!text)
    throw xtl::make_null_argument_exception ("scene_graph::TextLine::SetText", "text");

  impl->text = text;

  UpdateNotify ();
}

const char* TextLine::Text () const
{
  return impl->text.c_str ();
}

/*
   ���������/��������� ����� ������
*/

void TextLine::SetFont (const char* font_name)
{
  if (!font_name)
    throw xtl::make_null_argument_exception ("scene_graph::TextLine::SetFont", "font_name");

  impl->font_name = font_name;

  UpdateNotify ();
}

const char* TextLine::Font () const
{
  return impl->font_name.c_str ();
}

/*
    �����, ���������� ��� ��������� �������
*/

void TextLine::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
