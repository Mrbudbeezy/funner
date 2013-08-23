#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

/*
    �������� ���������� ��������� ����������
*/

namespace
{

struct ServerLog
{
  common::Log log; //����� ����������� ����������������

  ServerLog (const char* name)
    : log (common::format ("render.server.%s", name).c_str ())
  {
    log.Printf ("Render server has been created");
  }

  ~ServerLog ()
  {
    log.Printf ("Render server has been destroyed");
  }
};

}

struct RenderManager::Impl: public ServerLog, public xtl::reference_counter
{
  manager::RenderManager render_manager;

/// �����������
  Impl (const char* name)
    : ServerLog (name)
  {
  }
};

/*
    ����������� / ����������
*/

RenderManager::RenderManager (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    impl = new Impl (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::RenderManager::RenderManager");
    throw;
  }
}

RenderManager::RenderManager (const RenderManager& manager)
  : impl (manager.impl)
{
  addref (impl);
}

RenderManager::~RenderManager ()
{
  release (impl);
}

RenderManager& RenderManager::operator = (const RenderManager& manager)
{
  RenderManager tmp (manager);

  stl::swap (impl, tmp.impl);

  return *this;
}

/*
    ����� ����������� ����������������
*/

common::Log& RenderManager::Log ()
{
  return impl->log;
}

/*
    ��������
*/

render::manager::RenderManager& RenderManager::Manager ()
{
  return impl->render_manager;
}
