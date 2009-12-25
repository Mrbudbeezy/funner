#include "shared.h"

using namespace media::adobe::xfl;

typedef media::CollectionImpl<Resource, ICollection<Resource> > ResourceCollection;
typedef media::CollectionImpl<Symbol,   ICollection<Symbol> >   SymbolCollection;
typedef media::CollectionImpl<Timeline, ICollection<Timeline> > TimelineCollection;

namespace
{

const char*  XFL_LOADERS_MASK   = "media.adobe.xfl.loaders.*"; //����� ��� ����������� �������� xfl-����������
const float  DEFAULT_FRAME_RATE = 25.0f;                       //���������� ������ � ������� �� ���������
const size_t DEFAULT_WIDTH      = 1;                           //������ �� ���������
const size_t DEFAULT_HEIGHT     = 1;                           //������ �� ���������

}

/*
    �������� ���������� XFL ���������
*/

struct Document::Impl : public xtl::reference_counter
{
  stl::string        name;             //��� ���������
  math::vec3f        background_color; //���� ����
  float              framerate;        //������� ������
  size_t             width;            //������
  size_t             height;           //������
  ResourceCollection resources;        //�������
  SymbolCollection   symbols;          //�������� ��������
  TimelineCollection timelines;        //��������
  
  Impl ()
    : framerate (DEFAULT_FRAME_RATE)
    , width (DEFAULT_WIDTH)
    , height (DEFAULT_HEIGHT)
  {
  }
};

/*
    ������������ / ���������� / ������������
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
   ���
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
   ���� ����
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
   ������� ������
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
   ������/������ �����
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
   ������������ �������
*/

Document::ResourceList& Document::Resources ()
{
  return const_cast<ResourceList&> (const_cast<const Document&> (*this).Resources ());
}

const Document::ResourceList& Document::Resources () const
{
  return impl->resources;
}

Resource* Document::FindResource (const char* resource_name)
{
  return const_cast<Resource*> (const_cast<const Document&> (*this).FindResource (resource_name));
}

const Resource* Document::FindResource (const char* resource_name) const
{
  if (!resource_name)
    return 0;

  for (ResourceCollection::ConstIterator iter = impl->resources.CreateIterator (); iter; ++iter)
    if (!xtl::xstrcmp (resource_name, iter->Name ()))
      return &(*iter);

  return 0;
}

/*
   �������� ��������
*/

Document::SymbolList& Document::Symbols ()
{
  return const_cast<SymbolList&> (const_cast<const Document&> (*this).Symbols ());
}

const Document::SymbolList& Document::Symbols () const
{
  return impl->symbols;
}

Symbol* Document::FindSymbol (const char* symbol_name)
{
  return const_cast<Symbol*> (const_cast<const Document&> (*this).FindSymbol (symbol_name));
}

const Symbol* Document::FindSymbol (const char* symbol_name) const
{
  if (!symbol_name)
    return 0;

  for (SymbolCollection::ConstIterator iter = impl->symbols.CreateIterator (); iter; ++iter)
    if (!xtl::xstrcmp (symbol_name, iter->Name ()))
      return &(*iter);

  return 0;
}

/*
   ��������
*/

Document::TimelineList& Document::Timelines ()
{
  return const_cast<TimelineList&> (const_cast<const Document&> (*this).Timelines ());
}

const Document::TimelineList& Document::Timelines () const
{
  return impl->timelines;
}

Timeline* Document::FindTimeline (const char* timeline_name)
{
  return const_cast<Timeline*> (const_cast<const Document&> (*this).FindTimeline (timeline_name));
}

const Timeline* Document::FindTimeline (const char* timeline_name) const
{
  if (!timeline_name)
    return 0;

  for (TimelineCollection::ConstIterator iter = impl->timelines.CreateIterator (); iter; ++iter)
    if (!xtl::xstrcmp (timeline_name, iter->Name ()))
      return &(*iter);

  return 0;
}

/*
   �����
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
   �����
*/

void swap (Document& document1, Document& document2)
{
  document1.Swap (document2);
}

}

}

}
