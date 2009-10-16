#ifndef MEDIALIB_PLAYER_MEDIA_PLAYER_HEADER
#define MEDIALIB_PLAYER_MEDIA_PLAYER_HEADER

#include <xtl/functional_fwd>

namespace media
{

namespace player
{

//forward declarations
class PlayList;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������� �����-������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum MediaPlayerState
{  
  MediaPlayerState_Stopped, //����������
  MediaPlayerState_Playing, //�������������
  MediaPlayerState_Paused,  //������������� (���������� ��� ������ �������)
  
  MediaPlayerState_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum MediaPlayerRepeatMode
{
  MediaPlayerRepeatMode_Off,     //���������� ������������ ������� ���������
  MediaPlayerRepeatMode_Current, //����������� ������� ��������� �����
  MediaPlayerRepeatMode_Last,    //����������� ��������� ��������� ����� � ������ ������������
  MediaPlayerRepeatMode_All,     //����������� ���� ������ ������������

  MediaPlayerRepeatMode_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������������� �����-�������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum MediaPlayerEvent
{
  MediaPlayerEvent_OnChangeTrack,    //���������� ����
  MediaPlayerEvent_OnChangePlayback, //�������� ��������� ������������ (play / pause / stop / position)
  MediaPlayerEvent_OnChangeVolume,   //�������� ��������� ���������
  
  MediaPlayerEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� �����-�������
///////////////////////////////////////////////////////////////////////////////////////////////////
class MediaPlayer
{
  public:    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    MediaPlayer  ();
    MediaPlayer  (const char* target_name);
    MediaPlayer  (const char* target_name, const char* stream_name);
    MediaPlayer  (const char* target_name, const PlayList& list);
    MediaPlayer  (const MediaPlayer&);
    ~MediaPlayer ();

    MediaPlayer& operator = (const MediaPlayer&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetTarget (const char* target_name);    
    const char* Target    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� / �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Open  (const char* stream_name);
    void Open  (const PlayList&);
    void Close ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t TracksCount () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                  SetRepeatMode (MediaPlayerRepeatMode mode);
    MediaPlayerRepeatMode RepeatMode    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������������� ����� / ��� ������ / ��� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetTrack (size_t track);       //��������� �������� �����
    size_t      Track    () const;             //������� ����
    const char* Source   (size_t track) const; //��� �����    
    const char* Source   () const;             //��� ������������� �����
    float       Duration (size_t tack);        //������������ �����
    float       Duration () const;             //������������ �������������� �����
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void NextTrack ();
    void PrevTrack ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    MediaPlayerState State () const; //��������� ������������    
    void             Play  ();       //������ / ���������� ������������
    void             Pause ();       //������������� ������������
    void             Stop  ();       //���������� ������������

    bool IsPlaying () const { return State () == MediaPlayerState_Playing; } //������������� �� �����
    bool IsPaused  () const { return State () == MediaPlayerState_Paused; }  //������������� �� �����
    bool IsStopped () const { return State () == MediaPlayerState_Stopped; } //���������� �� �����

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetPosition (float position);
    float Position    () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetVolume (float volume); //in range [0..1]
    float Volume    () const;

    void SetMute (bool state); //������� ��� �������� �� ����
    bool IsMuted () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (MediaPlayer&)> EventHandler;

    xtl::connection RegisterEventHandler (MediaPlayerEvent event, const EventHandler& handler);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (MediaPlayer&);

  private:
    struct Impl;
    Impl* impl;    
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (MediaPlayer&, MediaPlayer&);

}

}

#endif
