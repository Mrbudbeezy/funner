#include "shared.h"

using namespace render::low_level::opengl;
using namespace render::low_level;

/*
    ������� ��뫮�
*/

void Object::AddRef ()
{
  addref (this);
}

void Object::Release ()
{
  release (this);
}

/*
    ���᮪ ᢮��� ��ꥪ�
*/

IPropertyList* Object::GetProperties ()
{
  typedef common::Singleton<DefaultPropertyList> DefaultPropertyListSingleton;
  
  return DefaultPropertyListSingleton::InstancePtr ();
}
