#ifndef MEDIALIB_PLAYER_STREAM_HEADER
#define MEDIALIB_PLAYER_STREAM_HEADER

#include <xtl/functional_fwd>

namespace media
{

namespace players
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum StreamEvent
{
  StreamEvent_OnPlay,         //����� ����� �������������
  StreamEvent_OnPause,        //������������ ������ ��������������
  StreamEvent_OnManualStop,   //������������ ������ ����������� �������
  StreamEvent_OnAutoStop,     //������������ ������ ����������� ������������� �� ��������� ��� ������������
  
  StreamEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� �������������� �����-������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IStreamPlayer
{
  public:  
    virtual ~IStreamPlayer () {}

    virtual float Duration    () = 0;                 //������������ ������
    virtual void  Play        () = 0;                 //������ ������������
    virtual void  Pause       () = 0;                 //������������� ������������
    virtual void  Stop        () = 0;                 //���������� ������������
    virtual void  SetPosition (float position) = 0;   //���������� ������� ������������
    virtual float Position    () = 0;                 //�������� ������� ������������    
    virtual void  SetLooping  (bool state) = 0;       //��������� ������ ������������ ������������    
    virtual void  SetVolume   (float volume) = 0;     //���������� ���������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������������� �����-�������
///////////////////////////////////////////////////////////////////////////////////////////////////
class StreamPlayerManager
{
  public:
    typedef xtl::function<void (IStreamPlayer*, StreamEvent)> StreamEventHandler;
    typedef xtl::function<IStreamPlayer* (const char* target_name, const char* source_name, const StreamEventHandler& handler)> PlayerCreator;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �������������� �����-�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterPlayer       (const char* target, const PlayerCreator& creator);
    static void UnregisterPlayer     (const char* target);
    static void UnregisterAllPlayers ();
};

}

}

#endif
