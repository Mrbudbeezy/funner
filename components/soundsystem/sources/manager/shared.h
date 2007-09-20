#include <stl/hash_map>
#include <stl/string>
#include <xtl/signal.h>
#include <common/refcount.h>
#include <syslib/window.h>
#include <math/mathlib.h>
#include <sound/device.h>
#include <sound/manager.h>
#include <media/sound.h>
#include <media/sound_source.h>

using namespace sound;
using namespace sound::low_level;
using namespace stl;
using namespace xtl;
using namespace media;
using namespace math;

/*
   �������� ���������� ���������� �����
*/

typedef signal<void (Emitter&, EmitterEvent)> EmitterSignal;

class sound::EmitterImpl : public common::ReferenceCounter
{
  public:
    EmitterImpl (SoundManagerImpl& in_manager);

  public:
    SoundManagerImpl& manager;                   //��������
    EmitterSignal     signals[EmitterEvent_Num]; //������� ����������
    SoundSource       sound_source;              //����
    SoundSample       sound_sample;              //����� �����
    float             gain;                      //��������
    float             group_gain;                //�������� ������
    bool              is_playing;                //������������
    float             duration;                  //�����������������
    vec3f             position;                  //�������
    vec3f             direction;                 //�����������
    vec3f             velocity;                  //��������
    clock_t           play_time_start;           //����� ������ ������������
    clock_t           play_time_offset;          //�������� ��� ������������
};

/*
   �������� ���������� ������ ������������� ������
*/

class sound::EmitterGroupImpl : public common::ReferenceCounter
{
  public:
    EmitterGroupImpl (const char* group_name);

  public:
    string              name;         //����
    float               gain;         //��������
};

/*
   �������� ���������� ��������� ���������
*/

class sound::SoundManagerImpl
{
  public:
    SoundManagerImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  Play  (Emitter& emitter);
    void  Pause (Emitter& emitter);
    void  Stop  (Emitter& emitter);
    void  Seek  (Emitter& emitter, float position);
    float Tell  (Emitter& emitter) const;


  public:
    ISoundDevice*                   sound_device;   //���������� ���������������
    syslib::Window*                 window;         //����
    float                           gain;           //��������
    bool                            mute;           //������������
    sound::Listener                 listener;       //���������
    hash_multimap <string, Emitter> emitters;       //��������� �����
};
