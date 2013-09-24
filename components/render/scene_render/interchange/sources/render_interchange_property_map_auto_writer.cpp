#include "shared.h"

using namespace render::scene::interchange;

/*
    ���������
*/

namespace
{

const char*  LOG_NAME                        = "render.scene.interchange"; //��� ������ ����������� ����������������
const size_t MAP_REMOVE_LIST_RESERVE_SIZE    = 1024;                       //������������� ���������� ��������� � ������ ��������
const size_t LAYOUT_REMOVE_LIST_RESERVE_SIZE = 128;                        //������������� ���������� ��������� � ������ ��������

}

/*
    �������� ���������� ��������������� �������������� ����� �������
*/

/// ������ ����������
struct PropertyMapAutoWriter::MapDescList
{
  MapDesc* first;
  MapDesc* last; 

  MapDescList () : first (), last () {}
};

typedef stl::vector<uint64> IdArray;

struct PropertyMapAutoWriter::Impl: public IPropertyMapWriterListener, public xtl::reference_counter
{
  IPropertyMapWriterListener* listener;           //��������� ������� ��������������
  PropertyMapWriter           writer;             //�������������
  MapDescList                 update_list;        //������ ����������
  IdArray                     map_remove_list;    //������ ��������������� �� ��������
  IdArray                     layout_remove_list; //������ ��������������� �� ��������
  common::Log                 log;                //����� ����������� ����������������

/// �����������
  Impl (IPropertyMapWriterListener* in_listener)
    : listener (in_listener)
    , writer (this)
    , log (LOG_NAME)
  {
    map_remove_list.reserve (MAP_REMOVE_LIST_RESERVE_SIZE);
    layout_remove_list.reserve (LAYOUT_REMOVE_LIST_RESERVE_SIZE);
  }

/// ����������� �������
  void OnPropertyMapRemoved (uint64 id)
  {
    try
    {
      map_remove_list.push_back (id);

      if (listener)
        listener->OnPropertyMapRemoved (id);
    }
    catch (std::exception& e)
    {
      log.Printf ("%s\n    at render::scene::interchange::PropertyMapAutoWriter::Impl::OnPropertyMapRemoved", e.what ());
    }
    catch (...)
    {
      log.Printf ("unknown exception\n    at render::scene::interchange::PropertyMapAutoWriter::Impl::OnPropertyMapRemoved");
    }
  }

  void OnPropertyLayoutRemoved (uint64 id)
  {
    try
    {
      layout_remove_list.push_back (id);

      if (listener)
        listener->OnPropertyLayoutRemoved (id);
    }
    catch (std::exception& e)
    {
      log.Printf ("%s\n    at render::scene::interchange::PropertyMapAutoWriter::Impl::OnPropertyLayoutRemoved", e.what ());
    }
    catch (...)
    {
      log.Printf ("unknown exception\n    at render::scene::interchange::PropertyMapAutoWriter::Impl::OnPropertyLayoutRemoved");
    }
  }

};

/// ���������� ����� �������
struct PropertyMapAutoWriter::MapDesc: public xtl::reference_counter
{
  const common::PropertyMap& properties;
  bool                       dirty;              //����� �� �������������
  xtl::auto_connection       update_connection;  //���������� � �������� ���������� �� ���������� ����� �������
  xtl::intrusive_ptr<Impl>   impl;               //���������� ��������������
  MapDesc*                   next;               //��������� ������� � ������ ����������

/// �����������
  MapDesc (const common::PropertyMap& in_properties, Impl* in_impl)
    : properties (in_properties)
    , dirty (true)
    , update_connection (properties.RegisterEventHandler (common::PropertyMapEvent_OnUpdate, xtl::bind (&MapDesc::OnUpdate, this)))
    , impl (in_impl)
    , next ()
  {
    AddToList ();
  }

/// ���������� �� ����������
  void OnUpdate ()
  {
    if (dirty)
      return;

    dirty = true;

    AddToList ();
  }

/// ������ ����� ����������
  void ClearDirtyFlag ()
  {
    dirty = false;
  }

/// ���������� � ������
  void AddToList ()
  {
    MapDescList& list = impl->update_list;

    next = 0;

    if (list.last)
    {
      list.last->next = this;
      return;
    }

    list.last = list.first = this;
  }
};

/*
    ����������� / ����������
*/

PropertyMapAutoWriter::PropertyMapAutoWriter (IPropertyMapWriterListener* listener)
  : impl (new Impl (listener))
{
}

PropertyMapAutoWriter::~PropertyMapAutoWriter ()
{
  release (impl);
}

/*
    �������� �� ������ �������
*/

PropertyMapAutoWriter::Synchronizer PropertyMapAutoWriter::CreateSynchronizer (const common::PropertyMap& properties)
{
  try
  {
    return Synchronizer (new MapDesc (properties, impl));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::interchange::PropertyMapAutoWriter::CreateSynchronizer");
    throw;
  }
}

/*
    ������ ����� ������� (� ������)
*/

void PropertyMapAutoWriter::Write (OutputStream& stream, const common::PropertyMap& properties)
{
  try
  {
    impl->writer.Write (stream, properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::interchange::PropertyMapAutoWriter::Write(OutputStream&,const common::PropertyMap&)");
    throw;
  }
}

/*
    �������������
*/

void PropertyMapAutoWriter::Write (OutputStream& stream)
{
  try
  {
      //��������� ������ ��������

    for (IdArray::iterator iter=impl->layout_remove_list.begin (), end=impl->layout_remove_list.end (); iter!=end; ++iter)
    {
      size_t saved_position = stream.Position ();

      try
      {
        stream.BeginCommand (CommandId_RemovePropertyLayout);
        write               (stream, static_cast<uint64> (*iter));
        stream.EndCommand   ();
      }
      catch (...)
      {
        stream.SetPosition (saved_position);

        impl->layout_remove_list.erase (impl->layout_remove_list.begin (), iter);

        throw;
      }
    }

    impl->layout_remove_list.clear ();

    for (IdArray::iterator iter=impl->map_remove_list.begin (), end=impl->map_remove_list.end (); iter!=end; ++iter)
    {
      size_t saved_position = stream.Position ();

      try
      {
        stream.BeginCommand (CommandId_RemovePropertyMap);
        write               (stream, static_cast<uint64> (*iter));
        stream.EndCommand   ();
      }
      catch (...)
      {
        stream.SetPosition (saved_position);

        impl->map_remove_list.erase (impl->map_remove_list.begin (), iter);

        throw;
      }
    }

    impl->map_remove_list.clear ();

      //��������� ������ ����������

    for (MapDesc* desc = impl->update_list.first; desc; desc = desc->next)
    {
      try
      {
          //������ ����������

        impl->writer.Write (stream, desc->properties);

          //������� ����� ����������

        desc->ClearDirtyFlag ();
      }
      catch (...)
      {
        impl->update_list.first = desc->next;

        desc->ClearDirtyFlag ();

        throw;
      }
    }

      //������� ������ ����������

    impl->update_list.first = impl->update_list.last = 0;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::interchange::PropertyMapAutoWriter::Write(OutputStream&)");
    throw;
  }  
}

/*
    Synchronizer
*/

PropertyMapAutoWriter::Synchronizer::Synchronizer (MapDesc* in_impl)
  : impl (in_impl)
{
}

PropertyMapAutoWriter::Synchronizer::Synchronizer (const Synchronizer& s)
  : impl (s.impl)
{
  addref (impl);
}

PropertyMapAutoWriter::Synchronizer::~Synchronizer ()
{
  release (impl);
}

PropertyMapAutoWriter::Synchronizer& PropertyMapAutoWriter::Synchronizer::operator = (const Synchronizer& s)
{
  Synchronizer tmp (s);

  stl::swap (impl, tmp.impl);

  return *this;
}
