#include "shared.h"

using namespace media::rfx;

/*
    ���������� ������� ������� ����������
*/

struct TechniquePass::Impl: public xtl::reference_counter
{
  common::PropertyMap properties; //�������� �������
  
  Impl ()
  {
  }
  
  Impl (const Impl& impl)
    : properties (impl.properties.Clone ())
  {
  }
};

/*
    ������������ / ���������� / ������������
*/

TechniquePass::TechniquePass ()
  : impl (new Impl)
{
}

TechniquePass::TechniquePass (Impl* in_impl)
  : impl (in_impl)
{
}

TechniquePass::TechniquePass (const TechniquePass& pass)
  : impl (pass.impl)
{
  addref (impl);
}

TechniquePass::~TechniquePass ()
{
  release (impl);
}

TechniquePass& TechniquePass::operator = (const TechniquePass& pass)
{
  TechniquePass (pass).Swap (*this);
  
  return *this;
}

/*
    �����������
*/

TechniquePass TechniquePass::Clone () const
{
  return TechniquePass (new Impl (*impl));
}

/*
    �������������
*/

size_t TechniquePass::Id () const
{
  return reinterpret_cast<size_t> (impl);
}

/*
    �������� ������� - ��������� ����������
*/

common::PropertyMap TechniquePass::Properties () const
{
  return impl->properties;
}

/*
    �����
*/

void TechniquePass::Swap (TechniquePass& pass)
{
  stl::swap (impl, pass.impl);
}

namespace media
{

namespace rfx
{

void swap (TechniquePass& pass1, TechniquePass& pass2)
{
  pass1.Swap (pass2);
}

}

}
