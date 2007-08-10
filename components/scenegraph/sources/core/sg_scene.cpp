#include <sg/scene.h>
#include <stl/string>
#include <xtl/intrusive_ptr.h>
#include <common/exception.h>

#include "scene_object.h"

using namespace scene_graph;
using namespace stl;
using namespace common;

/*
    �������� ���������� �����
*/

//typedef xtl::com_ptr<ISceneManager> SceneManagerPtr;

struct Scene::Impl: public SceneSpace
{
//  SceneManagerPtr scene_manager; //�������� �����
  string name; //��� �����  
  Node*  root; //������ �����
};

/*
    ����������� / ����������
*/

Scene::Scene ()
  : impl (new Impl)
{
    //�������� ��������� ���� �����
    
  impl->root = Node::Create (*this);
}

Scene::~Scene ()
{
    //������������ ����� �����

  impl->root->UnbindAllChildren ();
  impl->root->Release ();

    //�������� ����������
  
  delete impl;
}
    
/*
    ��� �����
*/

void Scene::SetName (const char* name)
{
  if (!name)
    RaiseNullArgument ("scene_graph::Scene::SetName", "name");
    
  impl->name = name;
}

const char* Scene::Name () const
{
  return impl->name.c_str ();
}

/*
    ������ �����
*/

Node& Scene::Root ()
{
  return *impl->root;
}

const Node& Scene::Root () const
{
  return *impl->root;
}

/*
    ���������� �������� � �����
*/

size_t Scene::EntitiesCount () const
{
  return impl->ObjectsCount ();
}

/*
    ����������� �������� �����
*/

void Scene::Attach (SceneObject& object)
{
  object.BindToSpace (impl);
}

/*
    ����� ��������, ������������� �����
*/

void Scene::Traverse (const TraverseFunction& fn)
{
  SceneObject* object = impl->FirstObject ();

  for (size_t i=0, count=impl->ObjectsCount (); i<count; i++, object=object->NextObject ())
    fn (object->Entity ());
}

void Scene::Traverse (const ConstTraverseFunction& fn) const
{
  const SceneObject* object = impl->FirstObject ();

  for (size_t i=0, count=impl->ObjectsCount (); i<count; i++, object=object->NextObject ())
    fn (object->Entity ());
}

void Scene::VisitEach (Visitor& visitor) const
{
  SceneObject* object = const_cast<SceneObject*> (impl->FirstObject ());

  for (size_t i=0, count=impl->ObjectsCount (); i<count; i++, object=object->NextObject ())
    object->Entity ().Accept (visitor);
}

/*
    Node
    ������� ����� � �����
*/

void Node::BindToScene (scene_graph::Scene& scene, NodeBindMode mode, NodeTransformSpace invariant_space)
{
  BindToParent (scene.Root (), mode, invariant_space);
}
