#include "shared.h"

using namespace social;

/*
   ���������� ������� ��������
*/

struct Leaderboard::Impl : public xtl::reference_counter
{
  stl::string         id;          //�������������
  stl::string         title;       //��������
  Score               user_score;  //���� ������������� ������������
  common::PropertyMap properties;  //��������
  ScoreList           scores;      //���� �������
  const void*         handle;      //�������������� ����������

  Impl ()
    : handle (0)
    {}
};

/*
   ����������� / ���������� / �����������
*/

Leaderboard::Leaderboard ()
  : impl (new Impl)
  {}

Leaderboard::Leaderboard (const Leaderboard& source)
  : impl (source.impl)
{
  addref (impl);
}

Leaderboard::~Leaderboard ()
{
  release (impl);
}

Leaderboard& Leaderboard::operator = (const Leaderboard& source)
{
  Leaderboard (source).Swap (*this);

  return *this;
}

/*
   �������������
*/

const char* Leaderboard::Id () const
{
  return impl->id.c_str ();
}

void Leaderboard::SetId (const char* id)
{
  if (!id)
    throw xtl::make_null_argument_exception ("social::Leaderboard::SetId", "id");

  impl->id = id;
}

/*
   ��������
*/

const char* Leaderboard::Title () const
{
  return impl->title.c_str ();
}

void Leaderboard::SetTitle (const char* title)
{
  if (!title)
    throw xtl::make_null_argument_exception ("social::Leaderboard::SetTitle", "title");

  impl->title = title;
}

/*
   ���� ������������� ������������
*/

const Score& Leaderboard::UserScore () const
{
  return impl->user_score;
}

void Leaderboard::SetUserScore (const Score& score)
{
  impl->user_score = score;
}

/*
   ������ ��������
*/

const common::PropertyMap& Leaderboard::Properties () const
{
  return const_cast<Leaderboard&> (*this).Properties ();
}

common::PropertyMap& Leaderboard::Properties ()
{
  return impl->properties;
}

void Leaderboard::SetProperties (const common::PropertyMap& properties)
{
  impl->properties = properties;
}

/*
   �������
*/

const ScoreList& Leaderboard::Scores () const
{
  return const_cast<Leaderboard&> (*this).Scores ();
}

ScoreList& Leaderboard::Scores ()
{
  return impl->scores;
}

void Leaderboard::SetScores (const ScoreList& scores)
{
  impl->scores = scores;
}

/*
   ���������/��������� ��������������� �����������
*/

const void* Leaderboard::Handle () const
{
  return impl->handle;
}

void Leaderboard::SetHandle (const void* handle)
{
  impl->handle = handle;
}

/*
   �����
*/

void Leaderboard::Swap (Leaderboard& source)
{
  stl::swap (impl, source.impl);
}

namespace social
{

/*
   �����
*/

void swap (Leaderboard& leaderboard1, Leaderboard& leaderboard2)
{
  leaderboard1.Swap (leaderboard2);
}

}
