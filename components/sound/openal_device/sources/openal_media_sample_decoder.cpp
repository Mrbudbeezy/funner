#include "shared.h"

using namespace sound::low_level::openal;

/*
   ����� ������ � �������� �� ������ media::Sample
*/

/*
   ������������
*/

MediaSampleDecoder::MediaSampleDecoder (const char* name)
{
  try
  {
    sample.Load (name);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("sound::low_level::openal::MediaSampleDecoder::MediaSampleDecoder (const char*)");
    throw;
  }
}

MediaSampleDecoder::MediaSampleDecoder (const media::SoundSample& in_sample)
{
  try
  {
    sample = in_sample.Clone ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("sound::low_level::openal::MediaSampleDecoder::MediaSampleDecoder (const media::SoundSample&)");
    throw;
  }
}

/*
   ������ ��������������� ����� ����������� samples_count ������� ������� � first sample � data
*/

size_t MediaSampleDecoder::Read (size_t first_sample, size_t samples_count, void* data)
{
  try
  {
    return sample.Read (first_sample, samples_count, data);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("sound::low_level::openal::MediaSampleDecoder::Read");
    throw;
  }
}

/*
   ������������
*/

MediaSampleDecoder* MediaSampleDecoder::Clone ()
{
  return new MediaSampleDecoder (sample);
}
