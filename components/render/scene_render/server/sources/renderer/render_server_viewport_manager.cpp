#include "shared.h"

using namespace render;
using namespace render::scene::server;

/*
    �������� ���������� ��������� �������� ������
*/

typedef stl::hash_map<object_id_t, Viewport> ViewportMap;

struct ViewportManager::Impl: public xtl::reference_counter
{
  ViewportMap viewports;      //������� ������
  size_t      max_draw_depth; //������������ ������� ����������� ����������

/// �����������
  Impl ()
    : max_draw_depth (1)
  {
  }
};

/*
    ����������� / ����������
*/

ViewportManager::ViewportManager ()
  : impl (new Impl)
{
}

ViewportManager::ViewportManager (const ViewportManager& manager)
  : impl (manager.impl)
{
  addref (impl);
}

ViewportManager::~ViewportManager ()
{
  release (impl);
}

ViewportManager& ViewportManager::operator = (const ViewportManager& manager)
{
  ViewportManager tmp (manager);

  stl::swap (impl, tmp.impl);

  return *this;
}

/*
    ��������� ������� ������
*/

Viewport& ViewportManager::GetViewport (object_id_t id)
{
  ViewportMap::iterator iter = impl->viewports.find (id);

  if (iter == impl->viewports.end ())
    throw xtl::format_operation_exception ("render::scene::server::ViewportManager::GetViewport", "Viewport with id %llu has not been added", id);

  return iter->second;
}

/*
    �������� ������� ������ / �������� ������� ������
*/

void ViewportManager::AddViewport (object_id_t id, const Viewport& viewport)
{
  ViewportMap::iterator iter = impl->viewports.find (id);

  if (iter != impl->viewports.end ())
    throw xtl::format_operation_exception ("render::scene::server::ViewportManager::GetViewport", "Viewport with id %llu has been already added", id);

  impl->viewports.insert_pair (id, viewport);
}

void ViewportManager::RemoveViewport (object_id_t id)
{
  impl->viewports.erase (id);
}

/*
    ������������ ������� ����������� ����������
*/

void ViewportManager::SetMaxDrawDepth (size_t level)
{
  try
  {
    if (level == impl->max_draw_depth)
      return;

    for (ViewportMap::iterator iter=impl->viewports.begin (), end=impl->viewports.end (); iter!=end; ++iter)
    {
      iter->second.SetMaxDrawDepth (level);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ViewportManager::SetMaxDrawDepth");
    throw;
  }
}

size_t ViewportManager::MaxDrawDepth () const
{
  return impl->max_draw_depth;
}
