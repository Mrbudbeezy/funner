#include "shared.h"

const char* DST_FILE_NAME = "/io/stdout/test.xmesh";

int main ()
{
  printf ("Results of xmesh_saver_test:\n");
  
  try
  {
    VertexStream vs1 (4, make_vertex_declaration<CustomVertex> ());    

    CustomVertex* verts1 = vs1.Data<CustomVertex> ();

    if (verts1)
    {
      for (size_t i=0; i<vs1.Size (); i++)
      {
        verts1 [i].position  = vec3f  ((float)i, (float)i * 2, (float)i * 3);
        verts1 [i].normal    = vec3f  ((float)(i & 1), (float)((i + 1) & 1), 0);
        verts1 [i].texcoord0 = vec2f  (0, -(float)i, 0);
        verts1 [i].color     = vec4ub (0, i, 0, i);
        verts1 [i].tangent   = vec3f  (1.0f, 0.0f, 0.0f);
      }
    }
    
    VertexWeightStream vws (5);
    
    VertexWeight* weights = vws.Data ();
    
    for (size_t i=0; i<vws.Size (); i++)
    {
      weights [i].joint_index  = i;
      weights [i].joint_weight = float (i) / float (vws.Size ());
    }
    
    typedef Vertex<VertexInfluence, Position3f> MyVertex;
    
    VertexStream vs2 (4, make_vertex_declaration<MyVertex> ());
    
    MyVertex* verts2 = vs2.Data<MyVertex> ();
    
    if (verts2)
    {
      for (size_t i=0; i<vs1.Size (); i++)
      {
        verts2 [i].first_weight  = i;
        verts2 [i].weights_count = 1;
        verts2 [i].position      = 0.0f;
      }
    }
    
    IndexBuffer ib (5);
    
    for (size_t i=0; i<ib.Size (); i++)
      ib.Data<unsigned int> () [i] = i;

    Mesh mesh1;

    mesh1.Rename ("mesh1");
    
    VertexBuffer vb1;
    
    vb1.Attach (vs1);

    mesh1.Attach (vb1);
    mesh1.Attach (ib);
    
    mesh1.AddPrimitive (PrimitiveType_TriangleList, 0, 0, 1, "material1");
    mesh1.AddPrimitive (PrimitiveType_TriangleStrip, 0, 1, 1, "material2");

    Mesh mesh2;
    
    mesh2.Rename ("mesh2");
    
    VertexBuffer vb2;
    
    vb2.Attach (vs1);
    vb2.Attach (vs2);
    vb2.AttachWeights (vws);
    
    mesh2.Attach (vb2);
    
    mesh2.AddPrimitive (PrimitiveType_LineList, 0, 0, 1, "material1");
    mesh2.AddPrimitive (PrimitiveType_LineStrip, 0, 1, 2, "material2");    

    MeshLibrary mesh_library;

    mesh_library.Rename ("my_mesh_library");
    
    mesh_library.Attach ("mesh1", mesh1);
    mesh_library.Attach ("mesh2", mesh2);

    mesh_library.Save (DST_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
