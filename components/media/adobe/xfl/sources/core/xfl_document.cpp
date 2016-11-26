#include "shared.h"

using namespace media::adobe::xfl;

typedef media::CollectionImpl<Resource, INamedCollection<Resource> > ResourceCollection;
typedef media::CollectionImpl<Symbol,   INamedCollection<Symbol> >   SymbolCollection;
typedef media::CollectionImpl<Timeline, INamedCollection<Timeline> > TimelineCollection;

namespace
{

const char*  XFL_LOADERS_MASK   = "media.adobe.xfl.loaders.*"; //маска имён компонентов загрузки xfl-документов
const float  DEFAULT_FRAME_RATE = 25.0f;                       //количество кадров в секунду по умолчанию
const size_t DEFAULT_WIDTH      = 1;                           //ширина по умолчанию
const size_t DEFAULT_HEIGHT     = 1;                           //высота по умолчанию

}

/*
    Описание реализации XFL документа
*/

struct Document::Impl : public xtl::reference_counter
{
  stl::string        name;             //имя документа
  math::vec3f        background_color; //цвет фона
  float              framerate;        //частота кадров
  size_t             width;            //ширина
  size_t             height;           //высота
  ResourceCollection resources;        //ресурсы
  SymbolCollection   symbols;          //элементы анимации
  TimelineCollection timelines;        //анимации
  
  Impl ()
    : framerate (DEFAULT_FRAME_RATE)
    , width (DEFAULT_WIDTH)
    , height (DEFAULT_HEIGHT)
  {
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

Document::Document ()
  : impl (new Impl)
  {}
  
Document::Document (const Document& source)
  : impl (source.impl)
{
  addref (impl);
}

Document::Document (const char* file_name)
  : impl (new Impl)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");

    static common::ComponentLoader loader (XFL_LOADERS_MASK);    

    DocumentManager::GetLoader (file_name, common::SerializerFindMode_ByName)(file_name, *this);
  }
  catch (xtl::exception& exception)
  {
    release (impl);
    exception.touch ("media::adobe::xfl::Document::Document(const char*)");
    throw;
  }
}

Document::~Document ()
{
  release (impl);
}

Document& Document::operator = (const Document& source)
{
  Document (source).Swap (*this);

  return *this;
}

/*
   Имя
*/

const char* Document::Name () const
{
  return impl->name.c_str ();
}

void Document::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::adobe::xfl::Document::SetName", "name");

  impl->name = name;
}

/*
   Цвет фона
*/

const math::vec3f& Document::BackgroundColor () const
{
  return impl->background_color;
}

void Document::SetBackgroundColor (const math::vec3f& background_color)
{
  impl->background_color = background_color;
}

/*
   Частота кадров
*/

float Document::FrameRate () const
{
  return impl->framerate;
}

void Document::SetFrameRate (float framerate)
{
  impl->framerate = framerate;
}

/*
   Ширина/высота кадра
*/

size_t Document::Width () const
{
  return impl->width;
}

size_t Document::Height () const
{
  return impl->height;
}

void Document::SetWidth (size_t width)
{
  impl->width = width;
}

void Document::SetHeight (size_t height)
{
  impl->height = height;
}

/*
   Используемые ресурсы
*/

Document::ResourceList& Document::Resources ()
{
  return const_cast<ResourceList&> (const_cast<const Document&> (*this).Resources ());
}

const Document::ResourceList& Document::Resources () const
{
  return impl->resources;
}

/*
   Элементы анимации
*/

Document::SymbolList& Document::Symbols ()
{
  return const_cast<SymbolList&> (const_cast<const Document&> (*this).Symbols ());
}

const Document::SymbolList& Document::Symbols () const
{
  return impl->symbols;
}

/*
   Анимации
*/

Document::TimelineList& Document::Timelines ()
{
  return const_cast<TimelineList&> (const_cast<const Document&> (*this).Timelines ());
}

const Document::TimelineList& Document::Timelines () const
{
  return impl->timelines;
}

/*
   Обмен
*/

void Document::Swap (Document& document)
{
  stl::swap (impl, document.impl);
}

namespace media
{

namespace adobe
{

namespace xfl
{

/*
   Обмен
*/

void swap (Document& document1, Document& document2)
{
  document1.Swap (document2);
}

}

}

}
