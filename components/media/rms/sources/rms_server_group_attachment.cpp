#include "shared.h"

using namespace media::rms;

/*
    ����������� / ����������
*/

ServerGroupAttachment::ServerGroupAttachment (const char* name, ICustomServer& in_server)
  : group (name)
  , server (&in_server)
{
  group.Attach (*server);
}

ServerGroupAttachment::~ServerGroupAttachment ()
{
  group.Detach (*server);
}

/*
    ��������� ������ � �������
*/

ICustomServer& ServerGroupAttachment::Server () const
{
  return *server;
}

media::rms::ServerGroup ServerGroupAttachment::ServerGroup () const
{
  return group;
}
