#include "shared.h"

using namespace render::mid_level;

namespace
{

/*
    ���������
*/

const char* LOG_NAME = "render.mid_level"; //��� ���������

}

namespace render
{

namespace mid_level
{

/*
    �������� ���������� ���������
*/

class LogImpl: public xtl::reference_counter
{
  public:
    common::Log log; //��������

    LogImpl () : log (LOG_NAME) {}
};

}

}

namespace
{

/*
    ��������� ��� ���������� ���������
*/

struct ImplHolder
{
  ImplHolder () : impl (new LogImpl, false) {}

  xtl::intrusive_ptr<LogImpl> impl;
};

}

/*
    ������������ / ���������� / ������������
*/

Log::Log ()
{
  typedef common::Singleton<ImplHolder> ImplSingleton;

  impl = ImplSingleton::Instance ()->impl.get ();

  addref (impl);
}

Log::Log (const Log& log)
  : impl (log.impl)
{
  addref (impl);
}

Log::~Log ()
{
  release (impl);
}

Log& Log::operator = (const Log& log)
{
  Log tmp = log;

  stl::swap (impl, tmp.impl);

  return *this;
}

/*
    ������ ���������
*/

void Log::Print (const char* message) const
{
  impl->log.Print (message);
}

void Log::Printf (const char* message, ...) const
{
  va_list list;
  
  va_start (list, message);  
  VPrintf  (message, list);
}

void Log::VPrintf (const char* message, va_list list) const
{
  impl->log.VPrintf (message, list);
}
