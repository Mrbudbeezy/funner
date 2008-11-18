#include "shared.h"

using namespace media::rms;
using namespace common;

/*
    �������� ���������� ������ ��������
*/

typedef stl::hash_set<stl::hash_key<const char*> > ResourceSet;
typedef xtl::signal<void (const char*)>            EventSignal;

struct ResourceGroup::Impl: public xtl::reference_counter
{
  ResourceSet references;                       //����� ��������
  StringArray resources;                        //����� ��������
  EventSignal signals [ResourceGroupEvent_Num]; //�������
  
  ~Impl ()
  {
    Clear ();
  }
  
  void Clear ()
  {
      //���������� �� �������� ���� ��������
      
    for (size_t i=0, count=resources.Size (); i<count; i++)
    {
      try
      {
        signals [ResourceGroupEvent_OnRemove] (resources [i]);
      }
      catch (...)
      {
        //���������� ���� ����������
      }
    }
    
      //�������
      
    references.clear ();
    resources.Clear ();    
  }  
};

/*
    ������������ / ���������� / ������������
*/

ResourceGroup::ResourceGroup ()
  : impl (new Impl)
{
}

ResourceGroup::ResourceGroup (const ResourceGroup& group)
  : impl (group.impl)
{
  addref (impl);
}

ResourceGroup::~ResourceGroup ()
{
  release (impl);
}

ResourceGroup& ResourceGroup::operator = (const ResourceGroup& group)
{
  ResourceGroup (group).Swap (*this);

  return *this;  
}

/*
    ���������� � ������
*/

//����� �� ������
bool ResourceGroup::IsEmpty () const
{
  return impl->resources.IsEmpty ();
}

//���������� ��������� � ������
size_t ResourceGroup::Size () const
{
  return impl->resources.Size ();
}

/*
    ������� ��������
*/

const char* ResourceGroup::Item (size_t index) const
{
  try
  {
    return impl->resources [index];
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rms::ResourceGroup::Item");
    throw;
  }
}

/*
    ���������� / �������� ��������
*/

void ResourceGroup::Add (const char* name)
{
  try
  {
      //�������� ������������ ����������
    
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

      //��������: �������� �� ��� ������

    if (impl->references.find (name) != impl->references.end ())
      return;

    size_t resource_index = impl->resources.Add (name);

    try
    {
      impl->references.insert (name);
    }
    catch (...)
    {
      impl->resources.Remove (resource_index);
      throw;
    }

    try
    {
      impl->signals [ResourceGroupEvent_OnAdd] (name);
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rfx::ResourceGroup::Add(const char*)");
    throw;
  }
}

void ResourceGroup::Add (const ResourceGroup& group)
{
  try
  {
    if (impl == group.impl) //������ �� ���������� ������� � ������ ����
      return;
      
    const StringArray& array = group.impl->resources;

    for (size_t i=0, count=array.Size (); i<count; i++)    
      Add (array [i]);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rfx::ResourceGroup::Add(const ResourceGroup&)");
    throw;
  }
}

void ResourceGroup::Remove (const char* name)
{
    //�������� ������������ ����������

  if (!name)
    return;
    
  ResourceSet::iterator iter = impl->references.find (name);

  if (iter == impl->references.end ())
    return;

  impl->references.erase (iter);
  
  StringArray& array = impl->resources;

  for (size_t i=0, count=array.Size (); i<count; i++)
    if (!strcmp (array [i], name))
    {
      array.Remove (i);
      break;
    }
    
  try
  {
    impl->signals [ResourceGroupEvent_OnRemove] (name);
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

void ResourceGroup::Remove (const ResourceGroup& group)
{
  try
  {
    if (impl == group.impl) //������ �� ���������� ������� � ������ ����
    {
      Clear ();
      return;
    }

    const StringArray& array = group.impl->resources;

    for (size_t i=0, count=array.Size (); i<count; i++)    
      Remove (array [i]);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rfx::ResourceGroup::Remove(const ResourceGroup&)");
    throw;
  }
}

void ResourceGroup::Clear ()
{
  impl->Clear ();
}

/*
    �������� �� �������
*/

xtl::connection ResourceGroup::RegisterEventHandler (ResourceGroupEvent event, const EventHandler& handler)
{
  if (event < 0 || event >= ResourceGroupEvent_Num)
    throw xtl::make_argument_exception ("media::rms::ResourceGroup::RegisterEventHandler", "event", event);

  return impl->signals [event].connect (handler);
}

/*
    �����
*/

void ResourceGroup::Swap (ResourceGroup& group)
{
  stl::swap (impl, group.impl);
}

namespace media
{

namespace rms
{

void swap (ResourceGroup& group1, ResourceGroup& group2)
{
  group1.Swap (group2);
}

}

}
