#include "shared.h"

#import <MPMoviePlayerController.h>

using namespace media::players;
using namespace media::players::iphone;

namespace
{

class MoviePlayer;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
@interface VideoPlayerListener : NSObject
{
  @private
    MoviePlayer *player;  //�������������
}

-(id) init;
-(id) initWithPlayer:(MoviePlayer*)in_player;

@end

namespace
{

/*
    ������������� ����� �� iPhone
*/

class MoviePlayer: public IStreamPlayer
{
  public:
///�����������
    MoviePlayer (const char* in_stream_name, const StreamPlayerManager::StreamEventHandler& in_handler, VideoPlayerControlsType controls_type)
      : handler (in_handler), player (0)
    {
      static const char* METHOD_NAME = "media::players::iphone::MoviePlayer::MoviePlayer";

      NSString *ns_file_name = [[NSString alloc] initWithCString:in_stream_name encoding:NSASCIIStringEncoding];

      NSURL *file_url = [[NSURL alloc] initFileURLWithPath: ns_file_name];

      [ns_file_name release];

      if (!file_url)
        throw xtl::make_argument_exception (METHOD_NAME, "stream_name", in_stream_name, "Can't create NSURL from stream_name");

      player = [[MPMoviePlayerController alloc] initWithContentURL: file_url];

      [file_url release];

      if (!player)
        throw xtl::format_operation_exception (METHOD_NAME, "Can't open file '%s' for playing", in_stream_name);

      switch (controls_type)
      {
        case VideoPlayerControlsType_NoControls:     player.movieControlMode = MPMovieControlModeHidden;     break;
        case VideoPlayerControlsType_VolumeControls: player.movieControlMode = MPMovieControlModeVolumeOnly; break;
        case VideoPlayerControlsType_AllControls:    player.movieControlMode = MPMovieControlModeDefault;    break;
        default:
          [player release];
          throw xtl::make_argument_exception (METHOD_NAME, "controls_type", controls_type, "Unknown controls type");
      }

      player_listener = [[VideoPlayerListener alloc] initWithPlayer:(this)];
    }
  
///����������
    ~MoviePlayer ()
    {
      [player_listener release];
      [player release];
    }

///������������ ������
    float Duration ()
    {
      throw xtl::format_not_supported_exception("media::players::iphone::MoviePlayer::Duration");
    }

///������ ������������
    void Play ()
    {
      [player play];

      OnEvent (StreamEvent_OnPlay);
    }
    
///������������� ������������
    void Pause ()
    {
      throw xtl::format_not_supported_exception("media::players::iphone::MoviePlayer::Pause");
    }
    
///���������� ������������
    void Stop ()
    {
      [player stop];

      OnEvent (StreamEvent_OnManualStop);
    }
    
///���������� ������� ������������
    void SetPosition (float position)
    {
      if (position == 0.f)
        return;

      throw xtl::format_not_supported_exception("media::players::iphone::MoviePlayer::SetPosition");
    }

///�������� ������� ������������
    float Position ()
    {
      throw xtl::format_not_supported_exception("media::players::iphone::MoviePlayer::Position");
    }

///��������� ������ ������������ ������������
    void SetLooping (bool state)
    {
      if (state)
        throw xtl::make_argument_exception ("media::players::iphone::MoviePlayer::SetLooping", "state", state, "Looping not supported");
    }

///���������� ���������
    void SetVolume (float volume)
    {
    }

///���������� � �������
    void OnEvent (StreamEvent event)
    {
      handler (this, event);
    }

///���������� � �������
    MPMoviePlayerController* GetPlayer ()
    {
      return player;
    }

  private:
    typedef StreamPlayerManager::StreamEventHandler StreamEventHandler;

  private:
    StreamEventHandler      handler;           //���������� �������
    MPMoviePlayerController *player;           //������������� �����
    VideoPlayerListener     *player_listener;  //��������� ������� �������������
};

}

@implementation VideoPlayerListener

-(id) init
{
  return [self initWithPlayer: 0];
}

-(id) initWithPlayer:(MoviePlayer*)in_player
{
  self = [super init];

  if (!self)
    return nil;

  try
  {
    if (!in_player)
      throw xtl::make_null_argument_exception ("media::players::iphone::VideoPlayerListener::InitWithPlayer", "in_player");

    player = in_player;
  }
  catch (xtl::exception& e)
  {
    printf ("%s\n", e.what ());

    [self release];
    return nil;
  }
  catch (...)
  {
    [self release];
    return nil;
  }

  NSNotificationCenter *notificationCenter = [NSNotificationCenter defaultCenter];

  [notificationCenter addObserver:self selector:@selector (handlePlaybackDidFinishNotification)
                      name:MPMoviePlayerPlaybackDidFinishNotification
                      object:player->GetPlayer ()];

  return self;
}

-(void) dealloc
{
  [[NSNotificationCenter defaultCenter] removeObserver:self];

  [super dealloc];
}

-(void) handlePlaybackDidFinishNotification
{
  player->OnEvent (StreamEvent_OnAutoStop);
}

@end


namespace media
{

namespace players
{

namespace iphone
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
IStreamPlayer* create_movie_player (const char* stream_name, const StreamPlayerManager::StreamEventHandler* handler, VideoPlayerControlsType controls_type)
{
  if (!stream_name)
    throw xtl::make_null_argument_exception ("media::players::iphone::create_movie_player", "stream_name");

  return new MoviePlayer (stream_name, *handler, controls_type);
}

}

}

}
