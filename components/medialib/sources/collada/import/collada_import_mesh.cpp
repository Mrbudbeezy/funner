#include "shared.h"

using namespace media::geometry;
using namespace media::collada;
using namespace xtl;

/*
    ������ ���� �� COLLADA
*/

namespace
{

void append_surface (const Surface& src_surface, MeshModel& dst_model)
{
  typedef media::geometry::Vertex<Position3f, Normalf> ColladaVertex;
  
    //������ ������
  
  VertexBuffer vb;  
  
    //������ ��������� � ��������
    
  size_t vertices_count = src_surface.VerticesCount ();

  VertexStream vs (vertices_count, make_vertex_declaration<ColladaVertex> ());
  
  const media::collada::Vertex* src_vertex = src_surface.Vertices ();
  ColladaVertex*                dst_vertex = vs.Data<ColladaVertex> ();

  if (!dst_vertex)
    return;

  for (size_t i=0; i<vertices_count; i++, src_vertex++, dst_vertex++)
  {
    dst_vertex->position = src_vertex->coord;
    dst_vertex->normal   = src_vertex->normal;
  }
  
  vb.Attach (vs);
  
    //������ ���������� ��������� ???
    
    //������ ��������� ������ ???
    
    //������ ��������� �����
    
    //������ ��������
    
  size_t indices_count = src_surface.IndicesCount ();
  
  IndexBuffer ib (indices_count);

  stl::copy (src_surface.Indices (), src_surface.Indices () + indices_count, ib.Data ());

    //�������� ����

  media::geometry::Mesh mesh;

  mesh.Attach (vb);
  mesh.Attach (ib);
  
  media::geometry::PrimitiveType primitive_type;
  
  switch (src_surface.PrimitiveType ())
  {
    case media::collada::PrimitiveType_LineList:      primitive_type = media::geometry::PrimitiveType_LineList;      break;
    case media::collada::PrimitiveType_LineStrip:     primitive_type = media::geometry::PrimitiveType_LineStrip;     break;
    case media::collada::PrimitiveType_TriangleList:  primitive_type = media::geometry::PrimitiveType_TriangleList;  break;
    case media::collada::PrimitiveType_TriangleStrip: primitive_type = media::geometry::PrimitiveType_TriangleStrip; break;
    case media::collada::PrimitiveType_TriangleFan:   primitive_type = media::geometry::PrimitiveType_TriangleFan;   break;
    default:                                          return;
  }

  mesh.AddPrimitive (primitive_type, 0, get_primitives_count (primitive_type, indices_count), src_surface.MaterialName ());
  
    //���������� ���� � ������
    
  dst_model.Attach (mesh);
}

void append_mesh (const media::collada::Mesh& src_mesh, MeshModel& dst_model)
{
    //������ ������������ (����������)

  src_mesh.Surfaces ().ForEach (bind (&append_surface, _1, ref (dst_model)));
}

}

namespace media
{

namespace geometry
{

void append (const media::collada::Mesh& src_mesh, MeshModel& dst_model)
{
  append_mesh (src_mesh, dst_model);
}

void append (const media::collada::Model& src_model, MeshModel& dst_model)
{
    //������ �����
    
  src_model.Meshes ().ForEach (bind (&append_mesh, _1, ref (dst_model)));
}

}

}

