#include "shared.h"

using namespace media::geometry;
using namespace common;

/*
 * array <> format: size_t elements_count; elements
 *
 * File format: int header; int version; data
 *
 * data format: array<vertex_stream>; array<vertex_weight_stream>; array<vertex_buffer_desc>; array<index_buffer>; array<mesh>
 *
 * vertex_stream format: size_t vertices_count; size_t vertex_size; array<vertex_stream_channel>; binary<data>
 * vertex_stream_channel format: VertexAttributeSemantic semantic; VertexAttributeType type; size_t offset
 * vertex_weight_stream format: array<VertexWeight>
 *
 * vertex_buffer_desc format: array<size_t> vertex_weigth_stream_id (0-1); array<size_t> vertex_stream_id;
 *
 * index_buffer format: array<unsigned int> indices
 *
 * mesh format: array<char> name; array<size_t> index_buffer (0-1); array<size_t> vertex_buffers; array<primitive> primitives;
 *
 * primitive format: PrimitiveType type; array<char> material; size_t vertex_buffer; size_t first; size_t count
 */

namespace components
{

namespace bin_mesh_loader
{

/*
    ���������
*/

const char HEADER [4] = {'B', 'M', 'S', 'H'};
const int VERSION = 2;

void file_read (InputFile& file, void* data, size_t size)
{
  if (file.Read (data, size) != size)
    throw xtl::format_operation_exception ("media::geometry::BinMeshLibraryLoader", "Can't read data from file at %u, file size is %u",
      file.Tell (), file.Size ());
}

stl::string file_read_string (InputFile& file)
{
  unsigned int length = 0;

  file_read (file, &length, sizeof (length));

  stl::string result;

  result.fast_resize (length);  

  file_read (file, &result [0], length);

  return result;
}

/*
    ��������� ���-������� � ������� Xml
*/

class BinMeshLibraryLoader
{
  private:
      //������ ���������� �������
    void ReadVertexStreamChannel (VertexFormat& vertex_format)
    {
      VertexAttributeSemantic semantic;
      VertexAttributeType     type;
      size_t                  offset;

      file_read (input_file, &semantic, sizeof (semantic));
      file_read (input_file, &type,     sizeof (type));
      file_read (input_file, &offset,   sizeof (offset));

      stl::string name = file_read_string (input_file);

      vertex_format.AddAttribute (name.c_str (), semantic, type, offset);
    }

      //������ ���������� ������
    void ReadVertexStream (size_t id)
    {
        //����������� ����� ������ � ������� �������

      size_t vertices_count, vertex_size;

      file_read (input_file, &vertices_count, sizeof (vertices_count));
      file_read (input_file, &vertex_size, sizeof (vertex_size));

        //������ ���������� �������

      VertexFormat vertex_format;
      size_t       vertex_stream_channels_count;

      file_read (input_file, &vertex_stream_channels_count, sizeof (vertex_stream_channels_count));

      for (size_t i = 0; i < vertex_stream_channels_count; i++)
        ReadVertexStreamChannel (vertex_format);

        //�������� ���������� ������

      VertexStream vs (vertices_count, VertexDeclaration (vertex_format, vertex_size));

        //������ ������

      file_read (input_file, vs.Data (), vertices_count * vertex_size);

        //����������� ������

      vertex_streams.insert_pair (id, vs);
    }

      //������ ��������� �������
    void ReadVertexStreams ()
    {
      size_t vertex_streams_count;

      file_read (input_file, &vertex_streams_count, sizeof (vertex_streams_count));

      for (size_t i = 0; i < vertex_streams_count; i++)
        ReadVertexStream (i);
    }

      //������ ������ ��������� �����
    void ReadVertexWeightStream (size_t id)
    {
      size_t weights_count;

      file_read (input_file, &weights_count, sizeof (weights_count));

        //�������� ������

      VertexWeightStream vws (weights_count);

        //������ �����

      file_read (input_file, vws.Data (), weights_count * sizeof (VertexWeight));

        //����������� ������

      vertex_weights.insert_pair (id, vws);
    }

    //������ ������� ��������� �����
    void ReadVertexWeights ()
    {
      size_t vertex_weights_streams_count;

      file_read (input_file, &vertex_weights_streams_count, sizeof (vertex_weights_streams_count));

      for (size_t i = 0; i < vertex_weights_streams_count; i++)
        ReadVertexWeightStream (i);
    }

      //������ ���������� ������
    void ReadVertexBuffer (size_t id)
    {
      static const char* METHOD_NAME = "media::geometry::BinMeshLibraryLoader::ReadVertexBuffer";

         //�������� ���������� ������

      VertexBuffer vb;

         //������ ������ ��������� �����

      size_t has_weights;

      file_read (input_file, &has_weights, sizeof (has_weights));

      if (has_weights)
      {
        if (has_weights > 1)
          throw xtl::format_operation_exception (METHOD_NAME, "Vertex buffer weights count more than 1");

        size_t weights_id;

        file_read (input_file, &weights_id, sizeof (weights_id));

        VertexWeightStreamMap::iterator weights_iter = vertex_weights.find (weights_id);

        if (weights_iter == vertex_weights.end ())
          throw xtl::make_argument_exception (METHOD_NAME, "weights", weights_id, "Weights stream not found");

        vb.AttachWeights (weights_iter->second);
      }

         //������ ��������� �������

      size_t streams_count;

      file_read (input_file, &streams_count, sizeof (streams_count));

      for (size_t i = 0; i < streams_count; i++)
      {
        size_t stream_id;

        file_read (input_file, &stream_id, sizeof (stream_id));

        VertexStreamMap::iterator stream_iter = vertex_streams.find (stream_id);

        if (stream_iter == vertex_streams.end ())
          throw xtl::make_argument_exception (METHOD_NAME, "streams[i]", stream_id, "Vertex stream not found");

        vb.Attach (stream_iter->second);
      }

         //����������� ���������� ������

      vertex_buffers.insert_pair (id, vb);
    }

      //������ ��������� �������
    void ReadVertexBuffers ()
    {
      size_t vertex_buffers_count;

      file_read (input_file, &vertex_buffers_count, sizeof (vertex_buffers_count));

      for (size_t i = 0; i < vertex_buffers_count; i++)
        ReadVertexBuffer (i);
    }

      //������ ���������� ������
    void ReadIndexBuffer (size_t id)
    {
      size_t indices_count;

      file_read (input_file, &indices_count, sizeof (indices_count));
      
      IndexType data_type = IndexType_Default;
      
      file_read (input_file, (int*)&data_type, sizeof (data_type));

        //�������� ������

      IndexBuffer ib (indices_count, data_type);

        //������ ��������

      file_read (input_file, ib.Data (), indices_count * sizeof (unsigned int));

        //����������� ������

      index_buffers.insert_pair (id, ib);
    }

      //������ ��������� �������
    void ReadIndexBuffers ()
    {
      size_t index_buffers_count;

      file_read (input_file, &index_buffers_count, sizeof (index_buffers_count));

      for (size_t i = 0; i < index_buffers_count; i++)
        ReadIndexBuffer (i);
    }

      //������ ���������
    void ReadPrimitive (Mesh& mesh)
    {
      static const char* METHOD_NAME = "media::geometry::BinMeshLibraryLoader::ParsePrimitive";

      PrimitiveType type;

      file_read (input_file, &type, sizeof (type));

      size_t material_name_length;

      file_read (input_file, &material_name_length, sizeof (material_name_length));

      xtl::uninitialized_storage<char> material (material_name_length + 1);

      file_read (input_file, material.data (), sizeof (char) * material_name_length);

      material.data ()[material_name_length] = '\0';

      size_t vertex_buffer_index, first, count;

      file_read (input_file, &vertex_buffer_index, sizeof (vertex_buffer_index));
      file_read (input_file, &first,               sizeof (first));
      file_read (input_file, &count,               sizeof (count));

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
        return;

      mesh.AddPrimitive (type, vertex_buffer_index, first, count, material.data ());
    }

      //������ ����
    void ReadMesh ()
    {
      static const char* METHOD_NAME = "media::geometry::BinMeshLibraryLoader::ParseMesh";
      
        //������ �������������� ����
        
      size_t id_length;
      
      file_read (input_file, &id_length, sizeof (id_length));
      
      xtl::uninitialized_storage<char> id (id_length + 1);
      
      file_read (input_file, id.data (), sizeof (char) * id_length);
      
      id.data ()[id_length] = '\0';

        //������ ����� ����

      size_t name_length;

      file_read (input_file, &name_length, sizeof (name_length));

      xtl::uninitialized_storage<char> name (name_length + 1);

      file_read (input_file, name.data (), sizeof (char) * name_length);

      name.data ()[name_length] = '\0';

        //�������� ����

      Mesh mesh;

      mesh.Rename (name.data ());

        //������ ���������� ������

      size_t has_index_buffer;

      file_read (input_file, &has_index_buffer, sizeof (has_index_buffer));

      IndexBuffer* ib = 0;

      if (has_index_buffer)
      {
        if (has_index_buffer > 1)
          throw xtl::format_operation_exception (METHOD_NAME, "Mesh index buffers count more than 1");

        size_t ib_id;

        file_read (input_file, &ib_id, sizeof (ib_id));

        IndexBufferMap::iterator ib_iter = index_buffers.find (ib_id);

        if (ib_iter == index_buffers.end ())
          throw xtl::make_argument_exception (METHOD_NAME, "index_buffer", ib_id, "Index buffer not found");

        ib = &ib_iter->second;
      }

      if (ib)
        mesh.Attach (*ib);

        //����� ��������� �������

      size_t vertex_buffers_count;

      file_read (input_file, &vertex_buffers_count, sizeof (vertex_buffers_count));

      for (size_t i = 0; i < vertex_buffers_count; i++)
      {
        size_t vb_id;

        file_read (input_file, &vb_id, sizeof (vb_id));

        VertexBufferMap::iterator iter = vertex_buffers.find (vb_id);

        if (iter == vertex_buffers.end ())
          throw xtl::make_argument_exception (METHOD_NAME, "vertex_buffers[i]", vb_id, "Vertex buffer not found");

        mesh.Attach (iter->second);
      }

        //������ ����������

      size_t primitives_count;

      file_read (input_file, &primitives_count, sizeof (primitives_count));

      for (size_t i = 0; i < primitives_count; i++)
        ReadPrimitive (mesh);

        //������������� ���� � ������

      library.Attach (id.data (), mesh);
    }

      //������ �����
    void ReadMeshes ()
    {
      size_t meshes_count;

      file_read (input_file, &meshes_count, sizeof (meshes_count));

      for (size_t i = 0; i < meshes_count; i++)
        ReadMesh ();
    }

      //������ ����������
    void ReadLibrary ()
    {
      ReadVertexStreams ();
      ReadVertexWeights ();
      ReadVertexBuffers ();
      ReadIndexBuffers  ();
      ReadMeshes        ();
    }

  public:
    BinMeshLibraryLoader (const char* file_name, MeshLibrary& in_library) : input_file (file_name), library (in_library)
    {
      static const char* METHOD_NAME = "media::geometry::BinMeshLibraryLoader::BinMeshLibraryLoader";

        //�������� ��������� �����

      char header [4] = {0, 0, 0, 0};

      file_read (input_file, header, sizeof (header));

      if (memcmp (&header, HEADER, 4))
        throw xtl::format_operation_exception (METHOD_NAME, "File '%s' has invalid header '%c%c%c%c', must be '%c%c%c%c'", file_name,
          header [0], header [1], header [2], header [3], HEADER [0], HEADER [1], HEADER [2], HEADER [3]);

      int version;

      file_read (input_file, &version, sizeof (version));

      if (version != VERSION)
        throw xtl::format_operation_exception (METHOD_NAME, "File '%s' has unsupported version %d, supported version - %d", file_name,
          version, VERSION);

        //������ ������

      ReadLibrary ();
    }

  private:
    typedef stl::hash_map<size_t, VertexStream>       VertexStreamMap;
    typedef stl::hash_map<size_t, VertexWeightStream> VertexWeightStreamMap;
    typedef stl::hash_map<size_t, VertexBuffer>       VertexBufferMap;
    typedef stl::hash_map<size_t, IndexBuffer>        IndexBufferMap;

  private:
    InputFile             input_file;     //�������� ����
    MeshLibrary&          library;        //����������
    VertexStreamMap       vertex_streams; //����������� ��������� ������
    VertexWeightStreamMap vertex_weights; //����������� ������ ��������� �����
    VertexBufferMap       vertex_buffers; //����������� ��������� ������
    IndexBufferMap        index_buffers;  //����������� ��������� ������
};

/*
    �������������� ����������� ����������
*/

class BinMeshLoaderComponent
{
  public:
    BinMeshLoaderComponent ()
    {
      MeshLibraryManager::RegisterLoader ("binmesh", &LoadLibrary);
    }

  private:
    static void LoadLibrary (const char* file_name, MeshLibrary& library)
    {
      BinMeshLibraryLoader (file_name, library);
    }
};

extern "C"
{

ComponentRegistrator<BinMeshLoaderComponent> BinMeshLoader ("media.geometry.loaders.BinMeshLoader");

}

}

}
