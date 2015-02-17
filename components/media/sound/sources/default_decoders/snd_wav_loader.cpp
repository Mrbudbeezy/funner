#include "shared.h"

using namespace common;
using namespace media;

namespace components
{

namespace wav_loader
{

class WavInputStream : public media::ISoundInputStream, public xtl::reference_counter 
{
  public:
    WavInputStream (const char* file_name, media::SoundSampleInfo& sound_sample_info);

    unsigned int Read (unsigned int first_sample, unsigned int samples_count, void* data);

    void AddRef () { addref (this); }  
    void Release () { release (this); }
 
    common::InputFile file;
    unsigned int      data_chunk_pos;
    unsigned int      sample_size;
    unsigned int      chunk_samples;
};

int SeekToWAVChunk (StdFile& file, const char* chunk_name)
{
  char read_buffer[8];
  int  ret_value;

  file.Seek (12);
  file.Read (read_buffer, 8);

  while (xtl::xstrnicmp(chunk_name, read_buffer, 4) && !file.Eof())
  {
    file.Seek ((*((int*)(read_buffer + 4))), FileSeekMode_Current);
    file.Read (read_buffer, 8);  
  }

  if (file.Eof())
    ret_value = -1;
  else
    ret_value = *(int*)(read_buffer + 4);

  return ret_value;
}

WavInputStream::WavInputStream (const char* file_name, SoundSampleInfo& sound_sample_info)
  : file (file_name)
{
  char read_buffer[16];
  int  chunk_size;    
  
  static const char* METHOD_NAME = "WavInputStream::WavInputStream";

  file.Rewind ();
  file.Read (read_buffer, 16);
  
  if (xtl::xstrnicmp("RIFF", read_buffer, 4))
    throw xtl::format_not_supported_exception (METHOD_NAME, "No 'RIFF' chunk in file.");
    
  if (xtl::xstrnicmp("WAVE", read_buffer+8, 4))
    throw xtl::format_not_supported_exception (METHOD_NAME, "No 'WAVE' ID in file.");

  if (SeekToWAVChunk (file, "fmt ") < 0)
    throw xtl::format_not_supported_exception (METHOD_NAME, "No 'fmt ' chunk in file.");
    
  file.Read (read_buffer, 16);

  short* format_tag = (short*)read_buffer;
  
  if (*format_tag != 1)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Compressed wav currently not supported.");
    
  sound_sample_info.channels        = *(short*)(read_buffer + 2);
  sound_sample_info.frequency       = *(int*)  (read_buffer + 4);
  sound_sample_info.bits_per_sample = *(short*)(read_buffer + 14);

  chunk_size = SeekToWAVChunk (file, "data");
  
  if (chunk_size < 0)
    throw xtl::format_not_supported_exception (METHOD_NAME, "No 'data' chunk in file.");
    
  data_chunk_pos = file.Tell();
  sample_size    = sound_sample_info.bits_per_sample / 8 * sound_sample_info.channels;

  sound_sample_info.samples_count = chunk_samples = chunk_size / sample_size;
}

unsigned int WavInputStream::Read (unsigned int first_sample, unsigned int samples_count, void* data)
{
  file.Seek ((filepos_t)data_chunk_pos + (filepos_t)(sample_size * first_sample));

  return (unsigned int)file.Read (data, (unsigned int)(stl::min(chunk_samples - first_sample, samples_count) * sample_size)) / sample_size;
}

ISoundInputStream* default_wav_loader (const char* file_name, SoundSampleInfo& sound_sample_info)
{
  return new WavInputStream (file_name, sound_sample_info);
}

/*
   ��������� ࠡ��� � ��㪠��
*/

class WavLoaderComponent
{
  public:
    //����㧪� ���������
    WavLoaderComponent () 
    {
      SoundSampleManager::RegisterLoader ("wav", &default_wav_loader);
    }
};

extern "C"
{

ComponentRegistrator<WavLoaderComponent> WavLoader ("media.sound.loaders.Wav");

}

}

}
