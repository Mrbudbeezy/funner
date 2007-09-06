#include "shared.h"

using namespace common;
using namespace stl;
using namespace sound::low_level;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

//����� ���������� ������� ��������� � �������������
const size_t SOURCE_BUFFERS_UPDATE_MILLISECONDS = size_t (SOURCE_BUFFERS_UPDATE_PERIOD * 1000);
const float DEFAULT_SOURCE_PROPERTIES_UPDATE_PERIOD = 0.03f;
const float DEFAULT_LISTENER_PROPERTIES_UPDATE_PERIOD = 0.03f;

namespace
{

void DefaultLogHandler (const char* log_message)
{
}

}

/*
    ����������� / ����������
*/

OpenALDevice::OpenALDevice (const char* driver_name, const char* device_name)
 : context (device_name, &DefaultLogHandler),
   buffer_update_frequency ((size_t) (1.f / SOURCE_BUFFERS_UPDATE_PERIOD)),
   source_properties_update_frequency ((size_t)(1.f / DEFAULT_SOURCE_PROPERTIES_UPDATE_PERIOD)),
   listener_properties_update_frequency ((size_t)(1.f / DEFAULT_LISTENER_PROPERTIES_UPDATE_PERIOD)),
   buffer_timer   (xtl::bind (&OpenALDevice::BufferUpdate, this), SOURCE_BUFFERS_UPDATE_MILLISECONDS),
   listener_timer (xtl::bind (&OpenALDevice::ListenerUpdate, this), (size_t)(DEFAULT_LISTENER_PROPERTIES_UPDATE_PERIOD * 1000)),
   source_timer   (xtl::bind (&OpenALDevice::SourceUpdate, this), (size_t)(DEFAULT_SOURCE_PROPERTIES_UPDATE_PERIOD * 1000)),
   log_handler (&DefaultLogHandler),
   listener_need_update (false),
   sample_buffer (DEFAULT_SAMPLE_BUFFER_SIZE),
   ref_count (1),
   is_muted (false), 
   gain (1.0f),
   channels_count (0),
   first_active_source (0),
   al_buffers_pool_size (0)
{
    //��������� ���!!!

  context.MakeCurrent ();

  char *extensions = (char*)context.alGetString (AL_EXTENSIONS);
  int  compare_value;

  info.eax_major_version = 0;
  info.eax_minor_version = 0;

  do
  {
    extensions = strstr (extensions, "EAX");
    if (extensions)
    {
      extensions += 3;
      if (isdigit (*extensions))
      {
        compare_value = atoi (extensions);
        if (compare_value > (int)info.eax_major_version)
        {
          info.eax_major_version = compare_value;
          extensions = strstr (extensions, ".");
          info.eax_minor_version = atoi (++extensions);
        }
        else if (compare_value == info.eax_major_version)
        {
          info.eax_major_version = compare_value;
          extensions = strstr (extensions, ".");
          compare_value = atoi (++extensions);
          if (compare_value > (int)info.eax_minor_version)
            info.eax_minor_version = compare_value;
        }
      }
    }
  } while (extensions);
  
    //������������ ����� ����������
    
  name = format ("%s:::%s", driver_name ? driver_name : "", device_name ? device_name : "");
    
    //�������� ������� ������������

  for (size_t i=0; i<MAX_DEVICE_CHANNELS_COUNT; i++)
  {    
    try
    {
      channels [i] = new OpenALSource (*this);
    }
    catch (...)
    {
      break;
    }

    channels_count++;
  }

  info.channels_count = channels_count;  
}

OpenALDevice::~OpenALDevice ()
{
    //�������� �������

  for (size_t i=0; i<channels_count; i++)
    delete channels [i];
    
    //������� ���� �������
    
  context.alDeleteBuffers (al_buffers_pool_size, al_buffers_pool);
}

/*
    ������� ������
*/

void OpenALDevice::AddRef ()
{
  ref_count++;
}

void OpenALDevice::Release ()
{
  if (!--ref_count)
    delete this;
}

/*
   ��� ����������
*/

const char* OpenALDevice::Name ()
{
  return name.c_str ();
}

/*
   ��������� ���������� �� ����������
*/

void OpenALDevice::GetCapabilities (Capabilities& target_info)
{
  target_info = info;
}

/*
    ���������� � ������������� ���������� ���������
*/

void OpenALDevice::UpdateListenerNotify ()
{
  listener_need_update = true;
  
  ListenerUpdate ();
}
    
/*
   ��������� ������ ��������� ��� ����������
*/

void OpenALDevice::SetVolume (float in_gain)
{
  if (in_gain < 0.0f) in_gain = 0.0f;
  if (in_gain > 1.0f) in_gain = 1.0f;

  gain     = in_gain;
  is_muted = false;
  
  UpdateListenerNotify ();
}

float OpenALDevice::GetVolume ()
{
  return gain;
}

/*
   ���������� ������������ �����
*/

void OpenALDevice::SetMute (bool state)
{
  is_muted = state;

  UpdateListenerNotify ();
}

bool OpenALDevice::IsMuted ()
{
  return is_muted;
}

/*
    ��������� ���������� ���������
*/

void OpenALDevice::SetListener (const Listener& in_listener)
{
  listener = in_listener;

  UpdateListenerNotify ();
}

void OpenALDevice::GetListener (Listener& target_listener)
{
  target_listener = listener;
}

/*
    ���������� ����������������
*/

void OpenALDevice::SetDebugLog (const LogHandler& in_log_handler)
{
  log_handler = in_log_handler ? in_log_handler : &DefaultLogHandler;
}

const OpenALDevice::LogHandler& OpenALDevice::GetDebugLog ()
{
  return log_handler;
}

void OpenALDevice::DebugPrintf (const char* format, ...)
{
  va_list list;
  
  va_start (list, format);
  DebugVPrintf (format, list);
}

void OpenALDevice::DebugVPrintf (const char* format, va_list list)
{
  try
  {
    log_handler (vformat (format, list).c_str ());
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

/*
    ��������� �������� �������������� �����
*/

void OpenALDevice::SetSample (size_t channel, const char* sample_name)
{
  if (channel >= channels_count)
    RaiseOutOfRange ("sound::low_level::OpenALDevice::SetSample", "channel", channel, channels_count);
    
  if (!sample_name)
    RaiseNullArgument ("sound::low_level::OpenALDevice::SetSample", "sample_name");
    
  channels [channel]->SetSample (sample_name);
}

const char* OpenALDevice::GetSample (size_t channel)
{
  if (channel >= channels_count)
    RaiseOutOfRange ("sound::low_level::OpenALDevice::GetSample", "channel", channel, channels_count);
    
  return channels [channel]->GetSample ();
}

/*
    �������� ����������� ������������ ������
*/

bool OpenALDevice::IsLooped (size_t channel)
{
  if (channel >= channels_count)
    RaiseOutOfRange ("sound::low_level::OpenALDevice::IsLooped", "channel", channel, channels_count);
    
  return channels [channel]->IsLooped ();
}
    
/*
    ��������� ���������� ���������
*/

void OpenALDevice::SetSource (size_t channel, const Source& source)
{
  if (channel >= channels_count)
    RaiseOutOfRange ("sound::low_level::OpenALDevice::SetSource", "channel", channel, channels_count);
    
  channels [channel]->SetSource (source);
}

void OpenALDevice::GetSource (size_t channel, Source& source)
{
  if (channel >= channels_count)
    RaiseOutOfRange ("sound::low_level::OpenALDevice::GetSource", "channel", channel, channels_count);

  source = channels [channel]->GetSource ();
}

/*
    ���������� �������������
*/

void OpenALDevice::Play (size_t channel, bool looping)
{
  if (channel >= channels_count)
    RaiseOutOfRange ("sound::low_level::OpenALDevice::Play", "channel", channel, channels_count);
    
  channels [channel]->Play (looping);
}

void OpenALDevice::Pause (size_t channel)
{
  if (channel >= channels_count)
    RaiseOutOfRange ("sound::low_level::OpenALDevice::Pause", "channel", channel, channels_count);
    
  channels [channel]->Pause ();
}

void OpenALDevice::Stop (size_t channel)
{
  if (channel >= channels_count)
    RaiseOutOfRange ("sound::low_level::OpenALDevice::Stop", "channel", channel, channels_count);
    
  channels [channel]->Stop ();
}

void OpenALDevice::Seek (size_t channel, float time_in_seconds)
{
  if (channel >= channels_count)
    RaiseOutOfRange ("sound::low_level::OpenALDevice::Seek", "channel", channel, channels_count);

  channels [channel]->Seek (time_in_seconds);
}

float OpenALDevice::Tell (size_t channel)
{
  if (channel >= channels_count)
    RaiseOutOfRange ("sound::low_level::OpenALDevice::Tell", "channel", channel, channels_count);
    
  return channels [channel]->Tell (); 
}

bool OpenALDevice::IsPlaying (size_t channel)
{
  if (channel >= channels_count)
    RaiseOutOfRange ("sound::low_level::OpenALDevice::IsPlaying", "channel", channel, channels_count);
    
  return channels [channel]->IsPlaying ();
}

/*
    ����������
*/

void OpenALDevice::BufferUpdate ()
{
    //���� ��� �������� ���������� ��� ������������� ���-���� ���������

  if (!first_active_source)
    return;
    
    //���������� ����������

  for (OpenALSource* source=first_active_source; source; source=source->NextActive ())
    source->BufferUpdate ();
}

void OpenALDevice::SourceUpdate ()
{
    //���� ��� �������� ���������� ��� ������������� ���-���� ���������

  if (!first_active_source)
    return;
    
    //���������� ����������

  for (OpenALSource* source=first_active_source; source; source=source->NextActive ())
    source->PropertiesUpdate ();
}

void OpenALDevice::ListenerUpdate ()
{
    //���� ��� �������� ���������� ��� ������������� ���-���� ���������

  if (!first_active_source)
    return;
    
    //���������� ���������

  if (listener_need_update)
  {
    listener_need_update = false;
    
    float orientation [6] = {listener.direction.x, listener.direction.y, listener.direction.z, 
                             listener.up.x,        listener.up.y,        listener.up.z};

    context.alListenerfv (AL_POSITION,    listener.position);
    context.alListenerfv (AL_VELOCITY,    listener.velocity);
    context.alListenerfv (AL_ORIENTATION, orientation);
    context.alListenerf  (AL_GAIN,        is_muted ? 0.0f : gain);
  }
}

/*
   ��������� ���������� ����������
*/

void OpenALDevice::SetHint (SoundDeviceHint hint, size_t frequency)
{
  if (frequency < 1)
    RaiseOutOfRange ("sound::low_level::OpenALDevice::SetHint", "value", frequency, 1u, 1000u);

  switch (hint)
  {
    case SoundDeviceHint_BufferUpdateFrequency: buffer_update_frequency = frequency; buffer_timer.SetPeriod ((size_t)(1000 / frequency)); break;
    case SoundDeviceHint_SourcePropertiesUpdateFrequency: source_properties_update_frequency = frequency; source_timer.SetPeriod ((size_t)(1000 / frequency)); break;
    case SoundDeviceHint_ListenerPropertiesUpdateFrequency: listener_properties_update_frequency = frequency; listener_timer.SetPeriod ((size_t)(1000 / frequency)); break;
  }
}

size_t OpenALDevice::GetHint (SoundDeviceHint hint)
{
  switch (hint)
  {
    case SoundDeviceHint_BufferUpdateFrequency:             return buffer_update_frequency;
    case SoundDeviceHint_SourcePropertiesUpdateFrequency:   return source_properties_update_frequency;
    case SoundDeviceHint_ListenerPropertiesUpdateFrequency: return listener_properties_update_frequency;
  }

  return 0;
}

/*
    ���������� �������������� ������� ������������
*/

ALuint OpenALDevice::AllocateSourceBuffer ()
{
  if (al_buffers_pool_size)
    return al_buffers_pool [--al_buffers_pool_size];

  const ALuint WRONG_BUFFER_ID = 0;

  ALuint buffer = WRONG_BUFFER_ID;
  
  context.alGenBuffers (1, &buffer);
  
  if (buffer == WRONG_BUFFER_ID)
    throw OpenALGenBufferException ();

  return buffer;
}

void OpenALDevice::DeallocateSourceBuffer (ALuint buffer)
{
  if (al_buffers_pool_size == DEVICE_BUFFERS_POOL_SIZE)
  {
    size_t flush_size = DEVICE_BUFFERS_POOL_SIZE / 2 + 1;
    
    context.alDeleteBuffers (flush_size, al_buffers_pool);
    
    al_buffers_pool_size -= flush_size;
  }

  al_buffers_pool [al_buffers_pool_size++] = buffer;
}
