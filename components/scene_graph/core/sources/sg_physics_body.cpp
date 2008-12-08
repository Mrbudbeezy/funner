#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::physics;

/*
    �������� ���������� ����������� ����
*/

struct Body::Impl
{
  stl::string model_name; //��� ���������� ������
};

/*
    ����������� / ����������
*/

Body::Body ()
  : impl (new Impl)
{
}

Body::~Body ()
{
}

/*
    �������� ����
*/

Body::Pointer Body::Create ()
{
  return Pointer (new Body, false);
}

/*
    ��������� ���������� ������
*/

void Body::SetModelName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("scene_graph::physics::Body::SetModelName", "name");
    
  impl->model_name = name;
}

const char* Body::ModelName () const
{
  return impl->model_name.c_str ();
}

/*
    �����, ���������� ��� ��������� ������� �������
*/

void Body::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);  
}
