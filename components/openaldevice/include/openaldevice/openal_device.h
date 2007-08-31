#ifndef SOUND_SYSTEM_OPENAL_DEVICE_HEADER
#define SOUND_SYSTEM_OPENAL_DEVICE_HEADER

#include <mathlib.h>
#include <xtl/functional_fwd>
#include <sound/device.h>

namespace sound
{

namespace openal_device
{

using namespace sound::low_level;

const float BUFFER_UPDATE_TIME = 0.5;

///////////////////////////////////////////////////////////////////////////////////////////////////
///C������ ��������������� �����, ������������� ����� OpenAL
///////////////////////////////////////////////////////////////////////////////////////////////////
struct OpenALSoundSystem : public sound::low_level::ISoundDevice
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    OpenALSoundSystem  (const char* device_name = NULL);
    ~OpenALSoundSystem ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name ();
    const char* Devices ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� �� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetCapabilities (Capabilities&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t ChannelsCount ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� �������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetSample (size_t channel, const char* sample_name);
    const char* GetSample (size_t channel);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������� ������������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsLooped (size_t channel);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetSource (size_t channel, const Source&);
    void GetSource (size_t channel, Source&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  Play  (size_t channel, bool looping = false);
    void  Pause (size_t channel);
    void  Stop  (size_t channel);
    void  Seek  (size_t channel, float time_in_seconds);
    float Tell  (size_t channel);
    bool  IsPlaying (size_t channel);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ ��������� ��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetVolume (float gain);
    float GetVolume ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetMute (bool state);
    bool IsMuted ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetListener (const Listener&);
    void GetListener (Listener&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ����������� ����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void              SetDebugLog (const LogHandler&);
    const LogHandler& GetDebugLog ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  ();
    void Release ();

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateBuffers ();

  private:
    struct Impl;
    Impl* impl;
};

}

}

#endif
