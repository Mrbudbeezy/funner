#include "shared.h"

using namespace render::scene::client;

/*
    �������� ���������� ������� �����
*/

struct SceneRenderImpl::Impl
{
};

/*
    ����������� / ����������
*/

SceneRenderImpl::SceneRenderImpl ()
  : impl (new Impl)
{
}

SceneRenderImpl::~SceneRenderImpl ()
{
}

/*
    ����������� ������� �������
*/

void SceneRenderImpl::Dummy ()
{
}
