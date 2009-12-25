#include "shared.h"

using namespace media::adobe::xfl;

/*
    �������� ���������� �������� ��������
*/

struct Symbol::Impl : public xtl::reference_counter
{
  stl::string   name;     //��� ��������
  xfl::Timeline timeline; //������� ��������
};

/*
    ������������ / ���������� / ������������
*/

Symbol::Symbol ()
  : impl (new Impl)
  {}
  
Symbol::Symbol (const Symbol& source)
  : impl (source.impl)
{
  addref (impl);
}

Symbol::~Symbol ()
{
  release (impl);
}

Symbol& Symbol::operator = (const Symbol& source)
{
  addref (source.impl);
  release (impl);

  impl = source.impl;

  return *this;
}

/*
   ���
*/

const char* Symbol::Name () const
{
  return impl->name.c_str ();
}

void Symbol::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::adobe::xfl::Symbol::SetName", "name");

  impl->name = name;
}

/*
   ������� ��������
*/

const media::adobe::xfl::Timeline& Symbol::Timeline () const
{
  return impl->timeline;
}

void Symbol::SetTimeline (const xfl::Timeline& timeline)
{
  impl->timeline = timeline;
}

/*
   �����
*/

void Symbol::Swap (Symbol& symbol)
{
  stl::swap (impl, symbol.impl);
}

namespace media
{

namespace adobe
{

namespace xfl
{

/*
   �����
*/

void swap (Symbol& symbol1, Symbol& symbol2)
{
  symbol1.Swap (symbol2);
}

}

}

}
