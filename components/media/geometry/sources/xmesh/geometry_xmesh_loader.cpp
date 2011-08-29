#include "shared.h"

using namespace media;
using namespace media::geometry;
using namespace common;

namespace
{

/*
    ���������
*/

const char* LOG_NAME = "media.geometry.XmlMeshLibraryLoader"; //��� ������ ����������������

}

/*
    ������ ��������� ������
*/

namespace media
{

namespace geometry
{

template <class Token, class BaseIter>
inline bool read (xtl::io::token_iterator<Token, BaseIter>& iter, VertexInfluence& influence)
{
  if (iter.available () < 2)
    return false;

  xtl::io::token_iterator<Token, BaseIter> save = iter;
  
  if (!read (iter, influence.first_weight) || !read (iter, influence.weights_count))
  {
    iter = save;
    return false;
  }

  return true;
}

template <class Token, class BaseIter>
inline bool read (xtl::io::token_iterator<Token, BaseIter>& iter, VertexWeight& weight)
{
  if (iter.available () < 2)
    return false;

  xtl::io::token_iterator<Token, BaseIter> save = iter;
  
  if (!read (iter, weight.joint_index) || !read (iter, weight.joint_weight))
  {
    iter = save;
    return false;
  }

  return true;
}

}

}

namespace components
{

namespace xmesh_loader
{

/*
    ��������� ���-������� � ������� Xml
*/

class XmlMeshLibraryLoader
{
  private:
      //������ ���������� �������
    void ParseVertexFormatChannel (Parser::Iterator channel_iter, VertexFormat& vertex_format)
    {
      static const char* METHOD_NAME = "media::geometry::XmlMeshLibraryLoader::ParseVertexFormatChannel";

      const char              *semantic_string = get<const char*> (*channel_iter, "semantic"),
                              *type_string     = get<const char*> (*channel_iter, "type");
      VertexAttributeSemantic semantic         = get_vertex_attribute_semantic (semantic_string);
      VertexAttributeType     type             = get_vertex_attribute_type (type_string);
      size_t                  offset           = get<size_t> (*channel_iter, "offset");

      if (semantic == VertexAttributeSemantic_Num)
        throw xtl::make_argument_exception (METHOD_NAME, "semantic", semantic_string);

      if (type == VertexAttributeType_Num)
        throw xtl::make_argument_exception (METHOD_NAME, "type", type_string);

      vertex_format.AddAttribute (semantic, type, offset);
    }
    
      //��������� ��������� �� ������ ���������� ������
    template <class T> static xtl::stride_ptr<T> MakeVertexAttributeIterator (VertexStream& vs, size_t offset)
    {
      return xtl::stride_ptr<T> (reinterpret_cast<T*> (static_cast<char*> (vs.Data ()) + offset), vs.VertexSize ());
    }
    
      //������ ���������� ������
    void ParseVertexStream (Parser::Iterator vs_iter)
    {     
        //������ �������������� ������
        
      const char* id = get<const char*> (*vs_iter, "id");

        //����������� ����� ������ � ������� �������
      
      size_t vertices_count = get<size_t> (*vs_iter, "vertices_count"),
             vertex_size    = get<size_t> (*vs_iter, "vertex_size");

      VertexFormat vertex_format;

        //������ ���������� �������

      for_each_child (*vs_iter, "channel", xtl::bind (&XmlMeshLibraryLoader::ParseVertexFormatChannel, this, _1, xtl::ref (vertex_format)));
      
        //�������� ���������� ������        
      
      VertexStream vs (vertices_count, VertexDeclaration (vertex_format, vertex_size));
      
        //������ ������
        
      Parser::NamesakeIterator channel_iter = vs_iter->First ("channel");
        
      for (size_t i=0, channel_count=vertex_format.AttributesCount (); i<channel_count; i++, ++channel_iter)
      {
        const VertexAttribute& attribute = vertex_format.Attribute (i);

        for (;channel_iter && get<size_t> (*channel_iter, "offset", (size_t)-1) != attribute.offset; ++channel_iter);

        if (!channel_iter)
          return;

        switch (attribute.type)
        {
          case VertexAttributeType_Float2:
            read (*channel_iter, "#text", MakeVertexAttributeIterator<math::vec2f> (vs, attribute.offset), vertices_count);
            break;
          case VertexAttributeType_Float3:
            read (*channel_iter, "#text", MakeVertexAttributeIterator<math::vec3f> (vs, attribute.offset), vertices_count);
            break;                      
          case VertexAttributeType_Float4:
            read (*channel_iter,"#text", MakeVertexAttributeIterator<math::vec4f> (vs, attribute.offset), vertices_count);
            break;
          case VertexAttributeType_Short2:
            read (*channel_iter, "#text", MakeVertexAttributeIterator<math::vec2s> (vs, attribute.offset), vertices_count);
            break;
          case VertexAttributeType_Short3:
            read (*channel_iter, "#text", MakeVertexAttributeIterator<math::vec3s> (vs, attribute.offset), vertices_count);
            break;
          case VertexAttributeType_Short4:
            read (*channel_iter,"#text", MakeVertexAttributeIterator<math::vec4s> (vs, attribute.offset), vertices_count);
            break;
          case VertexAttributeType_UByte4:
            read (*channel_iter, "#text", MakeVertexAttributeIterator<math::vec4ub> (vs, attribute.offset), vertices_count);
            break;          
          case VertexAttributeType_Influence:
            read (*channel_iter, "#text", MakeVertexAttributeIterator<VertexInfluence> (vs, attribute.offset), vertices_count);
            break;          
          default:
            return;
        }
      }

        //����������� ������

      vertex_streams.insert_pair (id, vs);
    }
    
      //������ ������ ��������� �����
    void ParseVertexWeightStream (Parser::Iterator vws_iter)
    {
        //������ �������������� ������

      const char* id            = get<const char*> (*vws_iter, "id");
      size_t      weights_count = get<size_t> (*vws_iter, "weights_count");

        //�������� ������

      VertexWeightStream vws (weights_count);

        //������ �����

      read (*vws_iter, "#text", vws.Data (), weights_count);

        //����������� ������

      vertex_weights.insert_pair (id, vws);
    }

      //������ ���������� ������
    void ParseVertexBuffer (Parser::Iterator vb_iter)
    {
      static const char* METHOD_NAME = "media::geometry::XmlMeshLibraryLoader::ParseVertexBuffer";

        //������ �������������� ������

      const char* id = get<const char*> (*vb_iter, "id");

         //�������� ���������� ������

      VertexBuffer vb;

         //������ ������ ��������� �����

      const char* weights_id = "";
      
      try_read (*vb_iter, "weights", weights_id);

      if (*weights_id)
      {
        VertexWeightStreamMap::iterator weights_iter = vertex_weights.find (weights_id);

        if (weights_iter == vertex_weights.end ())
          throw xtl::make_argument_exception (METHOD_NAME, "weights", weights_id, "Weights stream not found");

        vb.AttachWeights (weights_iter->second);
      }

         //������ ��������� �������
         
      for (Parser::AttributeIterator i=make_attribute_iterator (*vb_iter, "streams.#text"); i; ++i)
      {
        const char* id = *i;

        VertexStreamMap::iterator stream_iter = vertex_streams.find (id);

        if (stream_iter == vertex_streams.end ())
          throw xtl::make_argument_exception (METHOD_NAME, "streams[i]", id, "Vertex stream not found");

        vb.Attach (stream_iter->second);
      }

         //����������� ���������� ������

      vertex_buffers.insert_pair (id, vb);
    }
    
      //������ ���������� ������
    void ParseIndexBuffer (Parser::Iterator ib_iter)
    {
        //������ �������������� ������

      const char* id = get<const char*> (*ib_iter, "id");

      size_t indices_count = get<size_t> (*ib_iter, "indices_count");

        //�������� ������

      IndexBuffer ib (indices_count);

        //������ ��������

      read (*ib_iter, "#text", ib.Data (), indices_count);

        //����������� ������

      index_buffers.insert_pair (id, ib);
    }
    
      //������ ���������
    void ParsePrimitive (Parser::Iterator primitive_iter, Mesh& mesh)
    {
      static const char* METHOD_NAME = "media::geometry::XmlMeshLibraryLoader::ParsePrimitive";
      
      const char*   type_string         = get<const char*> (*primitive_iter, "type");
      PrimitiveType type                = get_primitive_type (type_string);
      const char*   material            = get<const char*> (*primitive_iter, "material", "");
      size_t        vertex_buffer_index = get<size_t> (*primitive_iter, "vertex_buffer"),
                    first               = get<size_t> (*primitive_iter, "first"),
                    count               = get<size_t> (*primitive_iter, "count");

      if (type >= PrimitiveType_Num)
        throw xtl::make_argument_exception (METHOD_NAME, "type", type_string);

      if (vertex_buffer_index >= mesh.VertexBuffersCount ())
        throw xtl::make_range_exception (METHOD_NAME, "vertex_buffer", vertex_buffer_index, mesh.VertexBuffersCount ());

      const VertexBuffer& vertex_buffer  = mesh.VertexBuffer (vertex_buffer_index);
      size_t              vertices_count = vertex_buffer.VerticesCount (),
                          indices_count  = mesh.IndexBuffer ().Size (),
                          max_primitives_count = 0,
                          max_count      = indices_count ? indices_count : vertices_count;                          

      if (first > max_count)
        throw xtl::make_range_exception (METHOD_NAME, "first", first, max_count);

      max_count -= first;

      switch (type)
      {
        case PrimitiveType_LineList:
          max_primitives_count = max_count / 2;
          break;
        case media::geometry::PrimitiveType_LineStrip:          
          max_primitives_count = max_count > 1 ? max_count - 1 : 0;
          break;
        case media::geometry::PrimitiveType_TriangleList:
          max_primitives_count = max_count / 3;
          break;
        case media::geometry::PrimitiveType_TriangleStrip:
          max_primitives_count = max_count > 2 ? max_count - 2 : 0;
          break;          
        case media::geometry::PrimitiveType_TriangleFan:
          max_primitives_count = max_count > 2 ? max_count - 2 : 0;
          break;            
        default:
          break;
      }

      if (count > max_primitives_count)
        throw xtl::make_range_exception (METHOD_NAME, "count", count, max_primitives_count + 1);

      if (!count)
      {
        primitive_iter->Log ().Warning (*primitive_iter, "Empty primitive");
        return;
      }

      mesh.AddPrimitive (type, vertex_buffer_index, first, count, material);
    }
    
      //������ ����
    void ParseMesh (Parser::Iterator mesh_iter)
    {
      static const char* METHOD_NAME = "media::geometry::XmlMeshLibraryLoader::ParseMesh";
      
        //������ ���: ����, ���������� ������ (���� ����)
      
      const char *id    = get<const char*> (*mesh_iter, "id"),
                 *name  = get<const char*> (*mesh_iter, "name", id),
                 *ib_id = "";
      
      try_read (*mesh_iter, "index_buffer", ib_id);
                 
        //�������� ����

      Mesh mesh;

      mesh.Rename (name);                 
                 
        //����� ��������� �������        
        
      for (Parser::AttributeIterator vb_iter=make_attribute_iterator (*mesh_iter, "vertex_buffers.#text"); vb_iter; ++vb_iter)
      {
        VertexBufferMap::iterator iter = vertex_buffers.find (*vb_iter);

        if (iter == vertex_buffers.end ())
          throw xtl::make_argument_exception (METHOD_NAME, "vertex_buffers[i]", *vb_iter, "Vertex buffer not found");

        mesh.Attach (iter->second);
      }

        //����� ���������� ������
        
      IndexBuffer* ib = 0;
      
      if (*ib_id)
      {
        IndexBufferMap::iterator ib_iter = index_buffers.find (ib_id);
        
        if (ib_iter == index_buffers.end ())
          throw xtl::make_argument_exception (METHOD_NAME, "index_buffer", ib_id, "Index buffer not found");

        ib = &ib_iter->second;
      }      

      if (ib)
        mesh.Attach (*ib);

        //������ ����������

      for_each_child (*mesh_iter, "primitives.primitive", xtl::bind (&XmlMeshLibraryLoader::ParsePrimitive, this, _1, xtl::ref (mesh)));

        //������������� ���� � ������

      library.Attach (id, mesh);
    }
  
      //������ ����������
    void ParseLibrary (Parser::Iterator library_iter)
    {
      for_each_child (*library_iter, "vertex_streams.vertex_stream", xtl::bind (&XmlMeshLibraryLoader::ParseVertexStream, this, _1));
      for_each_child (*library_iter, "vertex_streams.vertex_weight_stream", xtl::bind (&XmlMeshLibraryLoader::ParseVertexWeightStream, this, _1));
      for_each_child (*library_iter, "vertex_buffers.vertex_buffer", xtl::bind (&XmlMeshLibraryLoader::ParseVertexBuffer, this, _1));
      for_each_child (*library_iter, "index_buffers.index_buffer", xtl::bind (&XmlMeshLibraryLoader::ParseIndexBuffer, this, _1));      
      for_each_child (*library_iter, "meshes.mesh", xtl::bind (&XmlMeshLibraryLoader::ParseMesh, this, _1));
    }

  public:
    XmlMeshLibraryLoader (const char* file_name, MeshLibrary& in_library) : parser (file_name, "xml"), library (in_library)
    {
      ParseLibrary (parser.Root ().First ("mesh_library"));

        //����������������

      Log log (LOG_NAME);

      parser.Log ().Print (xtl::bind (&Log::Print, xtl::ref (log), _1));
    }
    
  private:
    typedef stl::hash_map<stl::hash_key<const char*>, VertexStream>       VertexStreamMap;
    typedef stl::hash_map<stl::hash_key<const char*>, VertexWeightStream> VertexWeightStreamMap;
    typedef stl::hash_map<stl::hash_key<const char*>, VertexBuffer>       VertexBufferMap;
    typedef stl::hash_map<stl::hash_key<const char*>, IndexBuffer>        IndexBufferMap;

  private:
    Parser                parser;         //������
    MeshLibrary&          library;        //����������
    VertexStreamMap       vertex_streams; //����������� ��������� ������
    VertexWeightStreamMap vertex_weights; //����������� ������ ��������� �����
    VertexBufferMap       vertex_buffers; //����������� ��������� ������
    IndexBufferMap        index_buffers;  //����������� ��������� ������
};

/*
    �������������� ����������� ����������
*/

class XMeshLoaderComponent
{
  public:
    XMeshLoaderComponent ()
    {
      MeshLibraryManager::RegisterLoader ("xmesh", &LoadLibrary);      
    }

  private:
    static void LoadLibrary (const char* file_name, MeshLibrary& library)
    {
      XmlMeshLibraryLoader (file_name, library);
    }
};

extern "C" ComponentRegistrator<XMeshLoaderComponent> XMeshLoader ("media.geometry.loaders.XMeshLoader");

}

}
