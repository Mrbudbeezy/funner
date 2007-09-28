#include "shared.h"

using namespace media;
using namespace stl;
using namespace common;

/*
    �������� ���������� SoundSource
*/

struct SoundSource::Impl: public xtl::reference_counter
{
  string          type;                //��� �����
  bool            looping;             //�����������
  vector <string> sounds;              //������ ������
  float           gain;                //��������
  float           minimum_gain;        //����������� ��������
  float           maximum_gain;        //������������ ��������
  float           inner_angle;         //���� ����������� ������
  float           outer_angle;         //���� �������� ������
  float           outer_gain;          //�������� �� ������� ������
  float           reference_distance;  //���������, � ������� ��������� ����� gain
  float           maximum_distance;    //������������ ���������, ������ ������� ��������� �� �����������
  float           cull_distance;       //��������� ���������
  
  Impl ();
};

/*
    Impl::Impl
*/

SoundSource::Impl::Impl ()
  : looping (false), gain (1.f), minimum_gain (0.f), maximum_gain (1.f), inner_angle (360.f), outer_angle (360.f), outer_gain (1.f)
  {}

/*
    ������������ / ����������
*/

SoundSource::SoundSource ()
  : impl (new Impl, false)
  {}

SoundSource::SoundSource (const SoundSource& sound_source, CloneMode mode)
  : impl (clone (sound_source.impl, mode, "media::SoundSource::SoundSource"))
  {}

SoundSource::~SoundSource ()
{
}

/*
    ������������
*/

SoundSource& SoundSource::operator = (const SoundSource& sound_source)
{
  impl = sound_source.impl;

  return *this;
}

/*
   ��� �����
*/

void SoundSource::SetType (const char* type)
{
  impl->type = type;
}

const char* SoundSource::Type () const
{
  return impl->type.c_str ();
}
  
/*
   ����������� ���������������
*/

void SoundSource::SetLooping (bool looping)
{
  impl->looping = looping;
}

bool SoundSource::Looping () const
{
  return impl->looping;
}
  
/*
   ���������� ����� � ������ �����
*/

size_t SoundSource::AddSound (const char* sound)
{
  impl->sounds.push_back (sound);

  return impl->sounds.size ();
}

size_t SoundSource::SoundsCount () const
{
  return impl->sounds.size ();
}

const char* SoundSource::Sound (size_t sound_index) const
{
  if (sound_index > impl->sounds.size ())
    RaiseOutOfRange ("media::SoundSource::Sound", "sound_index", sound_index, impl->sounds.size ());

  return impl->sounds[sound_index].c_str ();
}

void SoundSource::RemoveSound (size_t sound_index)
{
  if (sound_index > impl->sounds.size ())
    RaiseOutOfRange ("media::SoundSource::RemoveSound", "sound_index", sound_index, impl->sounds.size ());

  impl->sounds.erase (impl->sounds.begin () + sound_index);
}

void SoundSource::RemoveAllSounds ()
{
  impl->sounds.clear ();
}

/*
   ���������
*/

void SoundSource::SetParam (SoundSourceParam param, float value)
{
  switch (param)
  {
    case SoundSourceParam_Gain:              impl->gain               = value; break;
    case SoundSourceParam_MinimumGain:       impl->minimum_gain       = value; break;
    case SoundSourceParam_MaximumGain:       impl->maximum_gain       = value; break;
    case SoundSourceParam_InnerAngle:        impl->inner_angle        = value; break;
    case SoundSourceParam_OuterAngle:        impl->outer_angle        = value; break;
    case SoundSourceParam_OuterGain:         impl->outer_gain         = value; break;
    case SoundSourceParam_ReferenceDistance: impl->reference_distance = value; break;
    case SoundSourceParam_MaximumDistance:   impl->maximum_distance   = value; break;
    case SoundSourceParam_CullDistance:      impl->cull_distance      = value; break;
    default: RaiseInvalidArgument ("media::SoundSource::SetParam", "param", param);
  }
}

float SoundSource::Param (SoundSourceParam param) const
{
  switch (param)
  {
    case SoundSourceParam_Gain:              return impl->gain; break;
    case SoundSourceParam_MinimumGain:       return impl->minimum_gain; break;
    case SoundSourceParam_MaximumGain:       return impl->maximum_gain; break;
    case SoundSourceParam_InnerAngle:        return impl->inner_angle; break;
    case SoundSourceParam_OuterAngle:        return impl->outer_angle; break;
    case SoundSourceParam_OuterGain:         return impl->outer_gain; break;
    case SoundSourceParam_ReferenceDistance: return impl->reference_distance; break;
    case SoundSourceParam_MaximumDistance:   return impl->maximum_distance; break;
    case SoundSourceParam_CullDistance:      return impl->cull_distance; break;
    default: RaiseInvalidArgument ("media::SoundSource::SetParam", "param", param); return 0;
  }
}
     
/*
    �����
*/

void SoundSource::Swap (SoundSource& sound_source)
{
  swap (impl, sound_source.impl);  
}

namespace media
{

void swap (SoundSource& sound_source1, SoundSource& sound_source2)
{
  sound_source1.Swap (sound_source2);
}

}
