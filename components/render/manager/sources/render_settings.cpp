#include "shared.h"

using namespace render;

/*
    �������� ���������� �������� ��������� ����������
*/

struct Settings::Impl
{
  render::LogLevel log_level; //������� ����������������
  
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

void Settings::SetLogLevel (render::LogLevel level)
{
  switch (level)
  {
    case LogLevel_Info:
    case LogLevel_Debug:
      break;
    default:
      throw xtl::make_argument_exception ("render::Settings::SetLogLevel", "level", level);
  }
  
  impl->log_level = level;
}

LogLevel Settings::LogLevel ()
{
  return impl->log_level;
}
