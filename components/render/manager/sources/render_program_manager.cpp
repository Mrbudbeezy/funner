#include "shared.h"

using namespace render;

/*
    �������� ���������� ��������� ��������
*/

struct ProgramManager::Impl
{
  ProgramProxyManager program_proxy_manager; //�������� ������ ��������
};

/*
    ����������� / ����������
*/

ProgramManager::ProgramManager ()
  : impl (new Impl)
{
}

ProgramManager::~ProgramManager ()
{
}

/*
    ��������� ������
*/

ProgramProxy ProgramManager::GetProgramProxy (const char* name)
{
  return impl->program_proxy_manager.GetProxy (name);
}

/*
    ����� ����������� ���������
*/

ProgramPtr ProgramManager::FindProgram (const char* name)
{
  return impl->program_proxy_manager.FindResource (name);
}

/*
    ��������� ��������� �� ���������
*/

void ProgramManager::SetDefaultProgram (const ProgramPtr& program)
{
  impl->program_proxy_manager.SetDefaultResource (program);
}

ProgramPtr ProgramManager::DefaultProgram ()
{
  return impl->program_proxy_manager.DefaultResource ();
}
