#include "shared.h"

using namespace media;

/*
   �����
*/

namespace media
{

struct FontDesc::Impl : public xtl::reference_counter
{
  stl::string             source; //��� �����
  unsigned int            index;  //������ ������������� ������
  xtl::com_ptr<IFontDesc> desc;   //�����

  Impl (const char* in_source, IFontDesc* in_desc, unsigned int in_index)
    : source (in_source)
    , index (in_index)
    , desc (in_desc)
    {}
};

}

/*
   ����������� / ���������� / ������������
*/

FontDesc::FontDesc (const char* source, IFontDesc* desc, unsigned int index)
  : impl (new Impl (source, desc, index))
{
}

FontDesc::FontDesc (const FontDesc& source)
  : impl (source.impl)
{
  addref (impl);
}

FontDesc::~FontDesc ()
{
  release (impl);
}

FontDesc& FontDesc::operator = (const FontDesc& source)
{
  FontDesc (source).Swap (*this);

  return *this;
}

/*
   ��� �����
*/

const char* FontDesc::Source () const
{
  return impl->source.c_str ();
}

/*
   ��� ��������� / ��� ��������� / ��� �����
*/

const char* FontDesc::FamilyName () const
{
  return impl->desc->FamilyName (impl->index);
}

const char* FontDesc::StyleName  () const
{
  return impl->desc->StyleName (impl->index);
}
    
/*
   �������� ���������� ������
*/

Font FontDesc::CreateFont (const FontCreationParams& params) const
{
  return impl->desc->CreateFont (impl->index, params);
}

bool FontDesc::CanCreateFont (const FontCreationParams& params) const
{
  return impl->desc->CanCreateFont (impl->index, params);
}

/*
   �����
*/

void FontDesc::Swap (FontDesc& source)
{
  stl::swap (impl, source.impl);
}

namespace media
{

/*
   �����
*/

void swap (FontDesc& font1, FontDesc& font2)
{
  font1.Swap (font2);
}

}
