#include "shared.h"

using namespace common;
using namespace media::rfx;
using namespace media;
using namespace math;

/*
    �������� ���������� Texmap
*/

struct TexcoordParams
{
  int          source; //�������� ���������� ���������
  TexcoordWrap wrap;   //��������� ���������� ���������
  
  TexcoordParams () : source (TexcoordSource_Default), wrap (TexcoordWrap_Default) {}
};

struct Texmap::Impl
{
  stl::string      image;                          //��� ��������
  mat4f            transform;                      //������� �������������� ���������� ���������
  TexcoordParams   texcoord_params [Texcoord_Num]; //��������� ���������� ���������
  TexmapFilterType filters [TexmapFilter_Num];     //�������

  Impl ();
};

/*
    Impl
*/

Texmap::Impl::Impl ()
{
  for (size_t i=0; i<TexmapFilter_Num; i++)
    filters [i] = TexmapFilterType_Default;
}

/*
    ������������ / ����������
*/

Texmap::Texmap ()
  : impl (new Impl)
  {}

Texmap::Texmap (const Texmap& texmap)
  : impl (new Impl (*texmap.impl))
  {}

Texmap::~Texmap ()
{
  delete impl;
}

/*
    ������������
*/

Texmap& Texmap::operator = (const Texmap& texmap)
{
  Texmap (texmap).Swap (*this);

  return *this;
}

/*
    ��� �������� ��������� � ���������
*/

const char* Texmap::Image () const
{
  return impl->image.c_str ();
}

void Texmap::SetImage (const char* image_name)
{
  if (!image_name)
    RaiseNullArgument ("media::Texmap::SetImage", "image_name");
    
  impl->image = image_name;
}

/*
    ������� �������������� ���������� ���������
*/

const mat4f& Texmap::Transform () const
{
  return impl->transform;
}

void Texmap::SetTransform (const math::mat4f& tm)
{
  impl->transform = tm;
}

/*
    �������� ���������� ���������
*/

int Texmap::Source (Texcoord coord) const
{
  if (coord < 0 || coord >= Texcoord_Num)
    RaiseInvalidArgument ("media::Texmap::Source", "coord", coord);
    
  return impl->texcoord_params [coord].source;
}

void Texmap::SetSource (Texcoord coord, int source)
{
  if (coord < 0 || coord >= Texcoord_Num)
    RaiseInvalidArgument ("media::Texmap::SetSource", "coord", coord);
    
  impl->texcoord_params [coord].source = source;
}

/*
    ��������� ���������� ��������� �� �������
*/

TexcoordWrap Texmap::Wrap (Texcoord coord) const
{
  if (coord < 0 || coord >= Texcoord_Num)
    RaiseInvalidArgument ("media::Texmap::Wrap", "coord", coord);
    
  return impl->texcoord_params [coord].wrap;
}

void Texmap::SetWrap (Texcoord coord, TexcoordWrap wrap)
{
  if (coord < 0 || coord >= Texcoord_Num)
    RaiseInvalidArgument ("media::Texmap::SetWrap", "coord", coord);
    
  impl->texcoord_params [coord].wrap = wrap;
}

/*
    ������ ����������
*/

TexmapFilterType Texmap::FilterType (TexmapFilter filter) const
{
  if (filter < 0 || filter >= TexmapFilter_Num)
    RaiseInvalidArgument ("media::Texmap::FilterType", "filter", filter);
    
  return impl->filters [filter];
}

void Texmap::SetFilterType (TexmapFilter filter, TexmapFilterType type)
{
  if (filter < 0 || filter >= TexmapFilter_Num)
    RaiseInvalidArgument ("media::Texmap::SetFilterType", "filter", filter);
    
  switch (type)
  {
    case TexmapFilterType_Default:
    case TexmapFilterType_Point:
    case TexmapFilterType_Bilinear:
    case TexmapFilterType_Trilinear:
    case TexmapFilterType_Anisotropic:
      break;
    default:
      RaiseInvalidArgument ("media::Texmap::SetFilterType", "type", type);
      break;
  }
  
  impl->filters [filter] = type;
}

/*
    �����
*/  

void Texmap::Swap (Texmap& texmap)
{
  stl::swap (impl, texmap.impl);
}

namespace media
{

namespace rfx
{

void swap (Texmap& texmap1, Texmap& texmap2)
{
  texmap1.Swap (texmap2);  
}

}

}
