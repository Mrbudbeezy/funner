#include "shared.h"

using namespace common;
using namespace media::geometry;

namespace
{

/*
    ���������
*/

const char* FLOAT_FORMAT = ".#####"; //���������� ������ ����� ������� ��� ������ ������������ �����

}

namespace media
{

namespace geometry
{

/*
    ���������� ��������� ������
*/

void write (OutputTextStream& stream, const VertexWeight& weight)
{
  write (stream, weight.joint_index);
  write (stream, " ");
  write (stream, weight.joint_weight, FLOAT_FORMAT);
}

void write (OutputTextStream& stream, const VertexInfluence& influence)
{
  write (stream, influence.first_weight);
  write (stream, " ");
  write (stream, influence.weights_count);
}

}

}

namespace components
{

namespace xmesh_saver
{

/*
    �����, ����������� ���-������ � Xml-�������
*/

class XmlMeshLibrarySaver
{
  private:
    typedef stl::hash_map<size_t, size_t> ResourceMap;
    
      //�������� ��������� � ������� ������
    template <class T> static xtl::iterator_range<xtl::stride_ptr<const T> > MakeRange (const VertexStream& vs, size_t offset)
    {
      xtl::stride_ptr<const T> first (reinterpret_cast<const T*> (static_cast<const char*> (vs.Data ()) + offset), vs.VertexSize ()),
                               last = first + vs.Size ();

      return xtl::make_iterator_range (first, last);
    }

      //���������� ���������� ������
    void SaveVertexStream (const VertexStream& vs)
    {
      ResourceMap::const_iterator iter = vertex_streams.find (vs.Id ());
      
      if (iter != vertex_streams.end ())
        return; //����� ��� �������
        
        //���������� ��������� ������
      
      XmlWriter::Scope scope (writer, "vertex_stream");
      
      const VertexFormat& vertex_format = vs.Format ();
        
      writer.WriteAttribute ("id", format ("vs#%u", vertex_streams.size () + 1));
      writer.WriteAttribute ("vertices_count", vs.Size ());
      writer.WriteAttribute ("vertex_size", vs.VertexSize ());

        //���������� ��������� ������
        
      for (size_t i=0; i<vertex_format.AttributesCount (); i++)
      {
        const VertexAttribute& attribute = vertex_format.Attribute (i);

        XmlWriter::Scope scope (writer, "channel");

        writer.WriteAttribute ("semantic", get_semantic_name (attribute.semantic));
        writer.WriteAttribute ("type", get_type_name (attribute.type));
        writer.WriteAttribute ("offset", attribute.offset);

          //���������� ������ ������

        switch (attribute.type)
        {
          case VertexAttributeType_Float2: writer.WriteData (MakeRange<math::vec2f> (vs, attribute.offset), FLOAT_FORMAT); break;
          case VertexAttributeType_Float3: writer.WriteData (MakeRange<math::vec3f> (vs, attribute.offset), FLOAT_FORMAT); break;
          case VertexAttributeType_Float4: writer.WriteData (MakeRange<math::vec4f> (vs, attribute.offset), FLOAT_FORMAT); break;
          case VertexAttributeType_Short2: writer.WriteData (MakeRange<math::vec2s> (vs, attribute.offset)); break;
          case VertexAttributeType_Short3: writer.WriteData (MakeRange<math::vec3s> (vs, attribute.offset)); break;
          case VertexAttributeType_Short4: writer.WriteData (MakeRange<math::vec4s> (vs, attribute.offset)); break;
          case VertexAttributeType_UByte4: writer.WriteData (MakeRange<math::vec4ub> (vs, attribute.offset)); break;
          case VertexAttributeType_Influence: writer.WriteData (MakeRange<VertexInfluence> (vs, attribute.offset)); break;
          default: break;
        }
      }

        //���������� ������ � ������ ����������
        
      vertex_streams.insert_pair (vs.Id (), vertex_streams.size () + 1);
    }
    
      //���������� ������ ��������� �����
      
    void SaveVertexWeightStream (const VertexWeightStream& vws)
    {
      if (!vws.Size ())
        return;
      
      ResourceMap::const_iterator iter = vertex_weights.find (vws.Id ());

      if (iter != vertex_weights.end ())
        return; //����� ��� �������

        //���������� ��������� ������

      XmlWriter::Scope scope (writer, "vertex_weight_stream");

      writer.WriteAttribute ("id", format ("vws#%u", vertex_weights.size () + 1));
      writer.WriteAttribute ("weights_count", vws.Size ());

        //���������� �����

      writer.WriteData (xtl::make_iterator_range (vws.Size (), vws.Data ()));

        //���������� ������ � ������ ����������

      vertex_weights.insert_pair (vws.Id (), vertex_weights.size () + 1);
    }
    
      //���������� ���������� ������
    void SaveVertexBuffer (const VertexBuffer& vb)
    {
      if (!vb.VerticesCount ())
        return;
      
      ResourceMap::const_iterator iter = vertex_buffers.find (vb.Id ());
      
      if (iter != vertex_buffers.end ())
        return; //����� ��� �������
      
        //���������� ��������� ���������� ������
        
      XmlWriter::Scope vertex_buffer_scope (writer, "vertex_buffer");
      
      writer.WriteAttribute ("id", format ("vb#%u", vertex_buffers.size () + 1));
      
      ResourceMap::const_iterator weights_iter = vertex_weights.find (vb.Weights ().Id ());
      
      if (weights_iter != vertex_weights.end ())
        writer.WriteAttribute ("weights", format ("vws#%u", weights_iter->second));      
      
        //���������� ������ �� ��������� ������
        
      XmlWriter::Scope streams_scope (writer, "streams");
      
      for (size_t i=0; i<vb.StreamsCount (); i++)
      {
        ResourceMap::const_iterator iter = vertex_streams.find (vb.Stream (i).Id ());
        
        if (iter == vertex_streams.end ())
          continue;

        writer.WriteData (format ("vs#%u", iter->second));
      }      
      
        //���������� ������ � ������ ����������
      
      vertex_buffers.insert_pair (vb.Id (), vertex_buffers.size () + 1);
    }
    
      //���������� ���������� ������
    void SaveIndexBuffer (const IndexBuffer& ib)
    {
      if (!ib.Size ())
        return;
      
      ResourceMap::const_iterator iter = index_buffers.find (ib.Id ());
      
      if (iter != index_buffers.end ())
        return; //����� ��� �������
      
        //���������� ��������� ���������� ������
        
      XmlWriter::Scope index_buffer_scope (writer, "index_buffer");
      
      writer.WriteAttribute ("id", format ("ib#%u", index_buffers.size () + 1));
      writer.WriteAttribute ("indices_count", ib.Size ());

        //���������� ��������

      writer.WriteData (xtl::make_iterator_range (ib.Size (), ib.Data ()));

        //���������� ������ � ������ ����������

      index_buffers.insert_pair (ib.Id (), index_buffers.size () + 1);        
    }
    
      //���������� ���������
    void SavePrimitive (const Primitive& primitive)
    {
      XmlWriter::Scope scope (writer, "primitive");
      
      writer.WriteAttribute ("type", get_type_name (primitive.type));      
      writer.WriteAttribute ("material", primitive.material);
      writer.WriteAttribute ("vertex_buffer", primitive.vertex_buffer);
      writer.WriteAttribute ("first", primitive.first);
      writer.WriteAttribute ("count", primitive.count);
    }
    
      //���������� ��������� ������� ����
    void SaveMeshVertexBuffers (const Mesh& mesh)
    {
      XmlWriter::Scope scope (writer, "vertex_buffers");
      
      for (size_t i=0, count=mesh.VertexBuffersCount (); i<count; i++)
      {
        ResourceMap::const_iterator vb_iter = vertex_buffers.find (mesh.VertexBuffer (i).Id ());

        if (vb_iter != vertex_buffers.end ())
          writer.WriteData (format ("vb#%u", vb_iter->second));
      }
    }
    
      //���������� ����������
    void SaveMeshPrimitives (const Mesh& mesh)
    {
      XmlWriter::Scope scope (writer, "primitives");
      
      for (size_t i=0; i<mesh.PrimitivesCount (); i++)
        SavePrimitive (mesh.Primitive (i));      
    }
    
      //���������� ����
    void SaveMesh (const char* id, const Mesh& mesh)
    {
      if (!mesh.VertexBuffersCount () || !mesh.PrimitivesCount ())
        return;

      XmlWriter::Scope scope (writer, "mesh");
      
      writer.WriteAttribute ("id", id);
      
      if (strcmp (id, mesh.Name ()))
        writer.WriteAttribute ("name", mesh.Name ());
      
      ResourceMap::const_iterator ib_iter = index_buffers.find (mesh.IndexBuffer ().Id ());
      
      if (ib_iter != index_buffers.end ())
        writer.WriteAttribute ("index_buffer", format ("ib#%u", ib_iter->second));
        
      SaveMeshVertexBuffers (mesh);
      SaveMeshPrimitives (mesh);        
    }
    
      //���������� ��������� �������
    void SaveVertexStreams ()
    {
      XmlWriter::Scope scope (writer, "vertex_streams");
      
      for (MeshLibrary::ConstIterator i=library.CreateIterator (); i; ++i)
      {
        const Mesh& mesh = *i;
        
        for (size_t k=0, count=mesh.VertexBuffersCount (); k<count; k++)
        {
          const VertexBuffer& vb = mesh.VertexBuffer (k);

          for (size_t j=0; j<vb.StreamsCount (); j++)
            SaveVertexStream (vb.Stream (j));

          SaveVertexWeightStream (vb.Weights ());          
        }
      }
    }
    
      //���������� ��������� �������
    void SaveVertexBuffers ()
    {
      XmlWriter::Scope scope (writer, "vertex_buffers");            
      
      for (MeshLibrary::ConstIterator i=library.CreateIterator (); i; ++i)
      {
        const Mesh& mesh = *i;
        
        for (size_t j=0, count=mesh.VertexBuffersCount (); j<count; j++)        
          SaveVertexBuffer (mesh.VertexBuffer (j));
      }
    }
    
      //���������� ��������� �������
    void SaveIndexBuffers ()
    {
      XmlWriter::Scope scope (writer, "index_buffers");            
      
      for (MeshLibrary::ConstIterator i=library.CreateIterator (); i; ++i)
        SaveIndexBuffer (i->IndexBuffer ());
    }
    
      //���������� �����
    void SaveMeshes ()
    {
      XmlWriter::Scope scope (writer, "meshes");      
      
      for (MeshLibrary::ConstIterator i=library.CreateIterator (); i; ++i)
        SaveMesh (library.ItemId (i), *i);
    }
    
      //���������� ����������
    void SaveLibrary ()
    {
      XmlWriter::Scope scope (writer, "mesh_library");

      SaveVertexStreams ();          
      SaveVertexBuffers ();
      SaveIndexBuffers  ();
      SaveMeshes        ();
    }
    
  public:
      //�����������
    XmlMeshLibrarySaver (const char* file_name, const MeshLibrary& in_library) : writer (file_name), library (in_library)
    {
      SaveLibrary ();
    }

  private:
    XmlWriter          writer;          //������������ Xml
    const MeshLibrary& library;         //����������� ����������
    ResourceMap        vertex_streams;  //���������� ��������� ������
    ResourceMap        vertex_buffers;  //���������� ��������� ������
    ResourceMap        vertex_weights;  //���������� ������ ��������� �����
    ResourceMap        index_buffers;   //���������� ��������� ������
};

/*
    �������������� ����������� ����������
*/

class XMeshSaverComponent
{
  public:
    XMeshSaverComponent ()
    {
      MeshLibraryManager::RegisterSaver ("xmesh", &SaveLibrary);
    }

  private:
    static void SaveLibrary (const char* file_name, const MeshLibrary& library)
    {
      XmlMeshLibrarySaver (file_name, library);
    }
};

extern "C"
{

ComponentRegistrator<XMeshSaverComponent> XMeshSaver ("media.geometry.savers.XMeshSaver");

}

}

}
