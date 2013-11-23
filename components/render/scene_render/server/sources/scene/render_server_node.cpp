#include "shared.h"

using namespace render::scene;
using namespace render::scene::server;

/*
    �������� ���������� ����
*/

struct Node::Impl
{
  stl::string name;     //��� ����
  math::mat4f world_tm; //������� �������������� ����

/// ������������
  Impl ()
    : world_tm (1.0f)
  {
  }
};

/*
    ������������ / ���������� / ������������
*/

Node::Node ()
  : impl (new Impl)
{
}

Node::~Node ()
{
}

/*
    ��� ����
*/

void Node::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::scene::server::Node::SetName", "name");

  impl->name = name;
}

const char* Node::Name () const
{
  return impl->name.c_str ();
}

/*
    ������� ��������������
*/

void Node::SetWorldMatrix (const math::mat4f& tm)
{
  impl->world_tm = tm;
}

const math::mat4f& Node::WorldMatrix () const
{
  return impl->world_tm;
}

/*
    �����
*/

void Node::Visit (ISceneVisitor& visitor)
{
  try
  {
    VisitCore (visitor);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Visit(%s='%s')", typeid (*this).name (), Name ());
    throw;
  }
}

void Node::VisitCore (ISceneVisitor& visitor)
{
  visitor.Visit (*this);
}
