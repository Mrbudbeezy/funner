#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

using interchange::object_id_t;

/*
    �������� ���������� ��������� ����
*/

typedef stl::hash_map<object_id_t, Scene>   SceneMap;
typedef stl::hash_map<object_id_t, NodePtr> NodeMap;

struct SceneManager::Impl
{
  SceneMap scenes; //�����
  NodeMap  nodes;  //����
};

/*
    ����������� / ����������
*/

SceneManager::SceneManager  ()
  : impl (new Impl)
{
}

SceneManager::~SceneManager ()
{
}

/*
    ���������� � �������� ����
*/

Scene SceneManager::CreateScene (object_id_t id)
{
  SceneMap::iterator iter = impl->scenes.find (id);

  if (iter != impl->scenes.end ())
    throw xtl::format_operation_exception ("render::scene::server::SceneManager::CreateScene", "Scene with id %llu has been already added", id);

  return impl->scenes.insert_pair (id, Scene ()).first->second;
}

void SceneManager::RemoveScene (object_id_t id)
{
  impl->scenes.erase (id);
}

/*
    ����� �����
*/

Scene& SceneManager::GetScene (object_id_t id) const
{
  SceneMap::iterator iter = impl->scenes.find (id);

  if (iter == impl->scenes.end ())
    throw xtl::format_operation_exception ("render::scene::server::SceneManager::GetScene", "Scene with id %llu has not been added", id);

  return iter->second;
}

/*
    ���������� � �������� ����
*/

xtl::intrusive_ptr<Node> SceneManager::CreateNode (interchange::object_id_t id, interchange::NodeType type)
{
  NodeMap::iterator iter = impl->nodes.find (id);

  if (iter != impl->nodes.end ())
    throw xtl::format_operation_exception ("render::scene::server::SceneManager::CreateNode", "Node with id %llu has been already added", id);

  return impl->nodes.insert_pair (id, NodePtr (NodeFactory::CreateNode (type), false)).first->second;
}

void SceneManager::RemoveNode (interchange::object_id_t id)
{
  impl->nodes.erase (id);
}

/*
    ����� ����
*/

Node& SceneManager::GetNode (interchange::object_id_t id) const
{
  NodeMap::iterator iter = impl->nodes.find (id);

  if (iter == impl->nodes.end ())
    throw xtl::format_operation_exception ("render::scene::server::SceneManager::GetNode", "Node with id %llu has not been added", id);

  return *iter->second;
}
