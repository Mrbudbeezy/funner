#include "shared.h"

using namespace render::manager;

/*
    �������� ���������� �������� ��������� ����������
*/

struct Settings::Impl
{
  render::manager::LogLevel log_level; //������� ����������������
  
  Impl ()
    : log_level (LogLevel_Default)
  {
  }
};

/*
    ����������� / ����������
*/

Settings::Settings ()
  : impl (new Impl)
{
}

Settings::~Settings ()
{
}

/*
    ���������� �����������������
*/

void Settings::SetLogLevel (render::manager::LogLevel level)
{
  switch (level)
  {
    case LogLevel_Info:
    case LogLevel_Debug:
      break;
    default:
      throw xtl::make_argument_exception ("render::manager::Settings::SetLogLevel", "level", level);
  }
  
  impl->log_level = level;
}

LogLevel Settings::LogLevel ()
{
  return impl->log_level;
}
