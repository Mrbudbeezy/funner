#include <stl/hash_map>

#include <xtl/reference_counter.h>
#include <xtl/function.h>
#include <xtl/shared_ptr.h>

#include <common/exception.h>
#include <common/strlib.h>

#include <input/translation_map.h>

#include "shared.h"

using namespace common;
using namespace input;
using namespace stl;

namespace
{

void default_event_handler (const char*)
{
}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� ���������� ������ �������������� ��������� ����� � ���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct TranslationMap::Impl : public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Impl  () : current_replacer_id (0), handler (&default_event_handler) {}
    ~Impl () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������������
///  (������ ���������� � ���������� ����������� ����� {1}, {2}, ...)
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Add (const char* input_event, const char* client_event_replacement)
    {     
      replacer_map.insert_pair (word (input_event, 0).c_str (), EventReplacerPtr (new EventReplacer (input_event, client_event_replacement, current_replacer_id)));
      return current_replacer_id++;
    }

    void Remove (size_t input_event)
    {
      for (ReplacerMap::iterator iter = replacer_map.begin (), end = replacer_map.end (); iter != end; ++iter)
      {
        if (iter->second->Id () == input_event)
        {
          replacer_map.erase (iter);
          return;
        }
      }
    }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������� ����������� ����������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetHandler (const EventHandler& in_handler)
    {
      handler = in_handler;
    }

    const EventHandler& Handler () const
    {
      return handler;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ProcessEvent (const char* event) const
    {
      vector<string> event_components = split (event, " ");

      if (event_components.empty ())
        return;

      pair <ReplacerMap::const_iterator, ReplacerMap::const_iterator> range_pair = replacer_map.equal_range (event_components[0].c_str ());

      if (range_pair.first == replacer_map.end ())
        return;

      string replacement_buffer;

      for (ReplacerMap::const_iterator iter = range_pair.first, end = range_pair.second; iter != end; ++iter)
      {
        EventReplacer& replacer = *iter->second;

        if (replacer.Replace (event_components, replacement_buffer))
        {
          handler (replacement_buffer.c_str ());
          return;
        }
      }
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear ()
    {
      replacer_map.clear ();
    }

  private:
    typedef xtl::shared_ptr<EventReplacer>                                   EventReplacerPtr;
    typedef stl::hash_multimap<stl::hash_key<const char*>, EventReplacerPtr> ReplacerMap;

  private:
    ReplacerMap  replacer_map;
    EventHandler handler;
    size_t       current_replacer_id;
};

/*
   ������������� / ���������� / ������������
*/

TranslationMap::TranslationMap ()
  : impl (new Impl)
  {}

TranslationMap::TranslationMap (const char* file_name)
  : impl (new Impl)
{
  Load (file_name);
}

TranslationMap::TranslationMap (const TranslationMap& source)
  : impl (source.impl)
{
  addref (impl);
}

TranslationMap::~TranslationMap ()
{
  release (impl);
}

TranslationMap& TranslationMap::operator = (const TranslationMap& source)
{
  TranslationMap (source).Swap (*this);

  return *this;
}

/*
   ����������� ������������
     (������ ���������� � ���������� ����������� ����� #1, #2, ...)
*/

size_t TranslationMap::Add (const char* input_event, const char* client_event_replacement)
{
  if (!input_event)
    RaiseNullArgument ("input::TranslationMap::Add", "input_event");

  if (!client_event_replacement)
    RaiseNullArgument ("input::TranslationMap::Add", "client_event_replacement");

  return impl->Add (input_event, client_event_replacement);
}

void TranslationMap::Remove (size_t input_event)
{
  impl->Remove (input_event);
}
    
/*
   ��������� ����������� ����������� ����������������� �������
*/

void TranslationMap::SetHandler (const TranslationMap::EventHandler& handler)
{
  impl->SetHandler (handler);
}

const TranslationMap::EventHandler& TranslationMap::Handler () const
{
  return impl->Handler ();
}

/*
   ��������� �������
*/

void TranslationMap::ProcessEvent (const char* event) const
{
  if (!event)
    RaiseNullArgument ("input::TranslationMap::ProcessEvent", "event");

  impl->ProcessEvent (event);
}

/*
   �������
*/

void TranslationMap::Clear ()
{
  impl->Clear ();
}

/*
   �������� / ���������� �������
*/

void TranslationMap::Load (const char* file_name)
{
  if (!file_name)
    RaiseNullArgument ("input::TranslationMap::Load", "file_name");

  TranslationMapManagerSingleton::Instance ().Load (file_name, *this);
}

void TranslationMap::Save (const char* file_name)
{
  if (!file_name)
    RaiseNullArgument ("input::TranslationMap::Save", "file_name");

  TranslationMapManagerSingleton::Instance ().Save (file_name, *this);
}

/*
   �����
*/

void TranslationMap::Swap (TranslationMap& source)
{
  stl::swap (impl, source.impl);
}

namespace input
{

/*
   �����
*/

void swap (TranslationMap& source1, TranslationMap& source2)
{
  source1.Swap (source2);
}

}
