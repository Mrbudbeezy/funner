#include "shared.h"

using namespace sound::low_level;
using namespace sound::low_level::openal;

/*
   ����� ������ � ��������, �������� �������������� ����� �����
*/

typedef xtl::uninitialized_storage<char> ByteBuffer;

struct BufferedSampleDecoder::Impl : public xtl::reference_counter
{
  ByteBuffer   decoded_sound_buffer;
  unsigned int sample_size;
  unsigned int samples_count;

  Impl (unsigned int buffer_size, unsigned int in_sample_size, unsigned int in_samples_count)
    : decoded_sound_buffer (buffer_size)
    , sample_size (in_sample_size)
    , samples_count (in_samples_count)
    {}
};

/*
   ������������
*/

BufferedSampleDecoder::BufferedSampleDecoder (const char* name)
{
  try
  {
    media::SoundSample sample (name);

    unsigned int sample_size = sample.SamplesToBytes (1);

    stl::auto_ptr<Impl> impl_holder (new Impl (sample.SamplesToBytes (sample.SamplesCount ()), sample_size, sample.SamplesCount ()));

    for (unsigned int current_sample = 0, samples_count = sample.SamplesCount (); current_sample < samples_count;)
      current_sample += sample.Read (current_sample, samples_count - current_sample, impl_holder->decoded_sound_buffer.data () + sample_size * current_sample);

    impl = impl_holder.release ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("sound::low_level::openal::BufferedSampleDecoder::BufferedSampleDecoder (const char*)");
    throw;
  }
}

BufferedSampleDecoder::BufferedSampleDecoder (const SampleDesc& desc, const IDevice::SampleReadFunction& fn)
{
  try
  {
    unsigned int sample_size = desc.bits_per_sample / 8 * desc.channels;

    stl::auto_ptr<Impl> impl_holder (new Impl (desc.samples_count * sample_size, sample_size, desc.samples_count));

    for (unsigned int current_sample = 0, samples_count = desc.samples_count; current_sample < samples_count;)
      current_sample += fn (current_sample, samples_count - current_sample, impl_holder->decoded_sound_buffer.data () + sample_size * current_sample);

    impl = impl_holder.release ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("sound::low_level::openal::BufferedSampleDecoder::BufferedSampleDecoder (const SampleDesc&, const IDevice::SampleReadFunction&)");
    throw;
  }
}

BufferedSampleDecoder::BufferedSampleDecoder (Impl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

BufferedSampleDecoder::~BufferedSampleDecoder ()
{
  release (impl);
}

/*
   ������ ��������������� ����� ����������� samples_count ������� ������� � first sample � data
*/

unsigned int BufferedSampleDecoder::Read (unsigned int first_sample, unsigned int samples_count, void* data)
{
  try
  {
    if (first_sample >= impl->samples_count)
      return 0;

    unsigned int copied_samples_count = stl::min (samples_count, impl->samples_count - first_sample);

    memcpy (data, impl->decoded_sound_buffer.data () + first_sample * impl->sample_size, copied_samples_count * impl->sample_size);

    return copied_samples_count;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("sound::low_level::openal::BufferedSampleDecoder::Read");
    throw;
  }
}

/*
   ������������
*/

BufferedSampleDecoder* BufferedSampleDecoder::Clone ()
{
  return new BufferedSampleDecoder (impl);
}
