#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

namespace
{

/*
    ���������
*/

const char* LOG_NAME = "render.scene_render";

}

namespace render
{

namespace scene_render3d
{

/*
    �������� ���������� ���������
*/

class LogImpl: public xtl::reference_counter
{
  public:
    common::Log log; //��������

    LogImpl (const char* log_name) : log (log_name) {}
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
  ImplHolder () : impl (new LogImpl (LOG_NAME), false) {}

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
