#include "shared.h"

using namespace social;

/*
   ���������� ������������
*/

struct User::Impl : public xtl::reference_counter
{
  stl::string         id;         //�������������
  stl::string         nickname;   //���
  bool                is_friend;  //�������� �� ������ �������� ������������
  common::PropertyMap properties; //������ ��������
  const void*         handle;     //�������������� ����������

  Impl ()
    : is_friend (false)
    , handle (0)
    {}
};

/*
   ����������� / ���������� / �����������
*/

User::User ()
  : impl (new Impl)
  {}

User::User (const User& source)
  : impl (source.impl)
{
  addref (impl);
}

User::~User ()
{
  release (impl);
}

User& User::operator = (const User& source)
{
  User (source).Swap (*this);

  return *this;
}

/*
   �������������
*/

const char* User::Id () const
{
  return impl->id.c_str ();
}

void User::SetId (const char* id)
{
  if (id)
    throw xtl::make_null_argument_exception ("social::User::SetId", "id");

  impl->id = id;
}

/*
   ���
*/

const char* User::Nickname () const
{
  return impl->nickname.c_str ();
}

void User::SetNickname (const char* nickname)
{
  if (!nickname)
    throw xtl::make_null_argument_exception ("social::User::SetNickname", "nickname");

  impl->nickname = nickname;
}

/*
   �������� �� ������ �������� ������������
*/

bool User::IsFriend () const
{
  return impl->is_friend;
}

void User::SetFriend (bool is_friend)
{
  impl->is_friend = is_friend;
}

/*
   ������ ��������
*/

const common::PropertyMap& User::Properties () const
{
  return impl->properties;
}

common::PropertyMap& User::Properties ()
{
  return impl->properties;
}

void User::SetProperties (const common::PropertyMap& properties)
{
  impl->properties = properties;
}

/*
   �������������� �����������
*/

const void* User::Handle () const
{
  return impl->handle;
}

void User::SetHandle (const void* handle)
{
  impl->handle = handle;
}

/*
   �����
*/

void User::Swap (User& user)
{
  stl::swap (impl, user.impl);
}

namespace social
{

/*
   �����
*/

void swap (User& user1, User& user2)
{
  user1.Swap (user2);
}

}
