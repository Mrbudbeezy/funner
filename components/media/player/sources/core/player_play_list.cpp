#include "shared.h"

using namespace media::player;

/*
    �������� ���������� ������ ������������
*/

struct PlayList::Impl
{
};

/*
    ������������ / ���������� / ������������
*/

PlayList::PlayList ()
{
}

PlayList::PlayList (const PlayList&)
{
}

PlayList::~PlayList ()
{
}

PlayList& PlayList::operator = (const PlayList&)
{
}

/*
    �����������
*/

PlayList PlayList::Clone () const
{
}

/*
    ���������� ����������
*/

size_t PlayList::Size () const
{
}

/*
    ��������� ����������
*/

const char** PlayList::Items () const
{
}

const char* PlayList::Item (size_t index) const
{
}

/*
    ���������� � �������� ����������
*/

size_t PlayList::AddSource (const char* source_name)
{
}

void PlayList::RemoveSource (size_t source_index)
{
}

void PlayList::RemoveSource (const char* source_name)
{
}

void PlayList::Clear ()
{
}

/*
    �����
*/

void PlayList::Swap (PlayList&);

namespace media
{

namespace player
{

void swap (PlayList&, PlayList&)
{
}

}

}
