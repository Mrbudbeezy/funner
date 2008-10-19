#include "shared.h"

using namespace stl;
using namespace common;
using namespace input;

/*
   ������
*/

class EventsFilter : public ControlsDetector::IFilter
{
  public:
    EventsFilter (const char* in_action, const char* in_event_mask, const char* in_replacement) :
      action (in_action), event_mask (in_event_mask), replacement (in_replacement) {}

    const char* Action ()
    {
      return action.c_str ();
    }

    const char* EventMask ()
    {
      return event_mask.c_str ();
    }

    const char* Replacement ()
    {
      return replacement.c_str ();
    }

  private:
    string action;
    string event_mask;
    string replacement;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ControlsDetector::Impl : public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
   Impl  () {}
   ~Impl () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Add (const char* action, const char* input_event_mask, const char* replacement)
    {
      filters.insert_pair (action, EventsFilterPtr (new EventsFilter (action, input_event_mask, replacement)));
    }

    void Remove (const char* action, const char* input_event_mask)
    {
      FilterMap::iterator filter = filters.find (action);

      if (filter == filters.end ())
        return;

      for (hash_key<const char*> key (action); filter->first == key; ++filter)
      {
        if (!strcmp (input_event_mask, filter->second->EventMask ()))
          filters.erase (filter);
      }
    }

    void Remove (const Iterator& iter)
    {
      const FilterMap::iterator* erase_iterator = iter.target<FilterMap::iterator> ();

      if (!erase_iterator)
        return;

      filters.erase (*erase_iterator);
    }

    void Remove (const char* action)
    {
      filters.erase (action);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ControlsDetector::Iterator CreateIterator () const
    {
      return Iterator (filters.begin (), filters.begin (), filters.end (), FilterSelector ());
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear ()
    {
      filters.clear ();
    }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������� ��� ���������� ���� � ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::connection Detect (const EventsSource& source, const char* action, const EventHandler& handler)
    {
      DetectFunctor functor (this, action, handler);
      return source.RegisterHandler (functor);
    }

  private:
    typedef xtl::shared_ptr<EventsFilter>                         EventsFilterPtr;
    typedef hash_multimap<hash_key<const char*>, EventsFilterPtr> FilterMap;

    struct FilterSelector
    {
      IFilter& operator () (const FilterMap::value_type& value) { return *value.second; }
    };

    struct DetectFunctor
    {
      DetectFunctor (ControlsDetector::Impl* in_detector_impl, const char* in_action, const ControlsDetector::EventHandler& in_handler) :
        detector_impl (in_detector_impl), action_hash (in_action), handler (in_handler) {}

      void operator () (const char* event)
      {
        pair<FilterMap::iterator,FilterMap::iterator> filters_range = detector_impl->filters.equal_range (action_hash);

        StringArray event_components;

        split_event (event, event_components);

        for (; filters_range.first != filters_range.second; ++filters_range.first)
        {
          EventsFilter& filter = *filters_range.first->second;

          const char *event_mask  = filter.EventMask (),
                     *replacement = filter.Replacement (),
                     *action      = filter.Action ();

          StringArray event_mask_components;

          split_event (event_mask, event_mask_components);

          if (event_components.Size () != event_mask_components.Size ())
            continue;

          if (event_components.IsEmpty () && event_mask_components.IsEmpty ())
          {
            handler (action, "", replacement);
            return;
          }

          size_t matches = 0;

          for (; matches < event_components.Size (); matches++)
            if (!wcmatch (event_components [matches], event_mask_components [matches]))
              break;

          if (matches != event_components.Size ())
            continue;

          string control_mask;

          control_mask.reserve (strlen (event_components [0]) + strlen (event_mask) + 2); // 2 - ������� ������ ����� ��������

          control_mask += '\'';
          control_mask += event_components [0];
          control_mask += '\'';

          for (size_t i = 1; i < event_mask_components.Size (); i++)
          {
            control_mask += ' ';
            control_mask += event_mask_components [i];
          }

          handler (action, control_mask.c_str (), replacement);

          return;
        }
      }

      typedef xtl::intrusive_ptr<ControlsDetector::Impl> ControlsDetectorPtr;

      ControlsDetectorPtr            detector_impl;
      stl::hash_key<const char*>     action_hash;
      ControlsDetector::EventHandler handler;
    };

  private:
    FilterMap filters;
};


/*
   ������������ / ���������� / ������������
*/

ControlsDetector::ControlsDetector  ()
  : impl (new Impl)
  {}

ControlsDetector::ControlsDetector (const char* file_name)
  : impl (new Impl)
{
  Load (file_name);
}

ControlsDetector::ControlsDetector (const ControlsDetector& source)
  : impl (source.impl)
{
  addref (impl);
}

ControlsDetector::~ControlsDetector ()
{
  release (impl);
}

ControlsDetector& ControlsDetector::operator = (const ControlsDetector& source)
{
  ControlsDetector (source).Swap (*this);

  return *this;
}

/*
   ����������� ������������
*/

void ControlsDetector::Add (const char* action, const char* input_event_mask, const char* replacement)
{
  static const char* METHOD_NAME = "input::ControlsDetector::Add";

  if (!action)
    throw xtl::make_null_argument_exception (METHOD_NAME, "action");

  if (!input_event_mask)
    throw xtl::make_null_argument_exception (METHOD_NAME, "input_event_mask");

  if (!replacement)
    throw xtl::make_null_argument_exception (METHOD_NAME, "replacement");

  impl->Add (action, input_event_mask, replacement);
}

void ControlsDetector::Remove (const char* action, const char* input_event_mask)
{
  static const char* METHOD_NAME = "input::ControlsDetector::Remove(const char*,const char*)";

  if (!action)
    throw xtl::make_null_argument_exception (METHOD_NAME, "action");

  if (!input_event_mask)
    throw xtl::make_null_argument_exception (METHOD_NAME, "input_event_mask");

  impl->Remove (action, input_event_mask);
}

void ControlsDetector::Remove (const Iterator& iter)
{
  impl->Remove (iter);
}

void ControlsDetector::Remove (const char* action)
{
  if (!action)
    throw xtl::make_null_argument_exception ("input::ControlsDetector::Remove(const char*)", "action");

  impl->Remove (action);
}

/*
   �������
*/

ControlsDetector::Iterator ControlsDetector::CreateIterator () const
{
  return impl->CreateIterator ();
}

/*
   �������
*/

void ControlsDetector::Clear ()
{
  impl->Clear ();
}
    
/*
   ����������� ������� ��� ���������� ���� � ������� ����������
*/

xtl::connection ControlsDetector::Detect (const EventsSource& source, const char* action, const EventHandler& handler)
{
  if (!action)
    throw xtl::make_null_argument_exception ("input::ControlsDetector::Detect", "action");

  return impl->Detect (source, action, handler);
}

/*
   ���������� / ��������
*/

void ControlsDetector::Load (const char* file_name)
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("input::ControlsDetector::Load", "file_name");

  static ComponentLoader loader ("input.loaders.*");

  ControlsDetectorManager::GetLoader (file_name, SerializerFindMode_ByName) (file_name, *this);
}

void ControlsDetector::Save (const char* file_name)
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("input::ControlsDetector::Save", "file_name");

  static ComponentLoader loader ("input.savers.*");

  ControlsDetectorManager::GetSaver (file_name, SerializerFindMode_ByName) (file_name, *this);
}

/*
   �����
*/

void ControlsDetector::Swap (ControlsDetector& source)
{
  stl::swap (impl, source.impl);
}

namespace input
{

/*
   �����
*/
void swap (ControlsDetector& source1, ControlsDetector& source2)
{
  source1.Swap (source2);
}

}
