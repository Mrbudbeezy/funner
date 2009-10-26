#include "shared.h"

using namespace media::players;

/*
    ������������� ����� �� iPhone
*/

class MoviePlayer: public IStreamPlayer
{
  public:
///�����������
  
  
///����������
    ~MoviePlayer ()
    {
    }

///������������ ������
    float Duration ()
    {
      throw xtl::make_not_implemented_exception("media::players::iphone::MoviePlayer::Duration");
    }

///������ ������������
    void  Play ()
    {
    }
    
///������������� ������������
    void Pause ()
    {
    }
    
///���������� ������������
    void  Stop        ()
    {
    }
    
///���������� ������� ������������
    void  SetPosition (float position)
    {
    }

///�������� ������� ������������
    float Position ()
    {
      throw xtl::make_not_implemented_exception("media::players::iphone::MoviePlayer::Position");
    }

///��������� ������ ������������ ������������
    void SetLooping  (bool state)
    {
    }

///���������� ���������
    void SetVolume (float volume)
    {
    }
};

namespace media
{

namespace players
{

namespace iphone
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
IStreamPlayer* create_movie_player (const char* stream_name, const StreamPlayerManager::StreamEventHandler& handler)
{
  throw xtl::make_not_implemented_exception("media::players::iphone::create_movie_player");
}

}

}

}
