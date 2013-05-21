#ifndef SOUND_SYSTEM_OPENAL_DEVICE_SHARED_HEADER
#define SOUND_SYSTEM_OPENAL_DEVICE_SHARED_HEADER

#include <stdarg.h>
#include <AL/al.h>
#include <AL/alc.h>

#include <stl/auto_ptr.h>
#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/uninitialized_storage.h>
#include <xtl/ref.h>
#include <xtl/reference_counter.h>
#include <xtl/string.h>
#include <xtl/common_exceptions.h>

#include <common/action_queue.h>
#include <common/component.h>
#include <common/lockable.h>
#include <common/log.h>
#include <common/singleton.h>
#include <common/strlib.h>
#include <common/time.h>

#include <media/sound.h>

#include <syslib/application.h>
#include <syslib/thread.h>

#include <sound/device.h>
#include <sound/driver.h>

namespace sound
{

namespace low_level
{

namespace openal
{

//implementation forwards
class OpenALDevice;
class OpenALSource;
class OpenALContext;
class OpenALContextManagerImpl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
const size_t DEVICE_BUFFERS_POOL_SIZE        = 32;     //������ ���� �������
const size_t MAX_SOUND_SAMPLE_RATE           = 176400; //������������ ������� ������������� (Generic ���������� ����������� 192000 �����������)
//const size_t SOURCE_BUFFERS_COUNT            = 4;      //���������� ������� ������������ �� ��������
//const size_t SOURCE_BUFFERS_UPDATE_FREQUENCY = 10;     //������� ���������� �������
const size_t SOURCE_BUFFERS_COUNT            = 8;      //���������� ������� ������������ �� ��������
const size_t SOURCE_BUFFERS_UPDATE_FREQUENCY = 30;     //������� ���������� �������

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� OpenAL
///////////////////////////////////////////////////////////////////////////////////////////////////
struct OpenALException: virtual public xtl::exception {};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� OpenAL
///////////////////////////////////////////////////////////////////////////////////////////////////
class OpenALContext
{
  friend class OpenALContextManagerImpl;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    OpenALContext  (const char* device_name, const char* init_string = "");
    ~OpenALContext ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsCurrent () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��� �������� OpenAL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void alEnable (ALenum capability);
    void alDisable (ALenum capability);
    ALboolean alIsEnabled (ALenum capability);
    const ALchar* alGetString (ALenum param);
    void alGetBooleanv (ALenum param, ALboolean* data);
    void alGetIntegerv (ALenum param, ALint* data);
    void alGetFloatv (ALenum param, ALfloat* data);
    void alGetDoublev (ALenum param, ALdouble* data);
    ALboolean alGetBoolean (ALenum param);
    ALint alGetInteger (ALenum param);
    ALfloat alGetFloat (ALenum param);
    ALdouble alGetDouble (ALenum param);
    void alGenSources (ALsizei n, ALuint* sources);
    void alDeleteSources (ALsizei n, const ALuint* sources);
    ALboolean alIsSource (ALuint sid);
    void alGetSourcef (ALuint sid, ALenum param, ALfloat* value);
    void alGetSource3f (ALuint sid, ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3);
    void alGetSourcefv (ALuint sid, ALenum param, ALfloat* values);
    void alGetSourcei (ALuint sid, ALenum param, ALint* value);
    void alGetSource3i (ALuint sid, ALenum param, ALint* value1, ALint* value2, ALint* value3);
    void alGetSourceiv (ALuint sid, ALenum param, ALint* values);
    void alSourcef (ALuint sid, ALenum param, ALfloat value);
    void alSource3f (ALuint sid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
    void alSourcefv (ALuint sid, ALenum param, const ALfloat* values);
    void alSourcei (ALuint sid, ALenum param, ALint value);
    void alSource3i (ALuint sid, ALenum param, ALint value1, ALint value2, ALint value3);
    void alSourceiv (ALuint sid, ALenum param, const ALint* values);
    void alSourcePlayv (ALsizei ns, const ALuint *sids);
    void alSourceStopv (ALsizei ns, const ALuint *sids);
    void alSourceRewindv (ALsizei ns, const ALuint *sids);
    void alSourcePausev (ALsizei ns, const ALuint *sids);
    void alSourcePlay (ALuint sid);
    void alSourceStop (ALuint sid);
    void alSourceRewind (ALuint sid);
    void alSourcePause (ALuint sid);
    void alSourceQueueBuffers (ALuint sid, ALsizei numEntries, const ALuint *bids);
    void alSourceUnqueueBuffers (ALuint sid, ALsizei numEntries, ALuint *bids);
    void alListenerf (ALenum param, ALfloat value);
    void alListener3f (ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
    void alListenerfv (ALenum param, const ALfloat* values);
    void alListeneri (ALenum param, ALint value);
    void alListener3i (ALenum param, ALint value1, ALint value2, ALint value3);
    void alListeneriv (ALenum param, const ALint* values);
    void alGetListenerf (ALenum param, ALfloat* value);
    void alGetListener3f (ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3);
    void alGetListenerfv (ALenum param, ALfloat* values);
    void alGetListeneri (ALenum param, ALint* value);
    void alGetListener3i (ALenum param, ALint *value1, ALint *value2, ALint *value3);
    void alGetListeneriv (ALenum param, ALint* values);
    void alGenBuffers (ALsizei n, ALuint* buffers);
    void alDeleteBuffers (ALsizei n, const ALuint* buffers);
    ALboolean alIsBuffer (ALuint bid);
    void alBufferData (ALuint bid, ALenum format, const ALvoid* data, ALsizei size, ALsizei freq);
    void alBufferf (ALuint bid, ALenum param, ALfloat value);
    void alBuffer3f (ALuint bid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
    void alBufferfv (ALuint bid, ALenum param, const ALfloat* values);
    void alBufferi (ALuint bid, ALenum param, ALint value);
    void alBuffer3i (ALuint bid, ALenum param, ALint value1, ALint value2, ALint value3);
    void alBufferiv (ALuint bid, ALenum param, const ALint* values);
    void alGetBufferf (ALuint bid, ALenum param, ALfloat* value);
    void alGetBuffer3f (ALuint bid, ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3);
    void alGetBufferfv (ALuint bid, ALenum param, ALfloat* values);
    void alGetBufferi (ALuint bid, ALenum param, ALint* value);
    void alGetBuffer3i (ALuint bid, ALenum param, ALint* value1, ALint* value2, ALint* value3);
    void alGetBufferiv (ALuint bid, ALenum param, ALint* values);
    void alDopplerFactor (ALfloat value);
    void alDopplerVelocity (ALfloat value);
    void alSpeedOfSound (ALfloat value);
    void alDistanceModel (ALenum distanceModel);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��� �������� OpenALContext
///////////////////////////////////////////////////////////////////////////////////////////////////
    ALCboolean alcIsExtensionPresent (const ALCchar *extname);
    void       alcGetIntegerv        (ALCenum param, ALCsizei size, ALCint *data);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool MakeCurrent ();

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������ ����� ������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CheckErrors (const char* function_name);
    void ContextCheckErrors (const char* function_name);

  private:
    OpenALContext (const OpenALContext&); //no impl
    OpenALContext& operator = (const OpenALContext&); //no impl

  private:
    ALCdevice*  device;          //���������� OpenAL
    ALCcontext* context;         //�������� OpenAL
    bool        efx_present;     //������� EFX
    common::Log log;             //��������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��� ������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class ISampleDecoder
{
  public:
//////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������������� ����� ����������� samples_count ������� ������� � first sample � data
//////////////////////////////////////////////////////////////////////////////////////////////////
    virtual size_t Read (size_t first_sample, size_t samples_count, void* data) = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////
///������������
//////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ISampleDecoder* Clone () = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
//////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AddRef () = 0;
    virtual void Release () = 0;

  protected:
    virtual ~ISampleDecoder () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������ � �������� ���������� ����� ��������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class FunctionSampleDecoder : public ISampleDecoder, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FunctionSampleDecoder (const IDevice::SampleReadFunction& fn);
    ~FunctionSampleDecoder ();

//////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������������� ����� ����������� samples_count ������� ������� � first sample � data
//////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Read (size_t first_sample, size_t samples_count, void* data);

//////////////////////////////////////////////////////////////////////////////////////////////////
///������������
//////////////////////////////////////////////////////////////////////////////////////////////////
    FunctionSampleDecoder* Clone ();

//////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
//////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef () { addref (this); }
    void Release () { release (this); }

  private:
    FunctionSampleDecoder (const FunctionSampleDecoder&);             //no impl
    FunctionSampleDecoder operator = (const FunctionSampleDecoder&);  //no impl

  private:
    struct Impl;

  private:
    FunctionSampleDecoder (Impl* impl);

  private:
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������ � �������� �� ������ media::Sample
///////////////////////////////////////////////////////////////////////////////////////////////////
class MediaSampleDecoder : public ISampleDecoder, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    MediaSampleDecoder (const char* name);

//////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������������� ����� ����������� samples_count ������� ������� � first sample � data
//////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Read (size_t first_sample, size_t samples_count, void* data);

//////////////////////////////////////////////////////////////////////////////////////////////////
///������������
//////////////////////////////////////////////////////////////////////////////////////////////////
    MediaSampleDecoder* Clone ();

//////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
//////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef () { addref (this); }
    void Release () { release (this); }

  private:
    MediaSampleDecoder (const media::SoundSample& sample);

  private:
    MediaSampleDecoder (const MediaSampleDecoder&);             //no impl
    MediaSampleDecoder operator = (const MediaSampleDecoder&);  //no impl

  private:
    media::SoundSample sample;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������ � ��������, �������� �������������� ����� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class BufferedSampleDecoder : public ISampleDecoder, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    BufferedSampleDecoder (const char* name);
    BufferedSampleDecoder (const SampleDesc& desc, const IDevice::SampleReadFunction& fn);
    ~BufferedSampleDecoder ();

//////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������������� ����� ����������� samples_count ������� ������� � first sample � data
//////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Read (size_t first_sample, size_t samples_count, void* data);

//////////////////////////////////////////////////////////////////////////////////////////////////
///������������
//////////////////////////////////////////////////////////////////////////////////////////////////
    BufferedSampleDecoder* Clone ();

//////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
//////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef () { addref (this); }
    void Release () { release (this); }

  private:
    BufferedSampleDecoder (const BufferedSampleDecoder&);            //no impl
    BufferedSampleDecoder operator = (const BufferedSampleDecoder&); //no impl

  private:
    struct Impl;

  private:
    BufferedSampleDecoder (Impl* impl);

  private:
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ������ � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class OpenALSample : public ISample, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    OpenALSample (const char* name);
    OpenALSample (const SampleDesc& desc, const IDevice::SampleReadFunction& fn);

//////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� ������
//////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetName ();

//////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������� ������
//////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc (SampleDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� ����� ������������� ����� ������ (������ ���������� ���������� � ������� �������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t SamplesToBytes   (size_t sample_count);   //������� ���������� ������� � ������ � ������
    size_t BytesToSamples   (size_t byte_count);     //������� ���������� ���� � ������ � �������
    double SamplesToSeconds (size_t sample_count);   //������� ���������� ������� � ������ � ��������
    size_t SecondsToSamples (double second_count);   //����������������� ������ � ������ � �������

//////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
//////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Channels     ();
    size_t Frequency    ();
    size_t SamplesCount ();
    double Duration     ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISampleDecoder* CreateDecoder ();

//////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
//////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef () { addref (this); }
    void Release () { release (this); }

  private:
    typedef xtl::com_ptr<ISampleDecoder> SampleDecoderPtr;

  private:
    SampleDesc       desc;
    SampleDecoderPtr decoder;
    stl::string      name;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///C������ ��������������� �����, ������������� ����� OpenAL
///////////////////////////////////////////////////////////////////////////////////////////////////
class OpenALDevice : public sound::low_level::IDevice, public common::Lockable, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    OpenALDevice  (const char* driver_name, const char* device_name, const char* init_string = "");
    ~OpenALDevice ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� �� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetCapabilities (Capabilities&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t ChannelsCount ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������ ��� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISample* CreateSample (const char* name);
    ISample* CreateSample (const SampleDesc& desc, const SampleReadFunction& fn);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� �������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void     SetSample (size_t channel, ISample* sample);
    ISample* GetSample (size_t channel);

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
    void  Seek  (size_t channel, float time_in_seconds, SeekMode seek_mode);
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
///������ �� ������� �������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void          SetFirstActiveSource (OpenALSource* source);
    OpenALSource* GetFirstActiveSource () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///OpenAL ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const OpenALContext& Context () const;
          OpenALContext& Context ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const void* GetSampleBuffer     () const;
          void* GetSampleBuffer     ();
    size_t      GetSampleBufferSize () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������������� ������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ALuint AllocateSourceBuffer   ();
    void   DeallocateSourceBuffer (ALuint);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  ();
    void Release ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetParamsNames  ();
    bool        IsIntegerParam  (const char* name);
    bool        IsStringParam   (const char* name);
    void        SetIntegerParam (const char* name, int value);
    int         GetIntegerParam (const char* name);
    void        SetStringParam  (const char* name, const char* value);
    const char* GetStringParam  (const char* name);

  private:
    int  BufferUpdateLoop ();
    void BufferUpdate ();
    void ListenerUpdate ();
    void SourceUpdate ();
    void UpdateListenerNotify ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ClearALData ();

    OpenALDevice (const OpenALDevice&);
    OpenALDevice& operator = (const OpenALDevice&);

  private:
    typedef xtl::uninitialized_storage<char> SampleBuffer;
    typedef stl::vector<OpenALSource*>       ChannelsArray;

  private:
    stl::string                   name;                                       //��� ����������
    OpenALContext                 context;                                    //��������
    common::Action                listener_action;                            //������ ���������� ���������
    common::Action                source_action;                              //������ ���������� ����������
    Capabilities                  info;                                       //���������� � ����������
    Listener                      listener;                                   //���������
    bool                          listener_need_update;                       //��������� ������� ����������
    SampleBuffer                  sample_buffer;                              //����� �������������
    float                         gain;                                       //����������� ��������
    bool                          is_muted;                                   //��� ���������� ������������
    ChannelsArray                 channels;                                   //������ ������������
    volatile size_t               buffer_update_frequency;                    //������� ���������� �������
    size_t                        source_properties_update_frequency;         //������� ���������� ������� ���������
    size_t                        listener_properties_update_frequency;       //������� ���������� ������� ���������
    OpenALSource*                 first_active_source;                        //������ �������� ��������
    ALuint                        al_buffers_pool [DEVICE_BUFFERS_POOL_SIZE]; //��� �������
    size_t                        al_buffers_pool_size;                       //������ ���� �������
    stl::auto_ptr<syslib::Thread> buffers_update_thread;                      //���� ���������� �������
    volatile bool                 buffers_update_thread_stop_request;         //������ ��������� ���� ���������� �������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����� OpenAL
///////////////////////////////////////////////////////////////////////////////////////////////////
class OpenALSource
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    OpenALSource  (OpenALDevice& device);
    ~OpenALSource ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� �������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void          SetSample (OpenALSample* sample);
    OpenALSample* GetSample () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������� ������������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsLooped () const { return is_looped; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void          SetSource (const Source&);
    const Source& GetSource () const { return source; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  Play      (bool looping = false);
    void  Pause     ();
    void  Stop      ();
    void  Seek      (float time_in_seconds, SeekMode seek_mode);
    float Tell      () const;
    bool  IsPlaying () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� (������������� � OpenAL)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BufferUpdate ();
    void PropertiesUpdate ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �� ������� �������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    OpenALSource* PrevActive () const { return prev_active; }
    OpenALSource* NextActive () const { return next_active; }

  private:
    void   FillBuffer (ALuint al_buffer);
    void   FillBuffers ();
    void   UpdateSourceNotify ();
    void   UpdateSampleNotify ();
    size_t TellInMilliseconds () const;
    void   Activate ();
    void   Deactivate ();

  private:
    OpenALSource (const OpenALSource&); //no impl
    OpenALSource& operator = (const OpenALSource&); //no impl

  private:
    typedef xtl::com_ptr<OpenALSample>   OpenALSamplePtr;
    typedef xtl::com_ptr<ISampleDecoder> SampleDecoderPtr;

  private:
    OpenALDevice&    device;                            //���������� ������������
    OpenALSamplePtr  sound_sample;                      //����
    SampleDecoderPtr sound_sample_decoder;              //������� �����
    Source           source;                            //��������� ��������� �����
    bool             source_need_update;                //�������� ����� ������� ����������
    bool             sample_need_update;                //���������� �������� ������� ������������
    bool             is_looped;                         //����������� ������������
    bool             is_playing;                        //������������� �� ����
    bool             is_active;                         //�������� �� �������� ��������
    size_t           play_time_start;                   //����� ������ ������������
    size_t           play_time_offset;                  //�������� ��� ������������
    size_t           last_buffers_fill_time;            //����� ���������� ���������� �������
    size_t           play_sample_position;              //������� ������������
    ALuint           al_source;                         //��� ��������� � OpenAL
    ALuint           al_buffers [SOURCE_BUFFERS_COUNT]; //OpenAL ������
    OpenALSource     *prev_active, *next_active;        //������ �������� ���������� (�������������)
    common::Log      log;                               //��������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� OpenAL
///////////////////////////////////////////////////////////////////////////////////////////////////
class OpenALContextManagerImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    OpenALContextManagerImpl ();
    ~OpenALContextManagerImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetCurrentContext (OpenALContext& context);

  private:
    OpenALContextManagerImpl (const OpenALContextManagerImpl&);             //no impl
    OpenALContextManagerImpl& operator = (const OpenALContextManagerImpl&); //no impl

  private:
    OpenALContext* current_context;
};

typedef common::Singleton<OpenALContextManagerImpl> OpenALContextManager;

const char* get_al_constant_name (ALenum value);

}

}

}

#endif
