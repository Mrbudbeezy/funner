#include "shared.h"

using namespace render::scene::interchange;

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
typedef stl::hash_map<size_t, MapDescPtr> MapDescMap;

}

struct PropertyMapAutoWriter::Impl: public IPropertyMapWriterListener
{
  IPropertyMapWriterListener* listener;    //��������� ������� ��������������
  PropertyMapWriter           writer;      //�������������
  MapDescMap                  descs;       //����������� ���� �������
  MapDescList                 update_list; //������ ����������

/// �����������
  Impl (IPropertyMapWriterListener* in_listener)
    : listener (in_listener)
    , writer (this)
  {
  }

/// �������� ����� �������
  void Detach (size_t id)
  {
    descs.erase (id);
  }

/// ����������� �������
  void OnPropertyMapRemoved (size_t id)
  {
    Detach (id);

    if (listener)
      listener->OnPropertyMapRemoved (id);
  }

  void OnPropertyLayoutRemoved (size_t id)
  {
    if (listener)
      listener->OnPropertyLayoutRemoved (id);
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
  size_t id = properties.Id ();

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
