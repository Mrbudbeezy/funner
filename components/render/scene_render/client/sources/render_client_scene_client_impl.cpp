#include "shared.h"

using namespace render::scene::client;

/*
    �������� ���������� ������� �����
*/

struct ClientImpl::Impl
{
};

/*
    ����������� / ����������
*/

ClientImpl::ClientImpl ()
  : impl (new Impl)
{
}

ClientImpl::~ClientImpl ()
{
}

/*
    ����������� ������� �������
*/

void ClientImpl::Dummy ()
{
}
