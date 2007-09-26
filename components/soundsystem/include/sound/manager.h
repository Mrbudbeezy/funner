#ifndef SOUND_MANAGER_HEADER
#define SOUND_MANAGER_HEADER

#include <xtl/functional_fwd>
#include <stl/auto_ptr.h>
#include <mathlib.h>
#include "shared.h"

namespace syslib
{

//forward declarations
class Window;

}

namespace sound
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ��� ������������ ����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum WindowMinimizeAction
{
  WindowMinimizeAction_Ignore, //�� �����������
  WindowMinimizeAction_Mute,   //��������� ����
  WindowMinimizeAction_Pause,  //���������������� ������������ ���� ������
  
  WindowMinimizeAction_Default = WindowMinimizeAction_Ignore
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum EmitterEvent
{
  EmitterEvent_OnDestroy,          //���������� ��������������� ����� ��������� ����������
  EmitterEvent_OnActivate,         //���������� �����������
  EmitterEvent_OnDeactivate,       //���������� �������������
  EmitterEvent_OnUpdateVolume,     //�������� ��������� �����
  EmitterEvent_OnUpdateProperties, //�������� ��������� ���������� �����
  
  EmitterEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Emitter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Emitter  (const char* source_name);
    ~Emitter ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Source () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetVolume (float volume);
    float Volume    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������ ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetPosition  (const math::vec3f&);
    void SetDirection (const math::vec3f&);
    void SetVelocity  (const math::vec3f&);

    const math::vec3f& Position  () const;
    const math::vec3f& Direction () const;
    const math::vec3f& Velocity  () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����������
///  - ������ ����� Activate ���������� ������� EmitterEvent_OnActivate
///  - ��������� ����� Deactivate ���������� ������� EmitterEvent_OnDeactivate
///  - ���������� �������� �������� �� ��� ���, ���� ������������� ���� �� � ����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Activate   ();
    void Deactivate ();
    bool IsActive   () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (Emitter&, EmitterEvent)> EventHandler;

    xtl::connection RegisterEventHandler (EmitterEvent, const EventHandler&);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SoundManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SoundManager  (syslib::Window& target_window, const char* target_configuration, const char* init_string="");
    ~SoundManager ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������������ (������������� �� low_level::SoundDeviceSystem)
///////////////////////////////////////////////////////////////////////////////////////////////////
    static const char* FindConfiguration (const char* driver_mask, const char* device_mask);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ��� ������������ ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                        SetWindowMinimizeAction (WindowMinimizeAction action);
    sound::WindowMinimizeAction WindowMinimizeAction    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetVolume (float volume);
    float Volume    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetMute (bool state);
    bool IsMuted () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  PlaySound           (Emitter& emitter, float normalized_offset=0.0f); //normalized_offset in [0;1]
    void  StopSound           (Emitter& emitter);
    float GetNormalizedOffset (Emitter&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� �� ���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (Emitter&)>       EmitterHandler;
    typedef xtl::function<void (const Emitter&)> ConstEmitterHandler;

    void ForEachEmitter (const EmitterHandler&);
    void ForEachEmitter (const ConstEmitterHandler&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� �� ���� ���������� c �������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ForEachEmitter (const char* type, const EmitterHandler&);
    void ForEachEmitter (const char* type, const ConstEmitterHandler&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                   SetListener (const Listener&);
    const sound::Listener& Listener    () const;

  private:
    SoundManager (const SoundManager&); //no impl
    SoundManager& operator = (const SoundManager&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

#endif
