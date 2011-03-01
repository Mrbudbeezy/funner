#include "shared.h"

using namespace media::rfx;

/*
    �������� ���������� ���������� �����
*/

struct Texmap::Impl: public xtl::reference_counter
{
  stl::string image;   //��� ��������
  stl::string sampler; //��� ��������
};

/*
    ������������ / ���������� / ������������
*/

Texmap::Texmap ()
  : impl (new Impl)
{
}

Texmap::Texmap (const Texmap& map)
  : impl (map.impl)
{
  addref (map.impl);
}

Texmap::Texmap (Impl* in_impl)
  : impl (in_impl)
{
}

Texmap::~Texmap ()
{
  release (impl);
}

Texmap& Texmap::operator = (const Texmap& map)
{
  Texmap (map).Swap (*this);
  
  return *this;
}

/*
    �������������
*/

size_t Texmap::Id () const
{
  return reinterpret_cast<size_t> (impl);
}

/*
    �����������
*/

Texmap Texmap::Clone () const
{
  try
  {
    return Texmap (new Impl (*impl));
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::rfx::Texmap::Clone");
    throw;
  }
}

/*
    ��� ��������
*/

void Texmap::SetImage (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::rfx::Texmap::SetImage", "name");
    
  impl->image = name;
}

const char* Texmap::Image () const
{
  return impl->image.c_str ();
}

/*
    ��� ��������
*/

void Texmap::SetSampler (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::rfx::Texmap::SetSampler", "name");
    
  impl->sampler = name; 
}

const char* Texmap::Sampler () const
{
  return impl->sampler.c_str ();
}

/*
    �����
*/

void Texmap::Swap (Texmap& map)
{
  stl::swap (impl, map.impl);
}

namespace media
{

namespace rfx
{

void swap (Texmap& map1, Texmap& map2)
{
  map1.Swap (map2);
}

}

}

