#include "shared.h"

using namespace media::rfx;
using namespace math;
using namespace common;

/*
    ����� �������
*/

namespace
{

struct MapImpl
{
  Texmap   texmap; //���������� �����
  float    weight; //��� �����
  MapState state;  //��������� �����
  
  MapImpl () : weight (1.0f), state (MapState_Disabled) {}
};

}

/*
    �������� ���������� CommonShader
*/

struct CommonShader::Impl
{
  CommonShaderType shader_type;                    //��� �������
  vec3f            colors [CommonShaderColor_Num]; //�����
  float            shininess;                      //"�������������"
  float            transparency;                   //������������
  BlendFunction    blend_function;                 //������� ����������
  CompareMode      alpha_test_mode;                //����� ��������� ���������� ��� �����-��������� 
  float            alpha_test_reference;           //��������� �����-���������
  MapImpl          maps [CommonShaderMap_Num];     //���������� �����
  
  Impl ();
};

/*
    Impl::Impl
*/

CommonShader::Impl::Impl ()
  : blend_function (make_blend_solid ())
{
  shader_type          = CommonShaderType_Default;
  shininess            = 0.0f;
  transparency         = 0.0f;
  alpha_test_mode      = CompareMode_AlwaysPass;
  alpha_test_reference = 0.0f;
}

/*
    ������������ / ����������
*/

CommonShader::CommonShader ()
  : impl (new Impl)
  {}

CommonShader::CommonShader (const CommonShader& shader)
  : Shader (shader),
    impl (new Impl (*shader.impl))
  {}

CommonShader::~CommonShader ()
{
  delete impl;
}

/*
    ��������
*/

CommonShader::Pointer CommonShader::Create ()
{
  return Pointer (new CommonShader, false);
}

/*
    ���������� �����������
*/

Shader* CommonShader::CloneCore () const
{
  return new CommonShader (*this);
}

/*
    ��� �������
*/

CommonShaderType CommonShader::Type () const
{
  return impl->shader_type;
}

void CommonShader::SetType (CommonShaderType type)
{
  if (type < 0 || type >= CommonShaderType_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::SetShaderType", "type", type);
    
  impl->shader_type = type;
}

/*
    �����
*/

const vec3f& CommonShader::Color (CommonShaderColor color_id) const
{
  if (color_id < 0 || color_id >= CommonShaderColor_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::Color", "color_id", color_id);
    
  return impl->colors [color_id];
}

void CommonShader::SetColor (CommonShaderColor color_id, const vec3f& color)
{
  if (color_id < 0 || color_id >= CommonShaderColor_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::SetColor", "color_id", color_id);
    
  impl->colors [color_id] = clamp (color, vec3f (0.0f), vec3f (1.0f));
}

void CommonShader::SetColor (CommonShaderColor color_id, float red, float green, float blue)
{
  SetColor (color_id, vec3f (red, green, blue));
}

/*
    "�������������"
*/

float CommonShader::Shininess () const
{
  return impl->shininess;
}

void CommonShader::SetShininess (float value)
{
  impl->shininess = clamp (value, 0.0f, 128.0f);
}

/*
    ������������
*/

float CommonShader::Transparency () const
{
  return impl->transparency;
}

void CommonShader::SetTransparency (float value)
{
  impl->transparency = clamp (value, 0.0f, 1.0f);
}

/*
    ��������� ������ ���������� ������
*/

const BlendFunction& CommonShader::Blend () const
{
  return impl->blend_function;
}

void CommonShader::SetBlend (const BlendFunction& blend)
{
  if (blend.equation < 0 || blend.equation >= BlendEquation_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::SetBlend", "blend.equation", blend.equation);

  if (blend.argument [0] < 0 || blend.argument [0] >= BlendArgument_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::SetBlend", "blend.argument[0]", blend.argument [0]);

  if (blend.argument [1] < 0 || blend.argument [1] >= BlendArgument_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::SetBlend", "blend.argument[1]", blend.argument [1]);

  impl->blend_function = blend;
}

void CommonShader::SetBlend (BlendEquation equation, BlendArgument src_argument, BlendArgument dst_argument)
{
  SetBlend (BlendFunction (equation, src_argument, dst_argument));
}

/*
    ��������� �����-���������
*/

CompareMode CommonShader::AlphaTestMode () const
{
  return impl->alpha_test_mode;
}

float CommonShader::AlphaTestReference () const
{
  return impl->alpha_test_reference;
}

void CommonShader::SetAlphaTestMode (CompareMode mode)
{
  if (mode < 0 || mode >= CompareMode_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::SetAlphaTestMode", "mode", mode);
    
  impl->alpha_test_mode = mode;
}

void CommonShader::SetAlphaTestReference (float alpha_reference)
{
  impl->alpha_test_reference = clamp (alpha_reference, 0.0f, 1.0f);
}

void CommonShader::SetAlphaTest (CompareMode mode, float alpha_reference)
{
  SetAlphaTestMode      (mode);
  SetAlphaTestReference (alpha_reference);
}

/*
    ���������� �����
*/

const Texmap& CommonShader::Map (CommonShaderMap map) const
{
  if (map < 0 || map >= CommonShaderMap_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::Map", "map", map);
    
  return impl->maps [map].texmap;
}

Texmap& CommonShader::Map (CommonShaderMap map)
{
  return const_cast<Texmap&> (const_cast<const CommonShader&> (*this).Map (map));
}

/*
    ���� ���������� ����
*/

float CommonShader::MapWeight (CommonShaderMap map) const
{
  if (map < 0 || map >= CommonShaderMap_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::MapWeight", "map", map);
    
  return impl->maps [map].weight;
}

void CommonShader::SetMapWeight (CommonShaderMap map, float weight)
{
  if (map < 0 || map >= CommonShaderMap_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::SetMapWeight", "map", map);

  impl->maps [map].weight = clamp (weight, 0.0f, 1.0f);
}

/*
    ���������� / ���������� ���������� ����
*/

media::rfx::MapState CommonShader::MapState (CommonShaderMap map) const
{
  if (map < 0 || map >= CommonShaderMap_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::MapState", "map", map);
    
  return impl->maps [map].state;
}

void CommonShader::SetMapState (CommonShaderMap map, media::rfx::MapState state)
{
  if (map < 0 || map >= CommonShaderMap_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::SetMapState", "map", map);
    
  switch (state)
  {
    case MapState_Enabled:
    case MapState_Disabled:
      break;
    default:
      RaiseInvalidArgument ("media::rfx::CommonShader::SetMapState", "state", state);
      break;
  }

  impl->maps [map].state = state;
}

/*
    ������������ ���������������
*/

void CommonShader::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Shader::AcceptCore (visitor);
}
