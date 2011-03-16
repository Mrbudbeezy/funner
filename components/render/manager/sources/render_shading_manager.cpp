#include "shared.h"

using namespace render;

/*
    �������� ���������� ��������� ��������
*/

struct ShadingManager::Impl
{
  ProgramProxyManager program_proxy_manager; //�������� ������ ��������
};

/*
    ����������� / ����������
*/

ShadingManager::ShadingManager ()
  : impl (new Impl)
{
}

ShadingManager::~ShadingManager ()
{
}

/*
    ��������� ������
*/

ProgramProxy ShadingManager::GetProgramProxy (const char* name)
{
  return impl->program_proxy_manager.GetProxy (name);
}

/*
    ����� ����������� ���������
*/

LowLevelProgramPtr ShadingManager::FindProgram (const char* name)
{
  return impl->program_proxy_manager.FindResource (name);
}

/*
    ��������� ��������� �� ���������
*/

void ShadingManager::SetDefaultProgram (const LowLevelProgramPtr& program)
{
  impl->program_proxy_manager.SetDefaultResource (program);
}

LowLevelProgramPtr ShadingManager::DefaultProgram ()
{
  return impl->program_proxy_manager.DefaultResource ();
}
