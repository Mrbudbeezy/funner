#include <sg/scene.h>
#include <stl/string>
#include <xtl/intrusive_ptr.h>
#include <common/exception.h>

#include "scene_object.h"

using namespace scene_graph;
using namespace bound_volumes;
using namespace stl;
using namespace common;
using namespace xtl;

/*
    �������� ���������� �����
*/

//typedef xtl::com_ptr<ISceneManager> SceneManagerPtr;
typedef com_ptr<Node> NodePtr;

struct Scene::Impl: public SceneSpace
{
//  SceneManagerPtr scene_manager; //�������� �����
  string  name; //��� �����  
  NodePtr root; //������ �����
  
  template <class Fn>
  void ForEach (Fn& fn)
  {
    SceneObject* object = FirstObject ();

    for (size_t count=ObjectsCount (); count--; object=object->NextObject ())
      fn (object->Entity ());
  }
  
  template <class Fn>
  void ForEach (Fn& fn) const
  {
    const SceneObject* object = FirstObject ();

    for (size_t count=ObjectsCount (); count--; object=object->NextObject ())
      fn (object->Entity ());
  }  
};

/*
    ����������� / ����������
*/

Scene::Scene ()
  : impl (new Impl)
{
    //�������� ��������� ���� �����
    
  try
  {
    impl->root = Node::Create ();
    
      //��������� ��������� �� ������ �����

    impl->root->SetScene (this);        
  }
  catch (...)
  {
    delete impl;
    throw;
  }   
}

Scene::~Scene ()
{
    //������������ ����� �����

  impl->root->UnbindAllChildren ();

    //�������� ����������
  
  delete impl;
}
    
/*
    ��� �����
*/

void Scene::SetName (const char* name)
{
  if (!name)
    raise_null_argument ("scene_graph::Scene::SetName", "name");
    
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
    �������� ��� ������ �������� �����
*/

namespace
{

//������� ��� �������� ����������
struct VisitorWrapper
{
  VisitorWrapper (Scene::Visitor& in_visitor) : visitor (in_visitor) {}

  void operator () (Entity& entity) const
  {
    entity.Accept (visitor);
  }

  Scene::Visitor& visitor;
};

//������� ��� �������� ��������� � ��������� ��������� �������� � �������������� �����
template <class Fn>
struct BoundsCheckFunction
{
  BoundsCheckFunction (const aaboxf& in_box, Fn& in_fn) : box (in_box), fn (in_fn) {}
  
  template <class Entity> void operator () (Entity& entity) const
  {
      //���� ������ ����� ����������� �������������� ������ - ������������ ���

    if (entity.IsInfiniteBounds ())
    {
      fn (entity);
      return;
    }

      //���� ������ ����� �������� �������������� ������ - ��������� ��� ��������� � �������� �����

    if (intersects (entity.WorldBoundBox (), box))
      fn (entity);
  }

  const aaboxf& box;
  Fn&           fn;
};

}

/*
    ����� ��������, ������������� �����
*/

void Scene::Traverse (const TraverseFunction& fn)
{
  impl->ForEach (fn);
}

void Scene::Traverse (const ConstTraverseFunction& fn) const
{
  impl->ForEach (fn);
}

void Scene::VisitEach (Visitor& visitor) const
{
  const_cast<Impl*> (impl)->ForEach (VisitorWrapper (visitor));
}

/*
    ����� ��������, ������������� ����� � �������� � �������������� �����
*/

void Scene::Traverse (const aaboxf& box, const TraverseFunction& fn)
{  
  impl->ForEach (BoundsCheckFunction<const TraverseFunction> (box, fn));
}

void Scene::Traverse (const aaboxf& box, const ConstTraverseFunction& fn) const
{
  impl->ForEach (BoundsCheckFunction<const ConstTraverseFunction> (box, fn));
}

void Scene::VisitEach (const aaboxf& box, Visitor& visitor) const
{
  const_cast<Impl*> (impl)->ForEach (BoundsCheckFunction<VisitorWrapper> (box, VisitorWrapper (visitor)));
}
