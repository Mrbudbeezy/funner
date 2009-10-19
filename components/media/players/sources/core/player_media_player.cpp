#include "shared.h"

using namespace media::players;

namespace
{

/*
    ���������
*/

const char* COMPONENT_MASK = "media.players.stream.*"; //����� ����������� ������������ �����-�������

}

/*
    �������� ���������� ������������� �����-�������
*/

typedef xtl::shared_ptr<IStreamPlayer>                     StreamPtr;
typedef stl::vector<StreamPtr>                             StreamArray;
typedef xtl::signal<void (MediaPlayer&, MediaPlayerEvent)> MediaPlayerSignal;

struct MediaPlayer::Impl
{
  MediaPlayer&                            owner;                          //��������
  stl::string                             target_name;                    //��� ���� ������������
  PlayList                                list;                           //������ ���������������
  StreamArray                             streams;                        //������������� �������
  StreamPlayerManager::StreamEventHandler callback_handler;               //���������� ��������� ��������� ������������� �������
  size_t                                  current_track;                  //������� ����
  MediaPlayerState                        current_track_state;            //��������� �������� �����
  bool                                    is_muted;                       //�������� �� ����
  float                                   volume;                         //������� ���������
  MediaPlayerRepeatMode                   repeat_mode;                    //����� ������� ������������ ������
  MediaPlayerSignal                       signals [MediaPlayerEvent_Num]; //����������� ������� �������������
  
///�����������
  Impl (MediaPlayer& in_owner)
    : owner (in_owner)
    , callback_handler (xtl::bind (&Impl::OnStreamEvent, this, _1, _2))
    , current_track (0)
    , current_track_state (MediaPlayerState_Stopped)
    , is_muted (false)
    , volume (1.0f)
  {
  }  
  
///�������� �������������� �������
  void UpdatePlayList (const PlayList& in_play_list)
  {
    try
    {
      static common::ComponentLoader loader (COMPONENT_MASK);
      
      StreamArray new_streams;
      PlayList    new_play_list = in_play_list.Id () == list.Id () ? list : in_play_list.Clone ();      

      new_streams.reserve (new_play_list.Size ());

      for (size_t i=0, count=new_play_list.Size (); i<count; i++)
      {
        StreamPtr stream (create_stream_player (target_name.c_str (), new_play_list.Item (i), callback_handler));
        
        if (!stream)
          throw xtl::format_operation_exception ("", "Internal error: null player returned for target '%s' stream '%s'",
            target_name.c_str (), new_play_list.Item (i));

        new_streams.push_back (stream);
      }
      
      streams.swap (new_streams);
      
      list = new_play_list;
      
      Notify (MediaPlayerEvent_OnChangePlayList);
    }
    catch (xtl::exception& e)
    {
      e.touch ("media::players::MediaPlayer::Impl::UpdatePlayList(const PlayList&)");
      throw;
    }    
  }
  
  void UpdatePlayList (const char* stream_name)
  {
    try
    {
      PlayList new_play_list;
      
      new_play_list.AddSource (stream_name);
      
      UpdatePlayList (new_play_list);
    }
    catch (xtl::exception& e)
    {
      e.touch ("media::players::MediaPlayer::Impl::UpdatePlayList(const char*)");
      throw;
    }
  }
  
///��������� �� ������� �����
  bool IsCurrentStreamValid ()
  {
    return current_track < streams.size ();
  }
  
///������� �����-�����
  StreamPtr CurrentStream ()
  {
    return IsCurrentStreamValid () ? streams [current_track] : StreamPtr ();
  }
  
///���������� ������� �����-������
  void OnStreamEvent (IStreamPlayer* stream_player, StreamEvent event)
  {
    try
    {
        //�������������� ������ ������� �������� �����
    
      if (current_track >= streams.size ())
        return;
        
      if (stream_player != CurrentStream ().get ())
        return;
        
      switch (event)
      {
        case StreamEvent_OnPlay:
          current_track_state = MediaPlayerState_Playing;
          break;
        case StreamEvent_OnPause:
          current_track_state = MediaPlayerState_Paused;
          break;
        case StreamEvent_OnManualStop:
          current_track_state = MediaPlayerState_Stopped;
          break;
        case StreamEvent_OnAutoStop:
        {
          current_track_state = MediaPlayerState_Stopped;
          
          size_t track = current_track + 1;
          
          if (track >= streams.size ())
          {
              //���� ������ ��������
              
            switch (repeat_mode)
            {
              case MediaPlayerRepeatMode_Off:
                Notify (MediaPlayerEvent_OnChangeTrack);
                return;
              case MediaPlayerRepeatMode_Last:
                break;
              case MediaPlayerRepeatMode_All:
                current_track = 0;
                Notify (MediaPlayerEvent_OnChangeTrack);
                break;
              default:
                break;
            }
          }
          else
          {
            current_track = track;
            
            Notify (MediaPlayerEvent_OnChangeTrack);
          }
          
          owner.Play ();
          
          break;
        }
        default:
          break;
      }
    }
    catch (...)
    {
      //���������� ���� ����������
    }    
  }
  
///���������� � ������������� �������
  void Notify (MediaPlayerEvent event)
  {
    try
    {
      if (event < 0 || event >= MediaPlayerEvent_Num)
        return;

      signals [event](owner, event);
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }
};

/*
    ������������ / ���������� / ������������
*/

MediaPlayer::MediaPlayer ()  
  : impl (new Impl (*this))
{  
}

MediaPlayer::MediaPlayer (const char* target_name)
  : impl (new Impl (*this))
{
  try
  {       
    if (!target_name)
      throw xtl::make_null_argument_exception ("", "target_name");     

    impl->target_name = target_name;
  }
  catch (xtl::exception& e)
  {          
    e.touch ("media::players::MediaPlayer::MediaPlayer(const char*)");
    throw;
  }
}

MediaPlayer::MediaPlayer (const char* target_name, const char* stream_name)  
  : impl (new Impl (*this))
{
  try
  {
    if (!target_name)
      throw xtl::make_null_argument_exception ("", "target_name");
      
    if (!stream_name)
      throw xtl::make_null_argument_exception ("", "stream_name");

    impl->target_name = target_name;
    
    impl->UpdatePlayList (stream_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::MediaPlayer(const char*, const char*)");
    throw;
  }
}

MediaPlayer::MediaPlayer (const char* target_name, const PlayList& list) 
  : impl (new Impl (*this))
{
  try
  {       
    if (!target_name)
      throw xtl::make_null_argument_exception ("", "target_name");

    impl->target_name = target_name;

    impl->UpdatePlayList (list);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::MediaPlayer(const char*, const PlayList&)");
    throw;
  }
}

MediaPlayer::~MediaPlayer ()
{
  Close ();
}


/*
    ������� ���� ������������
*/

void MediaPlayer::SetTarget (const char* target_name)
{
  try
  {
    if (!target_name)
      throw xtl::make_null_argument_exception ("", "target_name");

      //�������� ���� ������������� �������

    Close ();
    
      //���������� ����

    impl->target_name.clear ();

    impl->target_name = target_name;
    
      //���������� � ����� ����
      
    impl->Notify (MediaPlayerEvent_OnChangeTarget);

      //���������� �������
      
    impl->UpdatePlayList (impl->list);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::SetTarget");
    throw;
  }
}

//��� ���� ������������
const char* MediaPlayer::Target () const
{
  return impl->target_name.c_str ();
}

/*
    �������� / �������� ������
*/

void MediaPlayer::Open (const char* stream_name)
{
  try
  {
    if (!stream_name)
      throw xtl::make_null_argument_exception ("", "stream_name");
    
    Close ();
    
    impl->UpdatePlayList (stream_name);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::Open(const char*)");
    throw;
  }
}

void MediaPlayer::Open (const PlayList& list)
{
  try
  {
    Close ();
    
    impl->UpdatePlayList (list);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::Open(const PlayList&)");
    throw;
  }
}

void MediaPlayer::Close ()
{
  try
  {
    switch (impl->current_track_state)
    {
      case MediaPlayerState_Playing:
      case MediaPlayerState_Paused:
        try
        {
          Stop ();
        }
        catch (...)
        {
        }
        break;
      case MediaPlayerState_Stopped:
      default:
        break;
    }
    
    impl->current_track_state = MediaPlayerState_Stopped;
    impl->current_track       = 0;
    
    impl->streams.clear ();
    impl->list.Clear ();
    
    impl->Notify (MediaPlayerEvent_OnChangePlayList);
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

/*
    ���������� ������
*/

size_t MediaPlayer::TracksCount () const
{
  return impl->list.Size ();
}

/*
    ����� ���������� ����������
*/

void MediaPlayer::SetRepeatMode (MediaPlayerRepeatMode mode)
{
  static const char* METHOD_NAME = "media::players::MediaPlayer::SetRepeatMode";
  
  if (mode < 0 || mode >= MediaPlayerRepeatMode_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "mode", mode);

  impl->repeat_mode = mode;

  impl->Notify (MediaPlayerEvent_OnChangeRepeatMode);
}

MediaPlayerRepeatMode MediaPlayer::RepeatMode () const
{
  return impl->repeat_mode;
}

/*
    ����� �������������� ����� / ��� ������ / ��� ������������
*/

//��������� �������� �����
void MediaPlayer::SetTrack (size_t track)
{
  try
  {
    if (track >= impl->streams.size ())
      throw xtl::make_range_exception ("", "track", track, impl->streams.size ());
      
    MediaPlayerState state = State ();
      
    Stop ();
    
    impl->current_track = track;
    
    switch (state)
    {
      case MediaPlayerState_Playing:
      case MediaPlayerState_Paused:
        Play ();
        break;
      case MediaPlayerState_Stopped:
      default:
        break;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::SetTrack");
    throw;
  }
}

//������� ����
size_t MediaPlayer::Track () const
{
  return impl->current_track;
}

//��� �����
const char* MediaPlayer::Source (size_t track) const
{
  try
  {
    return impl->list.Item (track);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::Source(size_t)");
    throw;
  }
}

//��� ������������� �����
const char* MediaPlayer::Source () const
{
  return Source (Track ());
}

//������������ �����
float MediaPlayer::Duration (size_t track) const
{  
  try
  {
    if (track >= impl->streams.size ())
      throw xtl::make_range_exception ("", "track", track, impl->streams.size ());

    return impl->streams [track]->Duration ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::Duration");
    throw;
  }
}

//������������ �������������� �����
float MediaPlayer::Duration () const
{
  return Duration (Track ());
}

/*
    ������������ ������
*/

void MediaPlayer::NextTrack ()
{
  try
  {
    size_t new_track = impl->current_track + 1;
  
    if (new_track == impl->list.Size ())
      new_track = 0;
      
    SetTrack (new_track);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::MediaPlayer::NextTrack");
    throw;
  }
}

void MediaPlayer::PrevTrack ()
{
  try
  {
    size_t new_track = impl->current_track - 1;
  
    if (new_track >= impl->list.Size ())
      new_track = impl->list.Size () ? impl->list.Size () - 1 : 0;

    SetTrack (new_track);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::MediaPlayer::PrevTrack");
    throw;
  }
}

/*
    ���������� �������������
*/

//��������� ������������
MediaPlayerState MediaPlayer::State () const
{
  return impl->current_track_state;
}

//������ / ���������� ������������
void MediaPlayer::Play ()
{
  try
  {
    if (!impl->IsCurrentStreamValid ())
      return;
    
    switch (impl->current_track_state)
    {
      case MediaPlayerState_Playing:
        SetPosition (0.0f);
        break;
      case MediaPlayerState_Paused:
        impl->CurrentStream ()->Play ();
        impl->Notify (MediaPlayerEvent_OnChangePlayback);
        break;
      case MediaPlayerState_Stopped:
      {
        StreamPtr stream = impl->CurrentStream ();

        stream->SetVolume (impl->volume);
        
        stream->Play ();

        impl->Notify (MediaPlayerEvent_OnChangePlayback);

        break;
      }
      default:
        break;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::Play");
    throw;
  }
}

//������������� ������������
void MediaPlayer::Pause ()
{
  try
  {
    if (!impl->IsCurrentStreamValid ())
      return;
    
    switch (impl->current_track_state)
    {
      case MediaPlayerState_Playing:
        impl->CurrentStream ()->Pause ();
        impl->Notify (MediaPlayerEvent_OnChangePlayback);       
        break;
      case MediaPlayerState_Paused:
        Play ();
        break;
      case MediaPlayerState_Stopped:
        break;
      default:
        break;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::Pause");
    throw;
  }
}

//���������� ������������
void MediaPlayer::Stop ()
{
  try
  {
    if (!impl->IsCurrentStreamValid ())
      return;
    
    switch (impl->current_track_state)
    {
      case MediaPlayerState_Playing:
      case MediaPlayerState_Paused:
        impl->CurrentStream ()->Stop ();
        impl->Notify (MediaPlayerEvent_OnChangePlayback);
        break;
      case MediaPlayerState_Stopped:
        break;
      default:
        break;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::Stop");
    throw;
  }
}

/*
    ������� ������������
*/

void MediaPlayer::SetPosition (float position)
{
  try
  {
    if (!impl->IsCurrentStreamValid ())
      return;
      
    StreamPtr stream   = impl->CurrentStream ();
    float     duration = stream->Duration ();
    
    if (position > duration)
      throw xtl::make_range_exception ("", "position", position, 0.f, duration);

    stream->SetPosition (position);

    impl->Notify (MediaPlayerEvent_OnChangePlayback);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::SetPosition");
    throw;
  }
}

float MediaPlayer::Position () const
{
  try
  {
    if (!impl->IsCurrentStreamValid ())
      return 0.0f;
      
    return impl->CurrentStream ()->Position ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::Position");
    throw;
  }
}

/*
    ���������� ����������
*/

//in range [0..1]
void MediaPlayer::SetVolume (float volume)
{
  try
  {
    if (volume == impl->volume)
      return;
      
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
      
    if (impl->IsCurrentStreamValid ())
      impl->CurrentStream ()->SetVolume (volume);
      
    impl->volume   = volume;
    impl->is_muted = false;
    
    impl->Notify (MediaPlayerEvent_OnChangeVolume);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::SetVolume");
    throw;
  }
}

float MediaPlayer::Volume () const
{
  return impl->volume;
}

//������� ��� �������� �� ����
void MediaPlayer::SetMute (bool state)
{
  try
  {
    if (impl->is_muted == state)
      return;
      
    if (impl->IsCurrentStreamValid ())
    {
      if (state) impl->CurrentStream ()->SetVolume (impl->volume);
      else       impl->CurrentStream ()->SetVolume (0.0f);
    }

    impl->is_muted = state;
    
    impl->Notify (MediaPlayerEvent_OnChangeVolume);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::SetMute");
    throw;
  }
}

bool MediaPlayer::IsMuted () const
{
  return impl->is_muted;
}

/*
    ���������� � �������� ������
*/

xtl::connection MediaPlayer::RegisterEventHandler (MediaPlayerEvent event, const EventHandler& handler) const
{
  static const char* METHOD_NAME = "media::players::MediaPlayer::RegisterEventHandler";
  
  if (event < 0 || event >= MediaPlayerEvent_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "event", event);
    
  return impl->signals [event].connect (handler);
}
