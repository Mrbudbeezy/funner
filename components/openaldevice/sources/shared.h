#ifndef SOUND_SYSTEM_OPENAL_DEVICE_SHARED_HEADER
#define SOUND_SYSTEM_OPENAL_DEVICE_SHARED_HEADER

#include <stdarg.h>
#include <time.h>
#include <al.h>
#include <alc.h>
#include <xtl/function.h>
#include <xtl/bind.h>
#include <xtl/uninitialized_storage.h>
#include <common/strlib.h>
#include <common/exception.h>
#include <media/sound.h>
#include <syslib/timer.h>
#include <sound/openal_device.h>

namespace sound
{

namespace low_level
{

//implementation forwards
class OpenALDevice;
class OpenALSource;
class OpenALContext;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
const size_t SOURCE_BUFFERS_COUNT         = 4;    //���������� ������� ������������ �� ��������
const size_t MAX_DEVICE_CHANNELS_COUNT    = 1024; //������������ ���������� ������� ������������
const size_t DEFAULT_SAMPLE_BUFFER_SIZE   = 4096; //������ ������ ������������� �� ���������
const float  SOURCE_BUFFERS_UPDATE_PERIOD = 0.1f; //������ ���������� �������

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� OpenAL
///////////////////////////////////////////////////////////////////////////////////////////////////
struct OpenALExceptionTag;

//����������, ����������� ��� ������������� �������� ���������
struct OpenALGenSourceException: public std::exception
{
  const char* what () const throw () { return "OpenALGenSourceException"; }
};

//����������, ����������� ��� ������������� �������� ������
struct OpenALGenBufferException: public std::exception
{
  const char* what () const throw () { return "OpenALGenBufferException"; }
};

typedef common::DerivedException<common::Exception, OpenALExceptionTag> OpenALException;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� OpenAL
///////////////////////////////////////////////////////////////////////////////////////////////////
class OpenALContext
{
  public:
    typedef ISoundDevice::LogHandler LogHandler;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    OpenALContext  (const char* device_name, const LogHandler& log_handler);
    ~OpenALContext ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsCurrent   () const;
    bool MakeCurrent ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��� �������� OpenAL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void alEnable( ALenum capability );
    void alDisable( ALenum capability ); 
    ALboolean alIsEnabled( ALenum capability );
    const ALchar* alGetString( ALenum param );
    void alGetBooleanv( ALenum param, ALboolean* data );
    void alGetIntegerv( ALenum param, ALint* data );
    void alGetFloatv( ALenum param, ALfloat* data );
    void alGetDoublev( ALenum param, ALdouble* data );
    ALboolean alGetBoolean( ALenum param );
    ALint alGetInteger( ALenum param );
    ALfloat alGetFloat( ALenum param );
    ALdouble alGetDouble( ALenum param );
    void alGenSources( ALsizei n, ALuint* sources );     
    void alDeleteSources( ALsizei n, const ALuint* sources );
    ALboolean alIsSource( ALuint sid );      
    void alGetSourcef( ALuint sid, ALenum param, ALfloat* value );
    void alGetSource3f( ALuint sid, ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3);
    void alGetSourcefv( ALuint sid, ALenum param, ALfloat* values );
    void alGetSourcei( ALuint sid,  ALenum param, ALint* value );
    void alGetSource3i( ALuint sid, ALenum param, ALint* value1, ALint* value2, ALint* value3);
    void alGetSourceiv( ALuint sid,  ALenum param, ALint* values );
    void alSourcef( ALuint sid, ALenum param, ALfloat value ); 
    void alSource3f( ALuint sid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3 );
    void alSourcefv( ALuint sid, ALenum param, const ALfloat* values ); 
    void alSourcei( ALuint sid, ALenum param, ALint value ); 
    void alSource3i( ALuint sid, ALenum param, ALint value1, ALint value2, ALint value3 );
    void alSourceiv( ALuint sid, ALenum param, const ALint* values );
    void alSourcePlayv( ALsizei ns, const ALuint *sids );
    void alSourceStopv( ALsizei ns, const ALuint *sids );
    void alSourceRewindv( ALsizei ns, const ALuint *sids );
    void alSourcePausev( ALsizei ns, const ALuint *sids );
    void alSourcePlay( ALuint sid );
    void alSourceStop( ALuint sid );
    void alSourceRewind( ALuint sid );
    void alSourcePause( ALuint sid );
    void alSourceQueueBuffers( ALuint sid, ALsizei numEntries, const ALuint *bids );
    void alSourceUnqueueBuffers( ALuint sid, ALsizei numEntries, ALuint *bids );     
    void alListenerf( ALenum param, ALfloat value );
    void alListener3f( ALenum param, ALfloat value1, ALfloat value2, ALfloat value3 );
    void alListenerfv( ALenum param, const ALfloat* values ); 
    void alListeneri( ALenum param, ALint value );
    void alListener3i( ALenum param, ALint value1, ALint value2, ALint value3 );
    void alListeneriv( ALenum param, const ALint* values );
    void alGetListenerf( ALenum param, ALfloat* value );
    void alGetListener3f( ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3 );
    void alGetListenerfv( ALenum param, ALfloat* values );
    void alGetListeneri( ALenum param, ALint* value );
    void alGetListener3i( ALenum param, ALint *value1, ALint *value2, ALint *value3 );
    void alGetListeneriv( ALenum param, ALint* values );
    void alGenBuffers( ALsizei n, ALuint* buffers );
    void alDeleteBuffers( ALsizei n, const ALuint* buffers );
    ALboolean alIsBuffer( ALuint bid );
    void alBufferData( ALuint bid, ALenum format, const ALvoid* data, ALsizei size, ALsizei freq );
    void alBufferf( ALuint bid, ALenum param, ALfloat value );
    void alBuffer3f( ALuint bid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3 );
    void alBufferfv( ALuint bid, ALenum param, const ALfloat* values );
    void alBufferi( ALuint bid, ALenum param, ALint value );
    void alBuffer3i( ALuint bid, ALenum param, ALint value1, ALint value2, ALint value3 );
    void alBufferiv( ALuint bid, ALenum param, const ALint* values );
    void alGetBufferf( ALuint bid, ALenum param, ALfloat* value );
    void alGetBuffer3f( ALuint bid, ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3);
    void alGetBufferfv( ALuint bid, ALenum param, ALfloat* values );
    void alGetBufferi( ALuint bid, ALenum param, ALint* value );
    void alGetBuffer3i( ALuint bid, ALenum param, ALint* value1, ALint* value2, ALint* value3);
    void alGetBufferiv( ALuint bid, ALenum param, ALint* values );
    void alDopplerFactor( ALfloat value );
    void alDopplerVelocity( ALfloat value );
    void alSpeedOfSound( ALfloat value );
    void alDistanceModel( ALenum distanceModel );

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LogError (const char* message, ...);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� � OpenAL
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Fn, class Tuple>
    void Dispatch (const char* function_name, Fn fn, const Tuple& args);

    template <class Ret, class Fn, class Tuple>
    Ret Dispatch (const char* function_name, Fn fn, const Tuple& args);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������ ����� ������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Tuple>
    void CheckErrors (const char* function_name, const Tuple& args);

  private:
    OpenALContext (const OpenALContext&); //no impl
    OpenALContext& operator = (const OpenALContext&); //no impl

  private:
    ALCdevice*  device;       //���������� OpenAL
    ALCcontext* context;      //�������� OpenAL
    LogHandler  log_handler;  //������� ����������������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///C������ ��������������� �����, ������������� ����� OpenAL
///////////////////////////////////////////////////////////////////////////////////////////////////
class OpenALDevice : public sound::low_level::ISoundDevice
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    OpenALDevice  (const char* driver_name, const char* device_name);
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
    size_t ChannelsCount () { return channels_count; }
    
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
///������ �� ������� �������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void          SetFirstActiveSource (OpenALSource* source) { first_active_source = source; }
    OpenALSource* GetFirstActiveSource () const { return first_active_source; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///OpenAL ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const OpenALContext& Context () const { return context; }
          OpenALContext& Context ()       { return context; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void*  GetSampleBuffer     () const { return sample_buffer.data (); }
    size_t GetSampleBufferSize () const { return sample_buffer.size (); }

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
///��������� ������� ����������� ����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void              SetDebugLog (const LogHandler&);
    const LogHandler& GetDebugLog ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void DebugPrintf  (const char* format, ...);
    void DebugVPrintf (const char* format, va_list);

  private:
    void Update ();   
    void UpdateListenerNotify ();

    OpenALDevice (const OpenALDevice&);
    OpenALDevice& operator = (const OpenALDevice&);

  private:
    typedef xtl::uninitialized_storage<char> SampleBuffer;

  private:
    stl::string    name;                 //��� ����������
    OpenALContext  context;              //��������    
    syslib::Timer  timer;                //������ ���������� ������
    LogHandler     log_handler;          //������� ����
    Capabilities   info;                 //���������� � ����������
    Listener       listener;             //���������
    bool           listener_need_update; //��������� ������� ����������
    SampleBuffer   sample_buffer;        //����� �������������        
    size_t         ref_count;            //���������� ������ �� ����������    
    float          gain;                 //����������� ��������
    bool           is_muted;             //��� ���������� ������������
    size_t         channels_count;       //���������� �������
    OpenALSource*  channels [MAX_DEVICE_CHANNELS_COUNT]; //������ ������������
    OpenALSource*  first_active_source;  //������ �������� ��������
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
    void        SetSample (const char* sample_name);
    const char* GetSample () const;

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
    void  Seek      (float time_in_seconds);
    float Tell      () const;
    bool  IsPlaying () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� (������������� � OpenAL)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Update ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �� ������� �������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    OpenALSource* PrevActive () const { return prev_active; }
    OpenALSource* NextActive () const { return next_active; }

  private:
    void   FillBuffer (size_t al_buffer);
    void   FillBuffers ();
    void   UpdateSourceNotify ();
    void   UpdateSampleNotify ();
    size_t TellInTicks () const;
    void   Activate ();
    void   Deactivate ();
    
  private:
    OpenALSource (const OpenALSource&); //no impl
    OpenALSource& operator = (const OpenALSource&); //no impl
    
  private:
    typedef medialib::SoundSample SoundSample;

  private:
    OpenALDevice&  device;                            //���������� ������������
    SoundSample    sound_sample;                      //����
    Source         source;                            //��������� ��������� �����
    bool           source_need_update;                //�������� ����� ������� ����������
    bool           sample_need_update;                //���������� �������� ������� ������������
    bool           is_looped;                         //����������� ������������
    bool           is_playing;                        //������������� �� ����
    bool           is_active;                         //�������� �� �������� ��������
    clock_t        play_time_start;                   //����� ������ ������������
    clock_t        play_time_offset;                  //�������� ��� ������������
    clock_t        last_buffers_fill_time;            //����� ���������� ���������� �������
    size_t         play_sample_position;              //������� ������������    
    ALuint         al_source;                         //��� ��������� � OpenAL
    ALuint         al_buffers [SOURCE_BUFFERS_COUNT]; //OpenAL ������
    OpenALSource   *prev_active, *next_active;        //������ �������� ���������� (�������������)
};

}

}

#endif
