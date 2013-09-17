#include "shared.h"

using namespace render::scene::interchange;

//TODO: output/input stream rollback
//TODO: exception safe

namespace
{

xtl::compile_time_assert<sizeof (int) == sizeof (int32)>     assert1;
xtl::compile_time_assert<sizeof (float) == sizeof (float32)> assert2;

typedef stl::vector<size_t> IndexArray;

#pragma pack(1)

struct PropertyMapHeader
{
  uint64 id;
  bool   has_layout;
  bool   has_layout_id;
};

}

/*
===================================================================================================
    PropertyMapWriter
===================================================================================================
*/

/*
    �������� ���������� �������������� ����� �������
*/

struct PropertyMapWriter::Impl: public xtl::trackable
{
  struct LayoutDesc: public xtl::reference_counter
  {
    IndexArray string_indices; //������� ����� � �������
    size_t     id;             //�������������� �������
    size_t     hash;           //��� �������

    LayoutDesc (size_t in_id) : id (in_id), hash () {}
  };

  typedef xtl::intrusive_ptr<LayoutDesc>       LayoutDescPtr;
  typedef stl::hash_map<size_t, LayoutDescPtr> LayoutMap;

  struct MapDesc: public xtl::reference_counter
  {
    LayoutDescPtr layout; //������
    size_t        hash;   //��� �����

    MapDesc () : hash () {}
  };

  typedef xtl::intrusive_ptr<MapDesc>       MapDescPtr;
  typedef stl::hash_map<size_t, MapDescPtr> MapDescMap;

  IPropertyMapWriterListener* listener;      //��������� �������
  LayoutMap                   layouts;       //������������������ �������
  MapDescMap                  property_maps; //������������������ ����� �������

/// �����������
  Impl (IPropertyMapWriterListener* in_listener)
    : listener (in_listener)
  {
  }

/// ������ �������
/// (exception safe instead of stream writing)
  void WriteLayout (OutputStream& stream, const common::PropertyLayout& layout, LayoutDescPtr& out_desc)
  {
    try
    {
        //�������� �������

      LayoutDescPtr desc = out_desc;

      if (!desc)
      {
        layout.Trackable ().connect_tracker (xtl::bind (&Impl::RemoveLayout, this, layout.Id ()), *this);

        desc = LayoutDescPtr (new LayoutDesc (layout.Id ()), false);
      }

        //������������� �������������� �������

      write (stream, static_cast<uint64> (layout.Id ()));

        //������������� �������� �������

      const common::PropertyDesc* property = layout.Properties ();

      size_t strings_count = 0;

      write (stream, static_cast<uint32> (layout.Size ()));

      for (size_t i=0, count=layout.Size (); i<count; i++, property++)
      {
        write (stream, property->name);
        write (stream, static_cast<uint32> (property->type));
        write (stream, static_cast<uint32> (property->elements_count));
        write (stream, static_cast<uint32> (property->offset));

        if (property->type == common::PropertyType_String)
          strings_count++;
      }

        //������������� �����

      desc->string_indices.reserve (strings_count); //clear after reserve for exception safeness
      desc->string_indices.clear ();

      property = layout.Properties ();

      for (size_t i=0, count=layout.Size (); i<count; i++, property++)
      {
        if (property->type == common::PropertyType_String)
          desc->string_indices.push_back (i);
      }

        //���������� ����

      desc->hash = layout.Hash ();

        //���������� ������� ��������

      if (!out_desc)
        layouts.insert_pair (layout.Id (), desc);

      out_desc = desc;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::interchange::PropertyMapWriter::Impl::WriteLayout");
      throw;
    }
  }

/// �������� �������
  void RemoveLayout (size_t id)
  {
    layouts.erase (id);

    try
    {
      if (listener)
        listener->OnPropertyLayoutRemoved (id);
    }
    catch (...)
    {
    }
  }

/// �������� ����� �������
  void RemovePropertyMap (size_t id)
  {
    property_maps.erase (id);

    try
    {
      if (listener)
        listener->OnPropertyMapRemoved (id);
    }
    catch (...)
    {
    }    
  }
};

/*
    ����������� / ����������
*/

PropertyMapWriter::PropertyMapWriter (IPropertyMapWriterListener* listener)
  : impl (new Impl (listener))
{
}

PropertyMapWriter::~PropertyMapWriter ()
{
}

/*
    ������������� ����� �������
*/

void PropertyMapWriter::Write (OutputStream& stream, const common::PropertyMap& properties)
{
  try
  {
    bool need_send_layout_id = false;

      //����� ����� � �������

    const common::PropertyLayout& layout = properties.Layout ();

    Impl::LayoutDescPtr        layout_desc;
    Impl::MapDescPtr           map_desc;
    Impl::MapDescMap::iterator map_iter = impl->property_maps.find (properties.Id ());

    if (map_iter != impl->property_maps.end ())
    {
      map_desc    = map_iter->second;
      layout_desc = map_desc->layout;

      if (properties.Hash () == map_desc->hash)
        return;

      if (layout_desc->id != layout.Id ())
      {
        layout_desc = Impl::LayoutDescPtr ();

        Impl::LayoutMap::iterator layout_iter = impl->layouts.find (layout.Id ());

        if (layout_iter != impl->layouts.end ())
          layout_desc = layout_iter->second;
      }
    }
    else
    {
      Impl::LayoutMap::iterator layout_iter = impl->layouts.find (layout.Id ());

      if (layout_iter != impl->layouts.end ())
        layout_desc = layout_iter->second;
    }

      //������������� �������������� �����

    PropertyMapHeader header = {static_cast<uint64> (properties.Id ()), false, false};

    if (!layout_desc || layout_desc->hash != layout.Hash ())
    {
      header.has_layout = true;
    }
    else if (!map_desc || map_desc->layout != layout_desc)
    {
      header.has_layout_id = true;
    }

    stream.BeginCommand (CommandId_UpdatePropertyMap);

    try
    {
      stream.WriteData (&header, sizeof (header));

        //������������� �������

      if (header.has_layout)
      {
        impl->WriteLayout (stream, layout, layout_desc);

        if (map_desc)
          map_desc->layout = layout_desc;
      }
      
      if (header.has_layout_id)
      {
        write (stream, static_cast<uint64> (layout.Id ()));
      }

        //�������� ������� �������� �� ������ �������

      if (!map_desc)
      {
        map_desc = Impl::MapDescPtr (new Impl::MapDesc, false);

        map_desc->layout = layout_desc;

        properties.Trackable ().connect_tracker (xtl::bind (&Impl::RemovePropertyMap, &*impl, properties.Id ()), *impl);

        impl->property_maps.insert_pair (properties.Id (), map_desc);
      }

        //������������� ������ ����� �������

      write (stream, static_cast<uint32> (properties.BufferSize ()));

      stream.WriteData (properties.BufferData (), properties.BufferSize ());

        //������������� �����

      for (IndexArray::iterator iter=layout_desc->string_indices.begin (), end=layout_desc->string_indices.end (); iter!=end; ++iter)
        write (stream, properties.GetString (*iter));

        //���������� ����

      map_desc->hash = properties.Hash ();

        //�������� �������

      stream.EndCommand ();
    }
    catch (...)
    {
      stream.EndCommand (); //FIX
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::interchange::PropertyMapWriter::Write");
    throw;
  }
}

/*
===================================================================================================
    PropertyMapReader
===================================================================================================
*/

/*
    �������� ���������� ������� ����� �������
*/

struct PropertyMapReader::Impl
{
  struct LayoutDesc: public xtl::reference_counter
  {
    common::PropertyLayout layout;         //������������ �������
    IndexArray             string_indices; //������� ����� � �������
  };

  typedef xtl::intrusive_ptr<LayoutDesc>             LayoutDescPtr;
  typedef stl::hash_map<size_t, LayoutDescPtr>       LayoutMap;

  struct MapDesc: public xtl::reference_counter
  {
    common::PropertyMap properties;   //����� �������
    LayoutDescPtr       layout;       //������������ �������
    size_t              layout_hash;  //��� �������

    MapDesc (const LayoutDescPtr& desc) : properties (desc->layout), layout (desc), layout_hash (layout->layout.Hash ()) {}
  };

  typedef xtl::intrusive_ptr<MapDesc>       MapDescPtr;
  typedef stl::hash_map<size_t, MapDescPtr> MapDescMap;

  LayoutMap  layouts;       //������������ �������
  MapDescMap property_maps; //����� �������

/// ������ �������
  LayoutDescPtr ReadLayout (InputStream& stream)
  {
    try
    {
        //������ �������������� �������

      size_t layout_id = static_cast<size_t> (read (stream, xtl::type<uint64> ()));

        //��������� �������

      Impl::LayoutMap::iterator iter = layouts.find (layout_id);

      Impl::LayoutDescPtr desc;

      if (iter == layouts.end ())
      {
        desc = LayoutDescPtr (new LayoutDesc, false);
      }
      else
      {
        desc = iter->second;
      }

        //������ �������

      common::PropertyLayout& layout = desc->layout;

      layout.Clear ();
      desc->string_indices.clear ();

      size_t strings_count = 0, properties_count = read (stream, xtl::type<uint32> ());

      for (size_t i=0; i<properties_count; i++)
      {
        const char*          name           = read (stream, xtl::type<const char*> ());
        common::PropertyType type           = static_cast<common::PropertyType> (read (stream, xtl::type<uint32> ()));
        size_t               elements_count = read (stream, xtl::type<uint32> ());

        size_t property_index = layout.AddProperty (name, type, elements_count);

        const common::PropertyDesc& property = layout.Properties ()[property_index];

        size_t required_offset = read (stream, xtl::type<uint32> ());

        if (required_offset != property.offset)
          throw xtl::format_operation_exception ("", "Internal error: required offset for layout %u is %u, but actual is %u", layout_id, required_offset, property.offset);

        if (property.type == common::PropertyType_String)
          strings_count++;
      }

        //������������� �����

      desc->string_indices.reserve (strings_count);

      const common::PropertyDesc* property = layout.Properties ();

      for (size_t i=0, count=layout.Size (); i<count; i++, property++)
      {
        if (property->type == common::PropertyType_String)
          desc->string_indices.push_back (i);
      }      

        //���������� ������� � ����� ��������

      if (iter == layouts.end ())
        layouts.insert_pair (layout_id, desc);

      return desc;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::interchange::PropertyMapReader::Impl::ReadLayout");
      throw;
    }
  }
};

/*
    ����������� / ����������
*/

PropertyMapReader::PropertyMapReader ()
  : impl (new Impl)
{
}

PropertyMapReader::~PropertyMapReader ()
{
}

/*
    ��������� ����� �������
*/

common::PropertyMap PropertyMapReader::GetProperties (size_t id) const
{
  try
  {
    Impl::MapDescMap::iterator iter = impl->property_maps.find (id);

    if (iter == impl->property_maps.end ())
      throw xtl::make_argument_exception ("", "id", id, "PropertyMap with such id has not been registered");

    return iter->second->properties;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::interchange::PropertyMapReader::GetProperties");
    throw;
  }
}

/*
    ���������� �����
*/

void PropertyMapReader::Read (InputStream& stream)
{
  try
  {
    const PropertyMapHeader& header = stream.Read<PropertyMapHeader> ();

      //������������� �������

    Impl::LayoutDescPtr layout_desc;

    if (header.has_layout)
    {      
      layout_desc = impl->ReadLayout (stream);
    }
    else if (header.has_layout_id)
    {
      size_t layout_id = static_cast<size_t> (read (stream, xtl::type<uint64> ()));

      Impl::LayoutMap::iterator iter = impl->layouts.find (layout_id);

      if (iter == impl->layouts.end ())
        throw xtl::format_operation_exception ("", "Internal error: can't read property map %u because corresponding layout %u has not been registered", (size_t)header.id, layout_id);

      layout_desc = iter->second;
    }

      //������������� ����� �������

    Impl::MapDescMap::iterator map_iter = impl->property_maps.find (static_cast<size_t> (header.id));

    Impl::MapDescPtr map_desc;

    if (map_iter == impl->property_maps.end ())
    {
      if (!layout_desc)
        throw xtl::format_operation_exception ("", "Internal error: no layout for new property map %u", size_t (header.id));

        //����� ����� �������

      Impl::MapDescPtr desc (new Impl::MapDesc (layout_desc), false);

      impl->property_maps.insert_pair (static_cast<size_t> (header.id), desc);

      map_desc = desc;
    }
    else
    {
      map_desc = map_iter->second;

      if (layout_desc)
      {
          //���������� ������� ������������ �����
         
        const common::PropertyLayout& layout = layout_desc->layout;

        if (map_desc->layout != layout_desc || map_desc->layout_hash != layout.Hash ())
        {
          map_desc->properties.Reset (layout);

          map_desc->layout      = layout_desc;
          map_desc->layout_hash = layout.Hash ();
        }
      }
      else
      {
        layout_desc = map_desc->layout;
      }
    }

      //������������� ����� �������

    common::PropertyMap& properties = map_desc->properties;

      //������������� ������ ����� �������

    size_t      src_buffer_size = stream.Read<uint32> ();
    const void* src_buffer      = stream.ReadData (src_buffer_size);

    if (src_buffer_size != properties.BufferSize ())
      throw xtl::format_operation_exception ("", "Internal error: can't sync property map %u. Incoming buffer data size is %u but actual is %u", size_t (header.id), src_buffer_size, properties.BufferSize ());

    properties.SetBufferData (src_buffer);

      //������������� �����

    for (IndexArray::iterator iter=layout_desc->string_indices.begin (), end=layout_desc->string_indices.end (); iter!=end; ++iter)
    {
      const char* string = read (stream, xtl::type<const char*> ());

      properties.SetProperty (*iter, string);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::interchange::PropertyMapReader::Read");
    throw;
  }
}

/*
    �������� ����� ������� � �������
*/

void PropertyMapReader::RemoveProperties (size_t id)
{
  impl->layouts.erase (id);
}

void PropertyMapReader::RemoveLayout (size_t id)
{
  impl->property_maps.erase (id);
}
