#include <media/collada/utility.h>
#include <stl/string>

using namespace medialib::collada;

/*
    ���������� Entity
*/

struct Entity::Impl
{
  stl::string id;    //������������� �������
  ModelImpl*  owner; //��������
    
  Impl (ModelImpl* in_owner, const char* in_id) : owner (in_owner), id (in_id) {}
};

/*
    ����������� � ����������
*/

Entity::Entity (ModelImpl* owner, const char* id)
  : impl (new Impl (owner, id))
  {}
  
Entity::~Entity ()
{
  delete impl;
}

/*
    ��������
*/

ModelImpl* Entity::Owner () const
{
  return impl->owner;
}

/*
    ������������� �������
*/

const char* Entity::EntityID () const
{
  return impl->id.c_str ();
}
