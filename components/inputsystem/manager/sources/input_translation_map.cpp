#include "shared.h"

using namespace common;
using namespace input;
using namespace stl;

/*
    ���������� ������� ���������� ������ �������������� ��������� ����� � ���������� �������
*/

struct TranslationMap::Impl : public xtl::reference_counter
{
  public:
    /*
        ����������� ������������    
          (������ ���������� � ���������� ����������� ����� {1}, {2}, ...)        
    */

    void Add (const char* input_event, const char* client_event_replacement, const char* tag)
    {     
      static const char* METHOD_NAME = "input::TranslationMap::Impl::Add"; 

      if (!input_event)
        RaiseNullArgument (METHOD_NAME, "input_event");

      if (!client_event_replacement)
        RaiseNullArgument (METHOD_NAME, "client_event_replacement");

      EventTranslatorPtr         replacer (new EventTranslator (input_event, client_event_replacement, tag));
      stl::hash_key<const char*> input_event_hash (word (input_event, 0).c_str ());

      if (!*tag)
      {
          //���������� ���������� ��� ����

        replacer_map.insert_pair (input_event_hash, replacer);

        return;
      }

        //�������� ����� ����������

      ReplacerMap::iterator iter = replacer_map.insert_pair (input_event_hash, replacer);

        //����������� ���������� �� ����

      try
      {
        tags.insert_pair (tag, iter);
      }
      catch (...)
      {
        replacer_map.erase (iter);
        throw;
      }
    }
    
    /*
        �������� �����������
    */

    void Remove (const char* tag)
    {
      TagMap::iterator iter = tags.find (tag);

      if (iter == tags.end ())
        return;

      replacer_map.erase (iter->second);
      tags.erase (iter);
    }

    void Remove (const Iterator& in_iter)
    {
      const ReplacerMap::iterator* iter = in_iter.target<ReplacerMap::iterator> ();

      if (!iter)
        return;

      stl::hash_key<const char*> tag_hash = (*iter)->first;

      tags.erase (tag_hash);
      replacer_map.erase (*iter);
    }
    
    /*
        �������    
    */

    void Clear ()
    {
      replacer_map.clear ();
      tags.clear ();
      handler.clear ();
    }
    
    /*
        ����� �����������
    */
    
    Iterator Find (const char* tag)
    {
      if (!*tag)
        return CreateIterator (replacer_map.end ());

      TagMap::iterator iter = tags.find (tag);

      if (iter == tags.end ())
        return CreateIterator (replacer_map.end ());

      return CreateIterator (iter->second);
    }
    
    /*
        �������� ���������
    */    
    
    Iterator CreateIterator ()
    {
      return CreateIterator (replacer_map.begin ());
    }

    /*
        ��������� ����������� ����������� ����������������� �������    
    */

    void SetHandler (const EventHandler& in_handler) { handler = in_handler; }

    const EventHandler& Handler () const { return handler; }

    /*
        ��������� �������
    */

    void ProcessEvent (const char* event) const
    {
      if (!handler)
        return;
      
      vector<string> event_components;
      
      split_event (event, event_components);

      if (event_components.empty ())
        return;

      pair <ReplacerMap::const_iterator, ReplacerMap::const_iterator> range_pair = replacer_map.equal_range (event_components[0].c_str ());

      if (range_pair.first == replacer_map.end ())
        return;

      string replacement_buffer;

      for (ReplacerMap::const_iterator iter = range_pair.first, end = range_pair.second; iter != end; ++iter)
      {
        EventTranslator& replacer = *iter->second;

        if (replacer.Replace (event_components, replacement_buffer))
        {
          handler (replacement_buffer.c_str ());
          return;
        }
      }
    }
    
  private:
    typedef xtl::shared_ptr<EventTranslator>                                   EventTranslatorPtr;
    typedef stl::hash_multimap<stl::hash_key<const char*>, EventTranslatorPtr> ReplacerMap;
    typedef stl::hash_map<stl::hash_key<const char*>, ReplacerMap::iterator>   TagMap;
    
  private:
    struct translator_selector
    {
      TranslationMap::Translator& operator () (ReplacerMap::value_type& value) const { return *value.second; }
    };

      //�������� ���������    
    Iterator CreateIterator (ReplacerMap::iterator iter)
    {
      return Iterator (iter, replacer_map.begin (), replacer_map.end (), translator_selector ());
    }

  private:
    ReplacerMap  replacer_map;
    TagMap       tags;
    EventHandler handler;
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
  if (!file_name)
    RaiseNullArgument ("input::TranslationMap::TranslationMap", "file_name");

  TranslationMapManagerSingleton::Instance ().Load (file_name, *this);
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
*/

void TranslationMap::Add (const char* input_event, const char* client_event_replacement, const char* tag)
{
  impl->Add (input_event, client_event_replacement, tag);
}

void TranslationMap::Remove (const char* tag)
{
  impl->Remove (tag);
}

void TranslationMap::Remove (const Iterator& iter)
{
  impl->Remove (iter);
}

/*
    �������
*/

void TranslationMap::Clear ()
{
  impl->Clear ();
}
    
/*
    �������� ���������
*/

TranslationMap::Iterator TranslationMap::CreateIterator () const
{
  return impl->CreateIterator ();
}

/*
    ����� �� ����
*/

TranslationMap::Iterator TranslationMap::Find (const char* tag) const
{
  return impl->Find (tag);
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
    return;

  impl->ProcessEvent (event);
}

/*
   �������� / ���������� �������
*/

void TranslationMap::Load (const char* file_name)
{
  TranslationMap (file_name).Swap (*this);
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
