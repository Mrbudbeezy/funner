#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>

#include <common/component.h>
#include <common/log.h>

#include <media/players/stream.h>

namespace media
{

namespace players
{

namespace iphone
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
IStreamPlayer* create_movie_player (const char* stream_name, const StreamPlayerManager::StreamEventHandler* handler); //��� ������� ��������� ������������ ���������, ��������� ���������� �� ������������ �������� ��������� �� ������ ��� mm ����
IStreamPlayer* create_music_player (const char* stream_name, const StreamPlayerManager::StreamEventHandler* handler);

}

}

}
