#include <cstdio>
#include <exception>

#include <common/hash.h>

#include <media/sound.h>

using namespace media;
using namespace common;

const char* file_name = "data/sound1.ogg";

int main ()
{
  printf ("Results of sound_common_test:\n");

  try
  {
    SoundSample sample, sample2;

    printf ("Empty sample properties:\n");
    printf ("  Samples   - %u\n", sample.SamplesCount ());
    printf ("  Size      - %u\n", sample.SizeInBytes ());
    printf ("  Frequency - %u\n", sample.Frequency ());
    printf ("  Channels  - %u\n", sample.Channels ());
    printf ("  Bits per sample - %u\n", sample.BitsPerSample ());

    sample = sample2;

    sample.Load (file_name);

    printf ("Sample name - '%s'\n", sample.Name ());
    sample.Rename ("new_name");
    printf ("New sample name - '%s'\n", sample.Name ());

    printf ("Sample properties:\n");
    printf ("  Samples   - %u\n", sample.SamplesCount ());
    printf ("  Size      - %u\n", sample.SizeInBytes ());
    printf ("  Frequency - %u\n", sample.Frequency ());
    printf ("  Channels  - %u\n", sample.Channels ());
    printf ("  Bits per sample - %u\n", sample.BitsPerSample ());

    printf ("Calculated sample properties:\n");
    printf ("  Bytes    - %u\n", sample.SamplesToBytes (sample.SamplesCount ()));
    printf ("  Samples  - %u\n", sample.BytesToSamples (sample.SizeInBytes ()));
    printf ("  Duration - %f\n", sample.Duration ());
    printf ("  Samples  - %u\n", sample.SecondsToSamples (sample.SamplesToSeconds (sample.SamplesCount ())));

    SoundSample clonned_sample (sample.Clone ());

    sample.Swap (sample2);

    printf ("Clonned sample properties:\n");
    printf ("  Samples   - %u\n", clonned_sample.SamplesCount ());
    printf ("  Size      - %u\n", clonned_sample.SizeInBytes ());
    printf ("  Frequency - %u\n", clonned_sample.Frequency ());
    printf ("  Channels  - %u\n", clonned_sample.Channels ());
    printf ("  Bits per sample - %u\n", clonned_sample.BitsPerSample ());
  }
  catch (std::exception& exception)
  {                                               
    printf ("exception: %s\n",exception.what ()); 
  }                                               
  catch (...)
  {
    printf ("unknown exception\n");
  }

  return 0;
}
