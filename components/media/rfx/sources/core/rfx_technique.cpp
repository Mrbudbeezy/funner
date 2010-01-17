#include "shared.h"

using namespace media::rfx;

/*
    ���������
*/

namespace
{

const size_t PASSES_ARRAY_RESERVE = 4; //������������� ����� ��������

}

/*
    �������� ���������� ������� ����������
*/

typedef stl::vector<TechniquePass> PassArray;

struct Technique::Impl: public xtl::reference_counter
{
  stl::string         name;       //��� �������
  PassArray           passes;     //�������
  common::PropertyMap properties; //�������� ����������
  
  Impl ()
  {
    passes.reserve (PASSES_ARRAY_RESERVE);
  }
  
  Impl (const Impl& impl)
    : name (impl.name)
    , properties (impl.properties.Clone ())
  {
    passes.reserve (impl.passes.size ());
    
    for (PassArray::const_iterator iter=impl.passes.begin (), end=impl.passes.end (); iter!=end; ++iter)
      passes.push_back (iter->Clone ());
  }
};

/*
    ������������ / ���������� / ������������
*/

Technique::Technique ()
  : impl (new Impl)
{
}

Technique::Technique (Impl* in_impl)
  : impl (in_impl)
{
}

Technique::Technique (const Technique& technique)
  : impl (technique.impl)
{
  addref (impl);
}

Technique::~Technique ()
{
  release (impl);
}

Technique& Technique::operator = (const Technique& technique)
{
  Technique (technique).Swap (*this);
  
  return *this;
}

/*
    �����������
*/

Technique Technique::Clone () const
{
  return Technique (new Impl (*impl));
}

/*
    �������������
*/

size_t Technique::Id () const
{
  return reinterpret_cast<size_t> (impl);
}

/*
    ��� �������
*/

const char* Technique::Name () const
{
  return impl->name.c_str ();
}

void Technique::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::rfx::Technique::SetName", "name");
    
  impl->name = name;
}

/*
    ���������� ��������
*/

size_t Technique::PassesCount () const
{
  return impl->passes.size ();
}

/*
    �������
*/

const TechniquePass& Technique::Pass (size_t index) const
{
  if (index >= impl->passes.size ())
    throw xtl::make_range_exception ("media::rfx::TechniquePass::Pass", "index", index, impl->passes.size ());
    
  return impl->passes [index];
}

TechniquePass& Technique::Pass (size_t index)
{
  return const_cast<TechniquePass&> (const_cast<const Technique&> (*this).Pass (index));
}

/*
    ���������� � �������� ��������
*/

size_t Technique::AddPass (const TechniquePass& pass)
{
  impl->passes.push_back (pass);
  
  return impl->passes.size () - 1;
}

void Technique::RemovePass (size_t index)
{
  if (index >= impl->passes.size ())
    return;
    
  impl->passes.erase (impl->passes.begin () + index);
}

void Technique::RemoveAllPasses ()
{
  impl->passes.clear ();
}

/*
    �������� ������� - ��������� ����������
*/

common::PropertyMap& Technique::Properties ()
{
  return impl->properties;
}

const common::PropertyMap& Technique::Properties () const
{
  return impl->properties;
}

/*
    �������
*/

void Technique::Clear ()
{
  RemoveAllPasses ();

  impl->name.clear ();
  impl->properties.Clear ();
}

/*
    �����
*/

void Technique::Swap (Technique& technique)
{
  stl::swap (impl, technique.impl);
}

namespace media
{

namespace rfx
{

void swap (Technique& technique1, Technique& technique2)
{
  technique1.Swap (technique2);
}

}

}
