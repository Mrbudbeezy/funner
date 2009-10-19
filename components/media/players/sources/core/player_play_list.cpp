#include "shared.h"

using namespace media::players;

/*
    �������� ���������� ������ ������������
*/

struct PlayList::Impl: public xtl::reference_counter
{
  common::StringArray sources; //������ ��� ����������
};

/*
    ������������ / ���������� / ������������
*/

PlayList::PlayList ()
  : impl (new Impl)
{
}

PlayList::PlayList (const PlayList& list)
  : impl (list.impl)
{
  addref (impl);
}

PlayList::~PlayList ()
{
  release (impl);
}

PlayList& PlayList::operator = (const PlayList& list)
{
  PlayList (list).Swap (*this);

  return *this;
}

/*
    ������������� ������
*/

size_t PlayList::Id () const
{
  return reinterpret_cast<size_t> (get_pointer (impl));
}

/*
    �����������
*/

PlayList PlayList::Clone () const
{
  try
  {
    PlayList list;

    list.impl->sources = impl->sources.Clone ();

    return list;
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::PlayList::Clone");
    throw;
  }
}

/*
    ���������� ���������� / �������� �� �������
*/

size_t PlayList::Size () const
{
  return impl->sources.Size ();
}

bool PlayList::IsEmpty () const
{
  return impl->sources.IsEmpty ();
}

/*
    ��������� ����������
*/

const char** PlayList::Items () const
{
  return impl->sources.Data ();
}

const char* PlayList::Item (size_t index) const
{
  try
  {
    return impl->sources [index];
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::PlayList::Item");
    throw;
  }
}

/*
    ���������� � �������� ����������
*/

size_t PlayList::AddSource (const char* source_name)
{
  try
  {
    if (!source_name)
      throw xtl::make_null_argument_exception ("", "source_name");
      
    return impl->sources.Add (source_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::PlayList::AddSource");
    throw;
  }
}

void PlayList::RemoveSource (size_t source_index)
{
  impl->sources.Remove (source_index);
}

void PlayList::RemoveSource (const char* source_name)
{
  if (!source_name)
    return;
    
  for (size_t i=0; i<impl->sources.Size ();)
    if (!strcmp (impl->sources [i], source_name)) impl->sources.Remove (i);
    else                                          i++;
}

void PlayList::Clear ()
{
  impl->sources.Clear ();
}

/*
    �������������� ������
*/

void PlayList::Reserve (size_t count)
{
  impl->sources.Reserve (count);
}

size_t PlayList::Capacity () const
{
  return impl->sources.Capacity ();
}

/*
    �����
*/

void PlayList::Swap (PlayList& list)
{
  stl::swap (impl, list.impl);  
}

namespace media
{

namespace player
{

void swap (PlayList& list1, PlayList& list2)
{
  list1.Swap (list2);
}

}

}
