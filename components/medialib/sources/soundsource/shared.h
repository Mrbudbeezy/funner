#ifndef MEDIALIB_SOUND_SOURCE_SHARED_HEADER
#define MEDIALIB_SOUND_SOURCE_SHARED_HEADER

#include <stl/vector>
#include <common/singleton.h>
#include <common/exception.h>
#include <media/sound_source.h>
#include <shared/shared_resource.h>
#include <shared/resource_manager.h>

namespace media
{

/*
    ������� ���������� �������
*/

class SoundSourceSystemImpl: public ResourceManager<SoundSourceSystem::LoadHandler, SoundSourceSystem::SaveHandler>
{
  public:
    SoundSourceSystemImpl ();
};

typedef common::Singleton<SoundSourceSystemImpl> SoundSourceSystemSingleton;

}

#endif
