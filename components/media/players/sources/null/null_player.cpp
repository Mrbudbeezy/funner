#include <xtl/common_exceptions.h>
#include <xtl/function.h>

#include <common/component.h>

#include <media/players/stream.h>

using namespace media::players;

namespace components
{

namespace null_player
{

const char* TARGET_NAME = "null";

/*
    ������ �������������
*/

class NullPlayer: public IStreamPlayer
{
  public:
///�����������
    NullPlayer (const StreamPlayerManager::StreamEventHandler& in_handler)
      : handler (in_handler)
      {}

///������������ ������
    float Duration ()
    {
      return 1.f;
    }

///������ ������������
    void Play ()
    {
      OnEvent (StreamEvent_OnPlay);
      OnEvent (StreamEvent_OnAutoStop);
    }

///������������� ������������
    void Pause ()
    {
    }

///���������� ������������
    void Stop ()
    {
      OnEvent (StreamEvent_OnManualStop);
    }

///���������� ������� ������������
    void SetPosition (float position)
    {
    }

///�������� ������� ������������
    float Position ()
    {
      return 0.f;
    }

///��������� ������ ������������ ������������
    void SetLooping (bool state)
    {
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

  private:
    typedef StreamPlayerManager::StreamEventHandler StreamEventHandler;

  private:
    StreamEventHandler handler; //���������� �������
};


/*
   ��������� ������������ �����
*/

class PlayerComponent
{
  public:
    //�������� ����������
    PlayerComponent ()
    {
      StreamPlayerManager::RegisterPlayer (TARGET_NAME, &PlayerComponent::CreatePlayer);
    }

    static IStreamPlayer* CreatePlayer (const char* target_name, const char* source_name, const StreamPlayerManager::StreamEventHandler& handler)
    {
      static const char* METHOD_NAME = "media::players::null:PlayerComponent::CreatePlayer";

      if (!target_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "target_name");

      if (!source_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "source_name");

      if (!xtl::xstrcmp (target_name, TARGET_NAME))
        return new NullPlayer (handler);

      throw xtl::make_argument_exception (METHOD_NAME, "target_name", target_name);
    }
};

extern "C"
{

common::ComponentRegistrator<PlayerComponent> NullPlayer ("media.players.stream.null");

}

}

}
