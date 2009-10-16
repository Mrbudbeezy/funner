#include "shared.h"

using namespace media::player;

namespace
{

/*
    ���������
*/

const char* COMPONENT_MASK = "media.player.streams.*"; //����� ����������� ������������ �����-�������

}

/*
    �������� ���������� ������������� �����-�������
*/

typedef xtl::shared_ptr<IStreamPlayer> PlayerPtr;
typedef stl::vector<PlayerPtr>         PlayerArray;

struct MediaPlayer::Impl: private xtl::reference_counter
{
  stl::string                        target_name;         //��� ���� ������������  
  PlayList                           list;                //������ ���������������
  PlayerArray                        players;             //������������� �������    
  StreamManager::StateChangeCallback callback_handler;    //���������� ��������� ��������� ������������� �������  
  size_t                             current_track;       //������� ����
  MediaPlayerState                   current_track_state; //��������� �������� �����
  bool                               is_muted;            //�������� �� ����
  float                              volume;              //������� ���������
  MediaPlayerRepeatMode              repeat_mode;         //����� ������� ������������ ������  
  
///�����������
  Impl ()
    : callback_handler (xtl::bind (&Impl::OnStreamEvent, this, _1, _2))
    , current_track (0)
    , current_track_state (MediaPlayerState_Stopped)
    , is_muted (false)
    , volume (1.0f)
  {
  }  
  
///���������������� ���������
  void CopySettingsFrom (Impl& impl)
  {
    is_muted    = impl.is_muted;
    repeat_mode = impl.repeat_mode;
    volume      = impl.volume;
    
    //???????????????
  }
  
///���������� �������
  void UpdatePlayers ()
  {
    try
    {
      PlayerArray new_players;

      new_players.reserve (list.Size ());

      for (size_t i=0, count=list.Size (); i<count; i++)
      {
        PlayerPtr player (create_player (target_name.c_str (), list.Item (i), callback_handler);
        
        if (!player)
          throw xtl::format_operation_exception ("", "Internal error: null player returned for target '%s' stream '%s'", target_name.c_str (), list.Item (i));

        new_players.push_back (player);
      }
      
      players.swap (new_players);
    }
    catch (xtl::exception& e)
    {
      e.touch ("media::player::MediaPlayer::Impl::UpdatePlayers");
      throw;
    }    
  }

///���������� ������� �����-������
  void OnStreamEvent (IStreamPlayer* player, StreamEvent event)
  {
    try
    {
        //�������������� ������ ������� �������� �����
    
      if (current_track >= players.size ())
        return;
        
      if (player != players [current_player])
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
            //next track  ?????????????
          break;
        }
      }
        
      switch (current_track_state)
      {
      }
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
  : impl (new Impl)
{  
}

MediaPlayer::MediaPlayer (const char* target_name)
  : impl (new Impl)
{
  try
  {
    try
    {       
      if (!target_name)
        throw xtl::make_null_argument_exception ("", "target_name");     

      impl->target_name = target_name;
    }
    catch (xtl::exception& e)
    {          
      e.touch ("media::player::MediaPlayer::MediaPlayer(const char*)");
      throw;
    }
  }
  catch (...)
  {
    delete impl;
    throw;
  }
}

MediaPlayer::MediaPlayer (const char* target_name, const char* stream_name)  
  : impl (new Impl)
{
  try
  {
    try
    {
      if (!target_name)
        throw xtl::make_null_argument_exception ("", "target_name");

      impl->target_name = target_name;
      
      impl->list.AddSource (stream_name);
      
      impl->UpdatePlayers ();
    }
    catch (xtl::exception& e)
    {
      e.touch ("media::player::MediaPlayer::MediaPlayer(const char*, const char*)");
      throw;
    }
  }
  catch (...)
  {
    delete impl;
    throw;
  }  
}

MediaPlayer::MediaPlayer (const char* target_name, const PlayList& list) 
  : impl (new Impl)
{
  try
  {
    try
    {       
      if (!target_name)
        throw xtl::make_null_argument_exception ("", "target_name");

      impl->target_name = target_name;

      impl->list = list.Clone ();

      impl->UpdatePlayers ();
    }
    catch (xtl::exception& e)
    {
      e.touch ("media::player::MediaPlayer::MediaPlayer(const char*, const PlayList&)");
      throw;
    }
  }
  catch (...)
  {
    delete impl;
    throw;
  }  
}

MediaPlayer::MediaPlayer (const MediaPlayer& player)
  : impl (player.impl)
{
  addref (impl);
}

MediaPlayer::~MediaPlayer ()
{
  release (impl);
}

MediaPlayer& MediaPlayer::operator = (const MediaPlayer& player)
{
  MediaPlayer (player).Swap (*this);

  return *this;
}

/*
    ������� ���� ������������
*/

void MediaPlayer::SetTarget (const char* target_name)
{
  try
  {
    MediaPlayer new_player (target_name, impl->list);
    
    new_player.impl->CopySettingsFrom (*impl);
    
    new_player.Swap (*this);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::player::MediaPlayer::SetTarget");
    throw;
  }
}

//��� ���� ������������
const char* MediaPlayer::Target () const
{
  return impl->target.c_str ();
}

/*
    �������� / �������� ������
*/

void MediaPlayer::Open (const char* stream_name)
{
  try
  {
    MediaPlayer new_player (impl->target_name.c_str (), stream_name);

    new_player.impl->CopySettingsFrom (*impl);

    new_player.Swap (*this);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::player::MediaPlayer::Open(const char*)");
    throw;
  }
}

void MediaPlayer::Open (const PlayList& list)
{
  try
  {
    MediaPlayer new_player (impl->target_name.c_str (), list);

    new_player.impl->CopySettingsFrom (*impl);

    new_player.Swap (*this);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::player::MediaPlayer::Open(const PlayList&)");
    throw;
  }
}

void MediaPlayer::Close ()
{
  try
  {
    MediaPlayer new_player (impl->target_name.c_str ());

    new_player.impl->CopySettingsFrom (*impl);

    new_player.Swap (*this);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::player::MediaPlayer::Close");
    throw;
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
  static const char* METHOD_NAME = "media::player::MediaPlayer::SetRepeatMode";
  
  if (mode < 0 || mode >= MediaPlayerRepeatMode_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "mode", mode);
    
  impl->repeat_mode = mode;
  
  //notify???
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
//?????????
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
    e.touch ("media::player::MediaPlayer::Source(size_t)");
    throw;
  }
}

//��� ������������� �����
const char* MediaPlayer::Source () const
{
  return Source (Track ());
}

//������������ �����
float MediaPlayer::Duration (size_t tack)
{  
  try
  {
    if (track >= impl->players.size ())
      throw xtl::make_range_exception ("", "track", track, impl->players.size ());

    return impl->players [track]->Duration ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::player::MediaPlayer::Duration");
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
//?????????
}

//������ / ���������� ������������
void MediaPlayer::Play ()
{
//?????????
}

//������������� ������������
void MediaPlayer::Pause ()
{
//?????????
}

//���������� ������������
void MediaPlayer::Stop ()
{
//?????????
}

/*
    ������� ������������
*/

void MediaPlayer::SetPosition (float position)
{
//?????????
}

float MediaPlayer::Position () const
{
//?????????
}

/*
    ���������� ����������
*/

//in range [0..1]
void MediaPlayer::SetVolume (float volume)
{
//?????????
}

float MediaPlayer::Volume () const
{
  return impl->volume;
}

//������� ��� �������� �� ����
void MediaPlayer::SetMute (bool state)
{
//?????????
}

bool MediaPlayer::IsMuted () const
{
  return impl->is_muted;
}

/*
    ���������� � �������� ������
*/

xtl::connection MediaPlayer::RegisterEventHandler (MediaPlayerEvent event, const EventHandler& handler)
{
  //?????????
}

/*
    �����
*/

void MediaPlayer::Swap (MediaPlayer& player)
{
  stl::swap (impl, player.impl);
}

namespace media
{

namespace player
{

void swap (MediaPlayer& player1, MediaPlayer& player2)
{
  player1.Swap (player2);
}

}

}
