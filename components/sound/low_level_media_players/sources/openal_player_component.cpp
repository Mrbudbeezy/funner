#include "shared.h"

using namespace media::players;

namespace components
{

namespace openal_media_player
{

const char* TARGET_NAME = "low_level_openal";


/*
   ��������� ������������ �����
*/

class PlayerComponent
{
  public:
    //�������� ����������
    PlayerComponent ()
    {
//      StreamPlayerManager::RegisterPlayer (TARGET_NAME, &PlayerComponent::CreatePlayer);
    }

    static IStreamPlayer* CreatePlayer (const char* target_name, const char* source_name, const StreamPlayerManager::StreamEventHandler& handler)
    {
      static const char* METHOD_NAME = "sound::low_level_media_players::openal:PlayerComponent::CreatePlayer";

      if (!target_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "target_name");

      if (!source_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "source_name");

//      if (!xtl::xstrcmp (target_name, TARGET_NAME))
//        return new NullPlayer (handler);

      throw xtl::make_argument_exception (METHOD_NAME, "target_name", target_name);
    }
};

extern "C"
{

common::ComponentRegistrator<PlayerComponent> OpenALPlayer ("media.players.stream.openal");

}

}

}
