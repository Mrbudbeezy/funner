#include "shared.h"

using namespace social;

/*
   ���������� ���������� � ������� ��������
*/

struct Score::Impl : public xtl::reference_counter
{
  stl::string         player_id;       //������������� ������, �������� ����������� ����������
  stl::string         leaderboard_id;  //������������� ������� ��������, ������� ����������� ����������
  stl::string         user_data;       //������, ��������������� � �����������
  stl::string         formatted_value; //��������� ������������� ��������
  double              value;           //��������
  size_t              rank;            //���� � �������
  common::PropertyMap properties;      //������ ��������
  const void*         handle;          //�������������� ����������

  Impl ()
    : value (0)
    , rank (0)
    , handle (0)
    {}
};

/*
   ����������� / ���������� / �����������
*/

Score::Score ()
  : impl (new Impl)
  {}

Score::Score (const Score& source)
  : impl (source.impl)
{
  addref (impl);
}

Score::~Score ()
{
  release (impl);
}

Score& Score::operator = (const Score& source)
{
  Score (source).Swap (*this);

  return *this;
}

/*
   ������������� ������, �������� ����������� ����������
*/

const char* Score::PlayerId () const
{
  return impl->player_id.c_str ();
}

void Score::SetPlayerId (const char* player_id)
{
  if (!player_id)
    throw xtl::make_null_argument_exception ("social::Score::SetPlayerId", "player_id");

  impl->player_id = player_id;
}

/*
   ������������� ������� ��������, ������� ����������� ����������
*/

const char* Score::LeaderboardId () const
{
  return impl->leaderboard_id.c_str ();
}

void Score::SetLeaderboardId (const char* leaderboard_id)
{
  if (!leaderboard_id)
    throw xtl::make_null_argument_exception ("social::Score::SetLeaderboardId", "leaderboard_id");

  impl->leaderboard_id = leaderboard_id;
}

/*
   ������, ��������������� � �����������
*/

const char* Score::UserData () const
{
  return impl->user_data.c_str ();
}

void Score::SetUserData (const char* user_data)
{
  if (user_data)
    throw xtl::make_null_argument_exception ("social::Score::SetUserData", "user_data");

  impl->user_data = user_data;
}

/*
   ��������
*/

double Score::Value () const
{
  return impl->value;
}

void Score::SetValue (double value)
{
  impl->value = value;
}

/*
   ��������� ������������� ��������
*/

const char* Score::FormattedValue () const
{
  return impl->formatted_value.c_str ();
}

void Score::SetFormattedValue (const char* formatted_value)
{
  if (formatted_value)
    throw xtl::make_null_argument_exception ("social::Score::SetFormattedValue", "formatted_value");

  impl->formatted_value = formatted_value;
}

/*
   ���� � �������
*/

size_t Score::Rank () const
{
  return impl->rank;
}

void Score::SetRank (size_t rank)
{
  impl->rank = rank;
}

/*
   ������ ��������
*/

const common::PropertyMap& Score::Properties () const
{
  return impl->properties;
}

common::PropertyMap& Score::Properties ()
{
  return impl->properties;
}

void Score::SetProperties (const common::PropertyMap& properties)
{
  impl->properties = properties;
}

/*
   �������������� ����������
*/

const void* Score::Handle () const
{
  return impl->handle;
}

void Score::SetHandle (const void* handle)
{
  impl->handle = handle;
}

/*
   �����
*/

void Score::Swap (Score& source)
{
  stl::swap (source.impl, impl);
}

namespace social
{

/*
   �����
*/

void swap (Score& score1, Score& score2)
{
  score1.Swap (score2);
}

}
