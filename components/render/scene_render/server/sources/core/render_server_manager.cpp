#include "shared.h"

using namespace render::scene::interchange;

/*
    �������� ���������� ������� ���������� �����
*/

struct SceneRenderServer::Impl
{
};

/*
    ����������� / ����������
*/

SceneRenderServer::SceneRenderServer (const char* name)
{
}

SceneRenderServer::~SceneRenderServer ()
{
}

/*
    ������������� ����
*/

void SceneRenderServer::AttachWindow (const char* name, syslib::Window& window, const char* init_string)
{
}

void SceneRenderServer::DetachWindow (const char* name)
{
}

void SceneRenderServer::DetachAllWindows ()
{
}
