#include "shared.h"

using namespace media::rfx;

/*
    �������� ���������� ��������� �������
*/

struct EffectParameter::Impl: public xtl::reference_counter
{
  stl::string         name;           //��� ���������
  EffectParameterType type;           //��� ���������
  size_t              elements_count; //���������� ��������� � ���������

  Impl ()
    : type (EffectParameterType_Int)
    , elements_count (1)
  {
  }
};

/*
    ������������ / ���������� / ������������
*/

EffectParameter::EffectParameter ()
  : impl (new Impl)
{
}

EffectParameter::EffectParameter (Impl* in_impl)
  : impl (in_impl)
{
}

EffectParameter::EffectParameter (const EffectParameter& param)
  : impl (param.impl)
{
  addref (impl);
}

EffectParameter::~EffectParameter ()
{
  release (impl);
}

EffectParameter& EffectParameter::operator = (const EffectParameter& param)
{
  EffectParameter (param).Swap (*this);
  
  return *this;
}

/*
    �����������
*/

EffectParameter EffectParameter::Clone () const
{
  return EffectParameter (new Impl (*impl));
}

/*
    �������������
*/

size_t EffectParameter::Id () const
{
  return reinterpret_cast<size_t> (impl);
}

/*
    ��� ���������
*/

const char* EffectParameter::Name () const
{
  return impl->name.c_str ();
}

void EffectParameter::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::rfx::EffectParameter::SetName", "name");
    
  impl->name = name;
}

/*
    ��� ���������
*/

EffectParameterType EffectParameter::Type () const
{
  return impl->type;
}

void EffectParameter::SetType (EffectParameterType type)
{
  switch (type)
  {
    case EffectParameterType_Int:
    case EffectParameterType_Float:
    case EffectParameterType_Int2:
    case EffectParameterType_Float2:
    case EffectParameterType_Int3:
    case EffectParameterType_Float3:
    case EffectParameterType_Int4:
    case EffectParameterType_Float4:
    case EffectParameterType_Float2x2:
    case EffectParameterType_Float3x3:
    case EffectParameterType_Float4x4:
      break;
    default:
      throw xtl::make_argument_exception ("media::rfx::EffectParameter::SetType", "type", type);
  }
  
  impl->type = type;
}

/*
    ���������� ��������� ���������� ���� � ���������
*/

size_t EffectParameter::ElementsCount () const
{
  return impl->elements_count;
}

void EffectParameter::SetElementsCount (size_t count)
{
  impl->elements_count = count;
}

/*
    �����
*/

void EffectParameter::Swap (EffectParameter& param)
{
  stl::swap (impl, param.impl);
}

namespace media
{

namespace rfx
{

/*
    �����
*/

void swap (EffectParameter& param1, EffectParameter& param2)
{
  param1.Swap (param2);
}

/*
    ��� ���� �������� �������
*/

const char* get_name (EffectParameterType type)
{
  switch (type)
  {
    case EffectParameterType_Int:      return "int";
    case EffectParameterType_Float:    return "float";
    case EffectParameterType_Int2:     return "int2";
    case EffectParameterType_Float2:   return "float2";
    case EffectParameterType_Int3:     return "int3";
    case EffectParameterType_Float3:   return "float3";
    case EffectParameterType_Int4:     return "int4";
    case EffectParameterType_Float4:   return "float4";
    case EffectParameterType_Float2x2: return "float2x2";
    case EffectParameterType_Float3x3: return "float3x3";
    case EffectParameterType_Float4x4: return "float4x4";
    default:
      throw xtl::make_argument_exception ("media::rfx::get_name(EffectParameterType)", "type", type);
  }
}

}

}
