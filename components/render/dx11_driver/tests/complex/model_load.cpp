#include "shared.h"

const char* VS_SHADER_FILE_NAME  = "data/spy.vsh";
const char* PS_SHADER_FILE_NAME  = "data/spy.psh";
const char* MODEL_NAME           = "data/spy.xmesh";

#pragma pack (1)

typedef stl::vector<BufferPtr> BufferArray;

struct ModelVertexBuffer
{
  BufferArray    vertex_streams;
  InputLayoutPtr input_layout;
  size_t         id;
};

typedef xtl::shared_ptr<ModelVertexBuffer>     VertexBufferPtr;
typedef stl::hash_map<size_t, VertexBufferPtr> VertexBufferMap;
typedef stl::hash_map<size_t, BufferPtr>       BufferMap;
typedef stl::vector<VertexBufferPtr>           VertexBufferArray;

struct ModelPrimitive
{
  PrimitiveType   type;          //��� ���������
  VertexBufferPtr vertex_buffer; //��������� �����
  size_t          first;         //������ ������ �������/�������
  size_t          count;         //���������� ����������
};

typedef stl::vector<ModelPrimitive> PrimitiveArray;

struct ModelMesh
{
  VertexBufferArray vertex_buffers;
  BufferPtr         index_buffer;
  PrimitiveArray    primitives;
};

typedef xtl::shared_ptr<ModelMesh> ModelMeshPtr;
typedef stl::vector<ModelMeshPtr>  ModelMeshArray;

struct Model
{
  DevicePtr                    device;
  media::geometry::MeshLibrary library;
  BufferMap                    vertex_streams;
  VertexBufferMap              vertex_buffers;
  BufferMap                    index_buffers;
  ModelMeshArray               meshes;

  Model (const DevicePtr& in_device, const char* name) : device (in_device), library (name)
  {
//    printf ("Begin convert\n");

    LoadVertexBuffers ();
    LoadMeshes ();

//    printf ("Model has loaded successful!\n");
  }

  void LoadMeshes ()
  {
//    printf ("Load meshes\n");

    for (media::geometry::MeshLibrary::Iterator iter=library.CreateIterator (); iter; ++iter)
    {
      media::geometry::Mesh& mesh = *iter;

//      printf ("Load mesh '%s'\n", mesh.Name ());

      ModelMeshPtr model_mesh (new ModelMesh);

//      printf ("Add vertex buffers\n");

      for (size_t i=0, vb_count=mesh.VertexBuffersCount (); i<vb_count; i++)
      {
        media::geometry::VertexBuffer& vb = mesh.VertexBuffer (i);

        VertexBufferMap::iterator iter = vertex_buffers.find (vb.Id ());

        if (iter == vertex_buffers.end ())
        {
          printf ("Internal error: VB#%u not found\n", vb.Id ());
          abort ();
        }

        VertexBufferPtr model_vb = iter->second;

        model_mesh->vertex_buffers.push_back (model_vb);
      }

//      printf ("Add index buffer\n");

      const media::geometry::IndexBuffer& ib = mesh.IndexBuffer ();

      BufferMap::iterator ib_iter = index_buffers.find (ib.Id ());
      BufferPtr ib_buffer;

      if (ib_iter == index_buffers.end ())
      {
        if (ib.Size ())
        {
//          printf ("Create new hw index-buffer (%u indices)\n", ib.Size ());

          BufferDesc desc;

          memset (&desc, 0, sizeof desc);

          desc.size         = ib.Size () * sizeof (unsigned short);
          desc.usage_mode   = UsageMode_Default;
          desc.bind_flags   = BindFlag_IndexBuffer;
          desc.access_flags = AccessFlag_ReadWrite;
          
          stl::vector<unsigned short> index_data (ib.Size ());
          
          const unsigned int* src_index = (unsigned int*)ib.Data ();
          unsigned short*     dst_index = &index_data [0];
          
          for (size_t i=0; i<ib.Size (); i++, src_index++, dst_index++)
            *dst_index = static_cast<unsigned short> (*src_index);

          BufferPtr (device->CreateBuffer (desc), false).swap (ib_buffer);

          ib_buffer->SetData (0, desc.size, &index_data [0]);
        }

        index_buffers [ib.Id ()] = ib_buffer;
      }
      else ib_buffer = ib_iter->second;

      model_mesh->index_buffer = ib_buffer;

//      printf ("Add primitives\n");

      for (size_t i=0, primitives_count=mesh.PrimitivesCount (); i<primitives_count; i++)
      {
        const media::geometry::Primitive& src_primitive = mesh.Primitive (i);
        ModelPrimitive                    dst_primitive;

        memset (&dst_primitive, 0, sizeof dst_primitive);

        switch (src_primitive.type)
        {
          case media::geometry::PrimitiveType_LineList:
            dst_primitive.type   = PrimitiveType_LineList;
            dst_primitive.count  = src_primitive.count * 2;
            break;
          case media::geometry::PrimitiveType_LineStrip:
            dst_primitive.type  = PrimitiveType_LineStrip;
            dst_primitive.count = src_primitive.count + 1;
            break;
          case media::geometry::PrimitiveType_TriangleList:
            dst_primitive.type  = PrimitiveType_TriangleList;
            dst_primitive.count = src_primitive.count * 3;
            break;
          case media::geometry::PrimitiveType_TriangleStrip:
            dst_primitive.type = PrimitiveType_TriangleStrip;
            dst_primitive.count = src_primitive.count + 2;
            break;
          case media::geometry::PrimitiveType_TriangleFan:
            dst_primitive.type = PrimitiveType_TriangleFan;
            dst_primitive.count = src_primitive.count + 2;
            break;
          default:
            continue;
        }

        if (src_primitive.vertex_buffer >= model_mesh->vertex_buffers.size ())
          continue;

        dst_primitive.vertex_buffer = model_mesh->vertex_buffers [src_primitive.vertex_buffer];
        dst_primitive.first         = src_primitive.first;

//        printf ("primitive#%u: type=%s vb-slot=%u vb-id=VB#%u vb-ptr=%p first=%u count=%u (vb-slots=%u)\n", i, get_name (dst_primitive.type), src_primitive.vertex_buffer,
//                dst_primitive.vertex_buffer->id, dst_primitive.vertex_buffer.get (), dst_primitive.first, dst_primitive.count, model_mesh->vertex_buffers.size ());

        model_mesh->primitives.push_back (dst_primitive);
      }

      meshes.push_back (model_mesh);
    }
  }

  void LoadVertexBuffers ()
  {
//    printf ("Load vertex buffers:\n");

    for (media::geometry::MeshLibrary::Iterator iter=library.CreateIterator (); iter; ++iter)
    {
      media::geometry::Mesh& mesh = *iter;

      for (size_t i=0, vb_count=mesh.VertexBuffersCount (); i<vb_count; i++)
      {
        media::geometry::VertexBuffer& vb = mesh.VertexBuffer (i);

//        printf ("Load VB#%u\n", vb.Id ());

        VertexBufferPtr model_vb (new ModelVertexBuffer);

        model_vb->id = vb.Id ();

        stl::vector<VertexAttribute> vertex_attributes;

        for (size_t j=0, streams_count=vb.StreamsCount (); j<streams_count; j++)
        {
          media::geometry::VertexStream& vs = vb.Stream (j);

//          printf ("Load VS#%u\n", vs.Id ());

          BufferMap::iterator iter = vertex_streams.find (vs.Id ());
          BufferPtr vs_buffer;

          if (iter == vertex_streams.end ())
          {
//            printf ("Create new hw vertex-buffer\n");

            BufferDesc desc;

            memset (&desc, 0, sizeof desc);

            desc.size         = vs.Size () * vs.VertexSize ();
            desc.usage_mode   = UsageMode_Default;
            desc.bind_flags   = BindFlag_VertexBuffer;
            desc.access_flags = AccessFlag_Read | AccessFlag_Write;

            vs_buffer = BufferPtr (device->CreateBuffer (desc), false);

            vs_buffer->SetData (0, desc.size, vs.Data ());

            vertex_streams [vs.Id ()] = vs_buffer;
          }
          else vs_buffer = iter->second;

          const media::geometry::VertexFormat& vf = vs.Format ();

          for (size_t k=0, attr_count=vf.AttributesCount (); k<attr_count; k++)
          {
            const media::geometry::VertexAttribute& src_va = vf.Attribute (k);
            VertexAttribute                         dst_va;

            memset (&dst_va, 0, sizeof dst_va);

            switch (src_va.semantic)
            {
              case media::geometry::VertexAttributeSemantic_Position:
                dst_va.semantic = device->GetVertexAttributeSemanticName (VertexAttributeSemantic_Position);
                break;
              case media::geometry::VertexAttributeSemantic_Normal:
                dst_va.semantic = device->GetVertexAttributeSemanticName (VertexAttributeSemantic_Normal);
                break;
              case media::geometry::VertexAttributeSemantic_Color:
                dst_va.semantic = device->GetVertexAttributeSemanticName (VertexAttributeSemantic_Color);
                break;
              case media::geometry::VertexAttributeSemantic_TexCoord0:
              case media::geometry::VertexAttributeSemantic_TexCoord1:
              case media::geometry::VertexAttributeSemantic_TexCoord2:
              case media::geometry::VertexAttributeSemantic_TexCoord3:
              case media::geometry::VertexAttributeSemantic_TexCoord4:
              case media::geometry::VertexAttributeSemantic_TexCoord5:
              case media::geometry::VertexAttributeSemantic_TexCoord6:
              case media::geometry::VertexAttributeSemantic_TexCoord7:
                dst_va.semantic = device->GetVertexAttributeSemanticName ((VertexAttributeSemantic)(VertexAttributeSemantic_TexCoord0 + src_va.semantic - media::geometry::VertexAttributeSemantic_TexCoord0));
                break;
              default:
                continue;
            }

            switch (src_va.type)
            {
              case media::geometry::VertexAttributeType_Float2:
                dst_va.type   = InputDataType_Float;
                dst_va.format = InputDataFormat_Vector2;
                break;
              case media::geometry::VertexAttributeType_Float3:
                dst_va.type   = InputDataType_Float;
                dst_va.format = InputDataFormat_Vector3;
                break;
              case media::geometry::VertexAttributeType_Float4:
                dst_va.type   = InputDataType_Float;
                dst_va.format = InputDataFormat_Vector4;
                break;
              case media::geometry::VertexAttributeType_Short2:
                dst_va.type   = InputDataType_Short;
                dst_va.format = InputDataFormat_Vector2;
                break;
              case media::geometry::VertexAttributeType_Short3:
                dst_va.type   = InputDataType_Short;
                dst_va.format = InputDataFormat_Vector3;
                break;
              case media::geometry::VertexAttributeType_Short4:
                dst_va.type   = InputDataType_Short;
                dst_va.format = InputDataFormat_Vector4;
                break;
              case media::geometry::VertexAttributeType_UByte4:
                dst_va.type   = InputDataType_UByte;
                dst_va.format = InputDataFormat_Vector4;
                break;
              default:
                continue;
            }

            dst_va.stride = vs.VertexSize ();
            dst_va.slot   = model_vb->vertex_streams.size ();
            dst_va.offset = src_va.offset;

//            printf ("va #%u: semantic=%s type=%s format=%s stride=%u slot=%u offset=%u\n",
//                    vertex_attributes.size (), get_name (dst_va.semantic), get_name (dst_va.type), get_name (dst_va.format),
//                    dst_va.stride, dst_va.slot, dst_va.offset);

            vertex_attributes.push_back (dst_va);
          }

          model_vb->vertex_streams.push_back (vs_buffer);
        }

        InputLayoutDesc layout_desc;

        memset (&layout_desc, 0, sizeof layout_desc);

        layout_desc.vertex_attributes_count = vertex_attributes.size ();
        layout_desc.vertex_attributes       = &vertex_attributes [0];
        layout_desc.index_type              = InputDataType_UShort;
        layout_desc.index_buffer_offset     = 0;

        model_vb->input_layout = InputLayoutPtr (device->CreateInputLayout (layout_desc), false);

        vertex_buffers [vb.Id ()] = model_vb;
      }
    }
  }

  void Draw ()
  {
    IDeviceContext& context = *device->GetImmediateContext ();

    for (ModelMeshArray::iterator iter=meshes.begin (); iter!=meshes.end (); ++iter)
    {
      const ModelMesh& mesh = **iter;

      context.ISSetIndexBuffer (mesh.index_buffer.get ());

      for (PrimitiveArray::const_iterator iter=mesh.primitives.begin (); iter!=mesh.primitives.end (); ++iter)
      {
        const ModelPrimitive& primitive = *iter;
        ModelVertexBuffer&    vb        = *primitive.vertex_buffer;

        context.ISSetInputLayout (vb.input_layout.get ());

        for (size_t i=0; i<vb.vertex_streams.size (); i++)
        {
          BufferPtr vs = vb.vertex_streams [i];

          context.ISSetVertexBuffer (i, vs.get ());
        }

        if (mesh.index_buffer)
        {
          context.DrawIndexed (primitive.type, primitive.first, primitive.count, 0);
        }
        else
        {
          context.Draw (primitive.type, primitive.first, primitive.count);
        }
      }
    }
  }
};

typedef xtl::shared_ptr<Model> ModelPtr;

#pragma pack (1)

struct MyShaderParameters
{
  math::mat4f object_tm;
  math::mat4f view_tm;
  math::mat4f proj_tm;
  math::vec3f light_pos;
  math::vec3f light_dir;
};

ModelPtr load_model (const DevicePtr& device, const char* file_name)
{
  printf ("Load model '%s':\n", file_name);

  return ModelPtr (new Model (device, file_name));
}

void print (const char* message)
{
  printf ("Shader message: '%s'\n", message);
}

Model* model_ptr = 0;

size_t frames_count = 0;

void redraw (Test& test)
{
  Model& model = *model_ptr;
  
  model.Draw ();

  frames_count++;
}

void idle (Test& test)
{
  if (test.window.IsClosed ())
    return;
    
  static size_t last = 0;
  static float angle;

  static size_t last_fps = 0;  
  
  if (common::milliseconds () - last > 25)
  {
    float dt = (common::milliseconds () - last) / 1000.0f;
    
    MyShaderParameters my_shader_parameters;

    IBuffer* cb = test.device->GetImmediateContext ()->SSGetConstantBuffer (0);

    if (!cb)
    {
      printf ("Null constant buffer #0\n");
      return;
    }

    cb->GetData (0, sizeof my_shader_parameters, &my_shader_parameters);

    angle += 0.5f * dt;

    my_shader_parameters.object_tm = math::rotate (math::radian (angle), math::vec3f (0, 0, 1)) *
                                     math::rotate (math::radian (angle*0.2f), math::vec3f (1, 0, 0));

    my_shader_parameters.light_pos = math::vec3f (40 * cos (angle), 40 * sin (angle), 0.0f);
    my_shader_parameters.light_dir = -normalize (my_shader_parameters.light_pos);

    cb->SetData (0, sizeof my_shader_parameters, &my_shader_parameters);    
    
    last = common::milliseconds ();
    
    return;
  }  

  if (common::milliseconds () - last_fps > 1000)
  {
    printf ("FPS: %.2f\n", float (frames_count)/float (common::milliseconds () - last_fps)*1000.f);
    fflush (stdout);

    last_fps = common::milliseconds ();
    frames_count = 0;
    return;
  }

//  test.window.Invalidate ();
  test.OnRedraw ();
}

//��������� ��������������� ������� ��������
math::mat4f get_ortho_proj (float left, float right, float bottom, float top, float znear, float zfar)
{
  math::mat4f proj_matrix;

  float width  = right - left,
        height = top - bottom,
        depth  = zfar - znear;

    //������� ������� �������������, ������������ gluOrtho2D

  proj_matrix [0] = math::vec4f (2.0f / width, 0, 0, - (right + left) / width);
  proj_matrix [1] = math::vec4f (0, 2.0f / height, 0, - (top + bottom) / height);
  proj_matrix [2] = math::vec4f (0, 0, -2.0f / depth, - (znear + zfar) / depth);
  proj_matrix [3] = math::vec4f (0, 0, 0, 1);

  return proj_matrix;
}

void log_print (const char* stream, const char* message)
{
  printf ("%s: %s\n", stream, message);
}

int main ()
{
  printf ("Results of model_load_test:\n");

  common::LogFilter filter ("*", &log_print);

  try
  {
    Test test (L"DX11 device test window (model_load)", &redraw, "*", "debug=1");

    test.window.Show ();

    ModelPtr model = load_model (test.device, MODEL_NAME);

    model_ptr = model.get ();

    printf ("Set shader stage\n");

    stl::string vs_shader_source = read_shader (VS_SHADER_FILE_NAME), ps_shader_source = read_shader (PS_SHADER_FILE_NAME);

    ShaderDesc shader_descs [] = {
      {VS_SHADER_FILE_NAME, size_t (-1), vs_shader_source.c_str (), "hlsl.vs", ""},
      {PS_SHADER_FILE_NAME, size_t (-1), ps_shader_source.c_str (), "hlsl.ps", ""},
    };

    static ProgramParameter shader_parameters[] = {
      {"myProjMatrix", ProgramParameterType_Float4x4, 0, 1, TEST_OFFSETOF (MyShaderParameters, proj_tm)},
      {"myViewMatrix", ProgramParameterType_Float4x4, 0, 1, TEST_OFFSETOF (MyShaderParameters, view_tm)},
      {"myObjectMatrix", ProgramParameterType_Float4x4, 0, 1, TEST_OFFSETOF (MyShaderParameters, object_tm)},
      {"lightPos", ProgramParameterType_Float3, 0, 1, TEST_OFFSETOF (MyShaderParameters, light_pos)},
      {"lightDir", ProgramParameterType_Float3, 0, 1, TEST_OFFSETOF (MyShaderParameters, light_dir)},
    };

    ProgramParametersLayoutDesc program_parameters_layout_desc = {sizeof shader_parameters / sizeof *shader_parameters, shader_parameters};

    ProgramPtr shader (test.device->CreateProgram (sizeof shader_descs / sizeof *shader_descs, shader_descs, &print), false);
    ProgramParametersLayoutPtr program_parameters_layout (test.device->CreateProgramParametersLayout (program_parameters_layout_desc), false);

    BufferDesc cb_desc;

    memset (&cb_desc, 0, sizeof cb_desc);

    cb_desc.size         = sizeof (MyShaderParameters);
    cb_desc.usage_mode   = UsageMode_Default;
    cb_desc.bind_flags   = BindFlag_ConstantBuffer;
    cb_desc.access_flags = AccessFlag_ReadWrite;

    BufferPtr cb (test.device->CreateBuffer (cb_desc), false);

    MyShaderParameters my_shader_parameters;

    my_shader_parameters.proj_tm   = get_ortho_proj (-100, 100, -100, 100, -1000, 1000);
    my_shader_parameters.view_tm = inverse (math::lookat (math::vec3f (0, 400, 0), math::vec3f (0.0f), math::vec3f (0, 0, 1)));

    cb->SetData (0, sizeof my_shader_parameters, &my_shader_parameters);

    test.device->GetImmediateContext ()->SSSetProgram (shader.get ());
    test.device->GetImmediateContext ()->SSSetProgramParametersLayout (program_parameters_layout.get ());
    test.device->GetImmediateContext ()->SSSetConstantBuffer (0, cb.get ());

    printf ("Register callbacks\n");

    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle, xtl::ref (test)));

    printf ("Main loop\n");

    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
