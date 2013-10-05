#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

/*
    ���������
*/

const size_t LISTENER_ARRAY_RESERVE_SIZE = 32; //������������� ���������� ����������

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

typedef stl::vector<IRenderManagerListener*> ListenerArray;

struct RenderManager::Impl: public ServerLog, public xtl::reference_counter
{
  manager::RenderManager render_manager;                   //�������� ����������
  xtl::auto_connection   configuration_changed_connection; //���������� � ������������ ��������� ������������
  ListenerArray          listeners;                        //��������� �������

/// �����������
  Impl (const char* name)
    : ServerLog (name)
  {
    configuration_changed_connection = render_manager.RegisterEventHandler (manager::RenderManagerEvent_OnConfigurationChanged, xtl::bind (&Impl::OnConfigurationChanged, this));

    listeners.reserve (LISTENER_ARRAY_RESERVE_SIZE);
  }

///���������� ��������� ������������
  void OnConfigurationChanged ()
  {
    common::ParseNode configuration = render_manager.Configuration ();

    for (ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
    {
      try
      {
        (*iter)->OnRenderManagerConfigurationChanged (configuration);
      }
      catch (...)
      {
      }
    }
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

/*
    �������� �� ������� ���������
*/

void RenderManager::AttachListener (IRenderManagerListener* listener)
{
  if (!listener)
    throw xtl::make_null_argument_exception ("render::scene::server::RenderManager::AttachListener", "listener");

  impl->listeners.push_back (listener);
}

void RenderManager::DetachListener (IRenderManagerListener* listener)
{
  if (!listener)
    return;

  impl->listeners.erase (stl::remove (impl->listeners.begin (), impl->listeners.end (), listener), impl->listeners.end ());
}

void RenderManager::DetachAllListeners ()
{
  impl->listeners.clear ();
}
