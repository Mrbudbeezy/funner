#include "shared.h"

using namespace social;
using namespace social::facebook;

/*
   ��������� �� ���������
*/

/*
   ����� (���������� true, ���� �������� ���������� ������������ ��� ������ ��������� ������)
*/

void DefaultPlatform::Login (const char* app_id, const PlatformLoginCallback& callback, const common::PropertyMap& properties)
{
  callback (false, OperationStatus_Failure, "", "");
}

void DefaultPlatform::CancelLogin ()
{
}

void DefaultPlatform::LogOut ()
{
}

/*
   ���������� ������� ��������� ����������
*/

void DefaultPlatform::PublishAppInstallEvent (const char* app_id)
{
}
