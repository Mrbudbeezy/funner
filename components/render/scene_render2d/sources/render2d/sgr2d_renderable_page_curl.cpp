#include "shared.h"

using namespace render;
using namespace render::mid_level;
using namespace render::obsolete::render2d;
using namespace scene_graph;

typedef xtl::com_ptr<low_level::IBlendState>              BlendStatePtr;
typedef xtl::com_ptr<low_level::IBuffer>                  BufferPtr;
typedef xtl::com_ptr<low_level::IDepthStencilState>       DepthStencilStatePtr;
typedef xtl::com_ptr<ILowLevelFrame>                      LowLevelFramePtr;
typedef xtl::com_ptr<low_level::IInputLayout>             InputLayoutPtr;
typedef xtl::com_ptr<low_level::IProgram>                 ProgramPtr;
typedef xtl::com_ptr<low_level::IProgramParametersLayout> ProgramParametersLayoutPtr;
typedef xtl::com_ptr<low_level::IRasterizerState>         RasterizerStatePtr;
typedef xtl::com_ptr<low_level::ISamplerState>            SamplerStatePtr;

namespace
{

const char* LOG_NAME = "render.obsolete.render2d.RenderablePageCurl";

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ����������� ��� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RenderableVertex
{
  math::vec3f  position; //��������� ������� � ������������
  math::vec2f  texcoord; //���������� ����������
  math::vec4ub color;    //���� �������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ProgramParameters
{
  math::mat4f view_matrix;       //������� ����
  math::mat4f projection_matrix; //������� ��������
};

//����� ������� ��������� �� ���������
const char* DEFAULT_SHADER_SOURCE_CODE =
"Parameters\n"
"{\n"
"  float4x4 currentViewMatrix currentProjMatrix\n"
"}\n"
"ProjectionMatrix currentProjMatrix\n"
"ViewMatrix       currentViewMatrix\n"
"AlphaCompareMode AlwaysPass\n"
"Texmap0\n"
"{\n"
"  TexcoordU Explicit\n"
"  TexcoordV Explicit\n"
"  Blend     Modulate\n"
"}";

//���������������� ����������� ��������
void shader_error_log (const char* message)
{
  static common::Log log (LOG_NAME);

  log.Print (message);
}

}

/*
    �������� ���������� �������� ��������
*/

struct RenderablePageCurl::Impl : public ILowLevelFrame::IDrawCallback
{
  Render&                    render;                     //������ �� ������
  scene_graph::PageCurl*     page_curl;                  //�������� ����
  Renderable*                renderable;                 //������
  LowLevelFramePtr           low_level_frame;            //����� ��������� ���������
  BlendStatePtr              none_blend_state;           //��������� ���������
  BlendStatePtr              translucent_blend_state;    //��������� ���������
  InputLayoutPtr             input_layout;               //��������� ���������� ���������
  ProgramPtr                 default_program;            //������
  ProgramParametersLayoutPtr program_parameters_layout;  //������������ ���������� �������
  SamplerStatePtr            sampler_state;              //�������
  RasterizerStatePtr         rasterizer_state;           //��������� �������������
  DepthStencilStatePtr       depth_stencil_state;        //��������� ������ ������������� ���������
  BufferPtr                  constant_buffer;            //����������� �����
  BufferPtr                  vertex_buffer;              //��������� �����
  BufferPtr                  index_buffer;               //��������� �����
  RenderableVertex           vertices [4];               //�������, ����������� ������     ??????????????????
  math::vec3f                view_point;                 //������� ������
  math::mat4f                projection;                 //������� ������
  bool                       initialized;                //���������������� �� ����������� ����

  ///�����������
  Impl (scene_graph::PageCurl* in_page_curl, Render& in_render, Renderable* in_renderable)
    : render (in_render)
    , page_curl (in_page_curl)
    , renderable (in_renderable)
    , initialized (false)
  {
    ILowLevelRenderer* low_level_renderer = dynamic_cast<ILowLevelRenderer*> (render.Renderer ().get ());

    if (!low_level_renderer)
      throw xtl::format_operation_exception ("render::obsolete::render2d::RenderablePageCurl::RenderablePageCurl", "Can't cast render to ILowLevelRenderer");

    low_level_frame = LowLevelFramePtr (low_level_renderer->CreateFrame (), false);

    low_level_frame->SetCallback (this);

    //!!!!!!!!!
    for (size_t i=0; i<4; i++)
      vertices [i].color = math::vec4ub (255, 255, 255, 255);

    vertices [0].position = math::vec3f (0, 0, 0);
    vertices [0].texcoord = math::vec2f (0, 0);
    vertices [1].position = math::vec3f (1, 0, 0);
    vertices [1].texcoord = math::vec2f (1, 0);
    vertices [2].position = math::vec3f (0, 1, 0);
    vertices [2].texcoord = math::vec2f (0, 1);
    vertices [3].position = math::vec3f (1, 1, 0);
    vertices [3].texcoord = math::vec2f (1, 1);
  }

  //�������� ��������� ���������� ������
  BlendStatePtr CreateBlendState
   (low_level::IDevice&      device,
    bool                     blend_enable,
    low_level::BlendArgument src_arg,
    low_level::BlendArgument dst_color_arg,
    low_level::BlendArgument dst_alpha_arg)
  {
    low_level::BlendDesc blend_desc;

    memset (&blend_desc, 0, sizeof (blend_desc));

    blend_desc.blend_enable                     = blend_enable;
    blend_desc.blend_color_operation            = low_level::BlendOperation_Add;
    blend_desc.blend_alpha_operation            = low_level::BlendOperation_Add;
    blend_desc.blend_color_source_argument      = src_arg;
    blend_desc.blend_color_destination_argument = dst_color_arg;
    blend_desc.blend_alpha_source_argument      = src_arg;
    blend_desc.blend_alpha_destination_argument = dst_alpha_arg;
    blend_desc.color_write_mask                 = low_level::ColorWriteFlag_All;

    return BlendStatePtr (device.CreateBlendState (blend_desc), false);
  }

  //�������� ������� ������������ ������� ���������
  InputLayoutPtr CreateInputLayout (low_level::IDevice& device)
  {
    low_level::VertexAttribute attributes [3];

    memset (attributes, 0, sizeof (attributes));

    attributes [0].semantic = low_level::VertexAttributeSemantic_Position;
    attributes [0].format   = low_level::InputDataFormat_Vector3;
    attributes [0].type     = low_level::InputDataType_Float;
    attributes [0].slot     = 0;
    attributes [0].offset   = offsetof (RenderableVertex, position);
    attributes [0].stride   = sizeof (RenderableVertex);

    attributes [1].semantic = low_level::VertexAttributeSemantic_TexCoord0;
    attributes [1].format   = low_level::InputDataFormat_Vector2;
    attributes [1].type     = low_level::InputDataType_Float;
    attributes [1].slot     = 0;
    attributes [1].offset   = offsetof (RenderableVertex, texcoord);
    attributes [1].stride   = sizeof (RenderableVertex);

    attributes [2].semantic = low_level::VertexAttributeSemantic_Color;
    attributes [2].format   = low_level::InputDataFormat_Vector4;
    attributes [2].type     = low_level::InputDataType_UByte;
    attributes [2].slot     = 0;
    attributes [2].offset   = offsetof (RenderableVertex, color);
    attributes [2].stride   = sizeof (RenderableVertex);

    low_level::InputLayoutDesc layout_desc;

    memset (&layout_desc, 0, sizeof layout_desc);

    layout_desc.vertex_attributes_count = sizeof attributes / sizeof *attributes;
    layout_desc.vertex_attributes       = attributes;
    layout_desc.index_type              = low_level::InputDataType_UShort;
    layout_desc.index_buffer_offset     = 0;

    return InputLayoutPtr (device.CreateInputLayout (layout_desc), false);
  }

  //�������� ���������
  ProgramPtr CreateProgram (low_level::IDevice& device, const char* shader_name, const char* shader_source)
  {
    low_level::ShaderDesc shader_desc;

    memset (&shader_desc, 0, sizeof (shader_desc));

    shader_desc.name             = shader_name;
    shader_desc.source_code_size = strlen (shader_source);
    shader_desc.source_code      = shader_source;
    shader_desc.profile          = "ffp";
    shader_desc.options          = "";

    return ProgramPtr (device.CreateProgram (1, &shader_desc, &shader_error_log), false);
  }

  //�������� ��������
  SamplerStatePtr CreateSampler (low_level::IDevice& device)
  {
    low_level::SamplerDesc sampler_desc;

    memset (&sampler_desc, 0, sizeof (sampler_desc));

    sampler_desc.min_filter           = low_level::TexMinFilter_LinearMipLinear;
    sampler_desc.mag_filter           = low_level::TexMagFilter_Linear;
    sampler_desc.max_anisotropy       = 1;
    sampler_desc.wrap_u               = low_level::TexcoordWrap_Clamp;
    sampler_desc.wrap_v               = low_level::TexcoordWrap_Clamp;
    sampler_desc.comparision_function = low_level::CompareMode_AlwaysPass;
    sampler_desc.min_lod              = 0;
    sampler_desc.max_lod              = FLT_MAX;

    return SamplerStatePtr (device.CreateSamplerState (sampler_desc), false);
  }

  //�������� ��������� ������������ ���������� ��������
  ProgramParametersLayoutPtr CreateProgramParametersLayout (low_level::IDevice& device)
  {
    low_level::ProgramParameter program_parameters [2];

    memset (program_parameters, 0, sizeof (program_parameters));

    program_parameters [0].name   = "currentViewMatrix";
    program_parameters [0].type   = low_level::ProgramParameterType_Float4x4;
    program_parameters [0].slot   = 0;
    program_parameters [0].count  = 1;
    program_parameters [0].offset = offsetof (ProgramParameters, view_matrix);

    program_parameters [1].name   = "currentProjMatrix";
    program_parameters [1].type   = low_level::ProgramParameterType_Float4x4;
    program_parameters [1].slot   = 0;
    program_parameters [1].count  = 1;
    program_parameters [1].offset = offsetof (ProgramParameters, projection_matrix);

    low_level::ProgramParametersLayoutDesc program_parameters_layout_desc;

    memset (&program_parameters_layout_desc, 0, sizeof (program_parameters_layout_desc));

    program_parameters_layout_desc.parameters_count = sizeof (program_parameters) / sizeof (*program_parameters);
    program_parameters_layout_desc.parameters       = program_parameters;

    return ProgramParametersLayoutPtr (device.CreateProgramParametersLayout (program_parameters_layout_desc), false);
  }

  //�������� ������������ ������
  BufferPtr CreateConstantBuffer (low_level::IDevice& device)
  {
    low_level::BufferDesc constant_buffer_desc;

    memset (&constant_buffer_desc, 0, sizeof (constant_buffer_desc));

    constant_buffer_desc.size         = sizeof (ProgramParameters);
    constant_buffer_desc.usage_mode   = low_level::UsageMode_Dynamic;
    constant_buffer_desc.bind_flags   = low_level::BindFlag_ConstantBuffer;
    constant_buffer_desc.access_flags = low_level::AccessFlag_ReadWrite;

    return BufferPtr (device.CreateBuffer (constant_buffer_desc), false);
  }

  //�������� ��������� ������ ������������
  RasterizerStatePtr CreateRasterizerState (low_level::IDevice& device)
  {
    low_level::RasterizerDesc rasterizer_desc;

    memset (&rasterizer_desc, 0, sizeof (rasterizer_desc));

    rasterizer_desc.fill_mode               = low_level::FillMode_Solid;
    rasterizer_desc.cull_mode               = low_level::CullMode_None;
    rasterizer_desc.front_counter_clockwise = true;
    rasterizer_desc.depth_bias              = 0;
    rasterizer_desc.scissor_enable          = false;
    rasterizer_desc.multisample_enable      = false;
    rasterizer_desc.antialiased_line_enable = false;

    return RasterizerStatePtr (device.CreateRasterizerState (rasterizer_desc), false);
  }

  //�������� ��������� ������ ������������� ���������
  DepthStencilStatePtr CreateDepthStencilState (low_level::IDevice& device)
  {
    low_level::DepthStencilDesc depth_stencil_desc;

    memset (&depth_stencil_desc, 0, sizeof (depth_stencil_desc));

    depth_stencil_desc.depth_test_enable   = true;
    depth_stencil_desc.depth_write_enable  = true;
    depth_stencil_desc.depth_compare_mode  = low_level::CompareMode_Less;

    return DepthStencilStatePtr (device.CreateDepthStencilState (depth_stencil_desc), false);
  }

  ///���������
  void Draw (low_level::IDevice& device)
  {
    if (!initialized)
    {
      none_blend_state          = CreateBlendState              (device, false, low_level::BlendArgument_Zero, low_level::BlendArgument_Zero, low_level::BlendArgument_Zero);
      translucent_blend_state   = CreateBlendState              (device, true, low_level::BlendArgument_SourceAlpha, low_level::BlendArgument_InverseSourceAlpha, low_level::BlendArgument_InverseSourceAlpha);
      input_layout              = CreateInputLayout             (device);
      default_program           = CreateProgram                 (device, "render.obsolete.renderer2d.RenderablePageCurl.default_program", DEFAULT_SHADER_SOURCE_CODE);
      program_parameters_layout = CreateProgramParametersLayout (device);
      constant_buffer           = CreateConstantBuffer          (device);
      rasterizer_state          = CreateRasterizerState         (device);
      depth_stencil_state       = CreateDepthStencilState       (device);
      sampler_state             = CreateSampler                 (device);

      low_level::BufferDesc buffer_desc;

      memset (&buffer_desc, 0, sizeof buffer_desc);

      buffer_desc.usage_mode   = low_level::UsageMode_Stream;
      buffer_desc.bind_flags   = low_level::BindFlag_VertexBuffer;
      buffer_desc.access_flags = low_level::AccessFlag_Write;
      buffer_desc.size         = sizeof (RenderableVertex) * 4;

      vertex_buffer = BufferPtr (device.CreateBuffer (buffer_desc), false);

        //�������� ���������� ������

      memset (&buffer_desc, 0, sizeof buffer_desc);

      buffer_desc.usage_mode   = low_level::UsageMode_Stream;
      buffer_desc.bind_flags   = low_level::BindFlag_IndexBuffer;
      buffer_desc.access_flags = low_level::AccessFlag_Write;
      buffer_desc.size         = sizeof (unsigned short) * 6;

      index_buffer = BufferPtr (device.CreateBuffer (buffer_desc), false);

      vertex_buffer->SetData (0, sizeof (vertices), vertices);

      unsigned short indices [6] = { 0, 1, 2, 1, 2, 3 };

      index_buffer->SetData (0, sizeof (indices), indices);

      initialized = true;
    }

    ProgramParameters program_parameters;

    program_parameters.view_matrix       = math::translate (-view_point);
    program_parameters.projection_matrix = projection;

    constant_buffer->SetData (0, sizeof (program_parameters), &program_parameters);

      //��������� ����� ��������

    device.ISSetInputLayout             (input_layout.get ());
    device.RSSetState                   (rasterizer_state.get ());
    device.SSSetConstantBuffer          (0, constant_buffer.get ());
    device.SSSetProgramParametersLayout (program_parameters_layout.get ());
    device.SSSetSampler                 (0, sampler_state.get ());
    device.SSSetProgram                 (default_program.get ());
    device.OSSetDepthStencilState       (depth_stencil_state.get ());

    BindMaterial (device, page_curl->PageMaterial (PageCurlPageType_BackLeft));

    device.ISSetVertexBuffer (0, vertex_buffer.get ());
    device.ISSetIndexBuffer  (index_buffer.get ());

    device.DrawIndexed (low_level::PrimitiveType_TriangleList, 0, 6, 0);
  }

  void BindMaterial (low_level::IDevice& device, const char* name)
  {
    static const char* METHOD_NAME = "render::obsolete::render2d::RenderablePageCurl::BindMaterial";

      //��������� ��������� �� ����

    SpriteMaterial* material = render.GetMaterial (name);

    BlendStatePtr material_blend_state;

    switch (material->BlendMode ())
    {
      case media::rfx::obsolete::SpriteBlendMode_None:
        material_blend_state = none_blend_state;
        break;
      case media::rfx::obsolete::SpriteBlendMode_Translucent:
        material_blend_state = translucent_blend_state;
        break;
      default:
        throw xtl::format_operation_exception (METHOD_NAME, "Unsupported material '%s' blend mode, only 'none' and 'translucent' page material blend mode supported", name);
    }

    device.OSSetBlendState (material_blend_state.get ());

    //��������� ��������

    ILowLevelTexture* texture = dynamic_cast<ILowLevelTexture*> (render.GetTexture (material->Image (), false, renderable));

    if (!texture)
      throw xtl::format_operation_exception (METHOD_NAME, "Material '%s' has unsupported texture type, only ILowLevelTexture supported", name);

      //��������� ��������
    device.SSSetTexture (0, texture->GetTexture ());
  }
};

/*
    �����������
*/

RenderablePageCurl::RenderablePageCurl (scene_graph::PageCurl* page_curl, Render& render)
  : Renderable (page_curl),
    impl (new Impl (page_curl, render, this))
{
}

RenderablePageCurl::~RenderablePageCurl ()
{
}

/*
    ���������
*/

void RenderablePageCurl::DrawCore (IFrame& frame)
{
  mid_level::Viewport viewport;

  frame.GetViewPoint  (impl->view_point);
  frame.GetProjection (impl->projection);
  frame.GetViewport   (viewport);

  impl->low_level_frame->SetRenderTargets (frame.GetRenderTarget (), frame.GetDepthStencilTarget ());
  impl->low_level_frame->SetViewport (viewport);

  impl->render.Renderer ()->AddFrame (impl->low_level_frame.get ());
}
