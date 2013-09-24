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

namespace
{

struct MapDesc;

/// ������ ����������
struct MapDescList
{
  MapDesc* first;
  MapDesc* last; 

  MapDescList () : first (), last () {}
};

/// ���������� ����� �������
struct MapDesc: public xtl::reference_counter
{
  const common::PropertyMap& properties;
  bool                       dirty;              //����� �� �������������
  xtl::auto_connection       update_connection;  //���������� � �������� ���������� �� ���������� ����� �������
  MapDescList&               list;               //������ ����������
  MapDesc*                   next;               //��������� ������� � ������ ����������

/// �����������
  MapDesc (const common::PropertyMap& in_properties, MapDescList& in_list)
    : properties (in_properties)
    , dirty (true)
    , update_connection (properties.RegisterEventHandler (common::PropertyMapEvent_OnUpdate, xtl::bind (&MapDesc::OnUpdate, this)))
    , list (in_list)
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
    next = 0;

    if (list.last)
    {
      list.last->next = this;
      return;
    }

    list.last = list.first = this;
  }
};

typedef xtl::intrusive_ptr<MapDesc>       MapDescPtr;
typedef stl::hash_map<uint64, MapDescPtr> MapDescMap;
typedef stl::vector<uint64>               IdArray;

}

struct PropertyMapAutoWriter::Impl: public IPropertyMapWriterListener
{
  IPropertyMapWriterListener* listener;           //��������� ������� ��������������
  PropertyMapWriter           writer;             //�������������
  MapDescMap                  descs;              //����������� ���� �������
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

/// �������� ����� �������
  void Detach (uint64 id)
  {
    descs.erase (id);
  }

/// ����������� �������
  void OnPropertyMapRemoved (uint64 id)
  {
    try
    {
      Detach (id);

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

/*
    ����������� / ����������
*/

PropertyMapAutoWriter::PropertyMapAutoWriter (IPropertyMapWriterListener* listener)
  : impl (new Impl (listener))
{
}

PropertyMapAutoWriter::~PropertyMapAutoWriter ()
{
}

/*
    �������� �� ������ �������
*/

void PropertyMapAutoWriter::Attach (const common::PropertyMap& properties)
{
  uint64 id = properties.Id ();

  MapDescMap::iterator iter = impl->descs.find (id);

  if (iter != impl->descs.end ())
    return;

  MapDescPtr desc (new MapDesc (properties, impl->update_list), false);

  impl->descs.insert_pair (id, desc);
}

void PropertyMapAutoWriter::Detach (const common::PropertyMap& properties)
{
  impl->Detach (properties.Id ());
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
