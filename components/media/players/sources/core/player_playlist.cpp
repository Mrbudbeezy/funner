#include "shared.h"

using namespace media::players;

/*
    Описание реализации списка проигрывания
*/

struct Playlist::Impl: public xtl::reference_counter
{
  common::StringArray sources; //массив имён источников
};

/*
    Конструкторы / деструктор / присваивание
*/

Playlist::Playlist ()
  : impl (new Impl)
{
}

Playlist::Playlist (const Playlist& list)
  : impl (list.impl)
{
  addref (impl);
}

Playlist::~Playlist ()
{
  release (impl);
}

Playlist& Playlist::operator = (const Playlist& list)
{
  Playlist (list).Swap (*this);

  return *this;
}

/*
    Идентификатор списка
*/

size_t Playlist::Id () const
{
  return reinterpret_cast<size_t> (xtl::get_pointer (impl));
}

/*
    Копирование
*/

Playlist Playlist::Clone () const
{
  try
  {
    Playlist list;

    list.impl->sources = impl->sources.Clone ();

    return list;
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::Playlist::Clone");
    throw;
  }
}

/*
    Количество источников / проверка на пустоту
*/

unsigned int Playlist::Size () const
{
  return (unsigned int)impl->sources.Size ();
}

bool Playlist::IsEmpty () const
{
  return impl->sources.IsEmpty ();
}

/*
    Получение источников
*/

const char** Playlist::Items () const
{
  return impl->sources.Data ();
}

const char* Playlist::Item (unsigned int index) const
{
  try
  {
    return impl->sources [index];
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::Playlist::Item");
    throw;
  }
}

/*
    Добавление и удаление источников
*/

unsigned int Playlist::AddSource (const char* source_name)
{
  try
  {
    if (!source_name)
      throw xtl::make_null_argument_exception ("", "source_name");
      
    if (impl->sources.Size () >= (unsigned int)-1)
      throw xtl::format_operation_exception ("", "Can't add source, sources count limit exceeded");

    return (unsigned int)impl->sources.Add (source_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::Playlist::AddSource");
    throw;
  }
}

void Playlist::RemoveSource (unsigned int source_index)
{
  impl->sources.Remove (source_index);
}

void Playlist::RemoveSource (const char* source_name)
{
  if (!source_name)
    return;
    
  for (unsigned int i=0; i<impl->sources.Size ();)
    if (!strcmp (impl->sources [i], source_name)) impl->sources.Remove (i);
    else                                          i++;
}

void Playlist::Clear ()
{
  impl->sources.Clear ();
}

/*
    Резервирование памяти
*/

void Playlist::Reserve (unsigned int count)
{
  impl->sources.Reserve (count);
}

unsigned int Playlist::Capacity () const
{
  return (unsigned int)impl->sources.Capacity ();
}

/*
    Обмен
*/

void Playlist::Swap (Playlist& list)
{
  stl::swap (impl, list.impl);  
}

namespace media
{

namespace player
{

void swap (Playlist& list1, Playlist& list2)
{
  list1.Swap (list2);
}

}

}
