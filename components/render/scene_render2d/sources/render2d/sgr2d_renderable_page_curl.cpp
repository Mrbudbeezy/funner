#include "shared.h"

using namespace render;
using namespace render::mid_level;
using namespace render::obsolete::render2d;
using namespace scene_graph;

typedef xtl::com_ptr<low_level::IBlendState>              BlendStatePtr;
typedef xtl::com_ptr<low_level::IBuffer>                  BufferPtr;
typedef xtl::com_ptr<low_level::IDepthStencilState>       DepthStencilStatePtr;
typedef xtl::com_ptr<ILowLevelFrame>                      LowLevelFramePtr;
typedef xtl::com_ptr<ILowLevelTexture>                    LowLevelTexturePtr;
typedef xtl::com_ptr<low_level::IInputLayout>             InputLayoutPtr;
typedef xtl::com_ptr<low_level::IProgram>                 ProgramPtr;
typedef xtl::com_ptr<low_level::IProgramParametersLayout> ProgramParametersLayoutPtr;
typedef xtl::com_ptr<low_level::IRasterizerState>         RasterizerStatePtr;
typedef xtl::com_ptr<low_level::ISamplerState>            SamplerStatePtr;
typedef xtl::com_ptr<low_level::IStateBlock>              StateBlockPtr;
typedef xtl::com_ptr<low_level::ITexture>                 DeviceTexturePtr;
typedef xtl::intrusive_ptr<RenderablePageCurlMesh>        RenderablePageCurlMeshPtr;

namespace
{

const char* LOG_NAME = "render.obsolete.render2d.RenderablePageCurl";

const float  EPS                   = 0.001f;
const float  BACK_SHADOW_OFFSET    = 0;
const float  MAX_SHADOW_LOG_VALUE  = 1.0;
const float  PI                    = 3.1415926f;
const size_t SHADOW_TEXTURE_WIDTH  = 16;
const float  STATIC_PAGES_Z_OFFSET = -0.001f;

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
  math::mat4f object_matrix;     //������� �������
};

//����� ������� ��������� �� ���������
const char* DEFAULT_SHADER_SOURCE_CODE =
"Parameters\n"
"{\n"
"  float4x4 currentViewMatrix currentProjMatrix objMatrix\n"
"}\n"
"ObjectMatrix     objMatrix\n"
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
  Render&                    render;                            //������ �� ������
  scene_graph::PageCurl*     page_curl;                         //�������� ����
  Renderable*                renderable;                        //������
  DeviceTexturePtr           shadow_texture;                    //�������� ����
  LowLevelTexturePtr         page_textures [scene_graph::PageCurlPageType_Num];                     //�������� �������
  SpriteMaterialPtr          page_materials [scene_graph::PageCurlPageType_Num];                    //��������� �������
  size_t                     current_page_material_name_hashes [scene_graph::PageCurlPageType_Num]; //��� �������� ����� ���������� �������
  LowLevelFramePtr           low_level_frame;                    //����� ��������� ���������
  BlendStatePtr              none_blend_state;                   //��������� ���������
  BlendStatePtr              translucent_blend_state;            //��������� ���������
  BlendStatePtr              mask_blend_state;                   //��������� ���������
  BlendStatePtr              additive_blend_state;               //��������� ���������
  BlendStatePtr              shadow_blend_state;                 //��������� ���������
  InputLayoutPtr             input_layout;                       //��������� ���������� ���������
  ProgramPtr                 default_program;                    //������
  ProgramParametersLayoutPtr program_parameters_layout;          //������������ ���������� �������
  SamplerStatePtr            sampler_state;                      //�������
  RasterizerStatePtr         rasterizer_no_cull_state;           //��������� ������������� ��� ���������
  RasterizerStatePtr         rasterizer_cull_back_state;         //��������� ������������� � ���������� ������ ������ �������������
  RasterizerStatePtr         rasterizer_cull_front_state;        //��������� ������������� � ���������� �������� ������ �������������
  RasterizerStatePtr         rasterizer_scissor_enabled_state;   //��������� ������������� � �������� ����������
  DepthStencilStatePtr       depth_stencil_state_write_enabled;  //��������� ������ ������������� ���������
  DepthStencilStatePtr       depth_stencil_state_write_disabled; //��������� ������ ������������� ���������
  BufferPtr                  constant_buffer;                    //����������� �����
  BufferPtr                  quad_vertex_buffer;                 //��������� ����� �� ��� ������������
  BufferPtr                  quad_index_buffer;                  //��������� ����� �� ��� ������������
  StateBlockPtr              render_state;                       //���������� ��������� �������
  math::vec3f                view_point;                         //������� ������
  math::mat4f                projection;                         //������� ������
  mid_level::Viewport        viewport;                           //������� ������
  RenderablePageCurlMeshPtr  curled_page;                        //����� ���������� ��������
  math::vec2ui               current_page_grid_size;             //������� ������ ����� ��������
  bool                       initialized;                        //���������������� �� ����������� ����

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

    memset (current_page_material_name_hashes, 0, sizeof (current_page_material_name_hashes));

    low_level_frame = LowLevelFramePtr (low_level_renderer->CreateFrame (), false);

    low_level_frame->SetCallback (this);
  }

  ILowLevelTexture* GetLowLevelTexture (const char* image_name)
  {
    static const char* METHOD_NAME = "render::obsolete::render2d::RenderablePageCurl::GetLowLevelTexture";

    ILowLevelTexture* texture = dynamic_cast<ILowLevelTexture*> (render.GetTexture (image_name, false, renderable));

    if (!texture)
      throw xtl::format_operation_exception (METHOD_NAME, "Material '%s' has unsupported texture type, only ILowLevelTexture supported", image_name);

    return texture;
  }

  //����������
  void Update ()
  {
    for (size_t i = 0; i < scene_graph::PageCurlPageType_Num; i++)
    {
      const char* material_name = page_curl->PageMaterial ((scene_graph::PageCurlPageType)i);

      size_t material_name_hash = xtl::xstrlen (material_name) ? common::strhash (material_name) : 0;

      if (material_name_hash != current_page_material_name_hashes [i])
      {
        if (material_name_hash)
        {
          page_materials [i] = render.GetMaterial (material_name);
          page_textures [i]  = GetLowLevelTexture (page_materials [i]->Image ());
        }
        else
        {
          page_textures [i]  = 0;
          page_materials [i] = 0;
        }

        current_page_material_name_hashes [i] = material_name_hash;
      }
    }
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

#ifdef ARM
    sampler_desc.min_filter           = low_level::TexMinFilter_Linear;
#else
    sampler_desc.min_filter           = low_level::TexMinFilter_LinearMipLinear;
#endif    
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
    low_level::ProgramParameter program_parameters [3];

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

    program_parameters [2].name   = "objMatrix";
    program_parameters [2].type   = low_level::ProgramParameterType_Float4x4;
    program_parameters [2].slot   = 0;
    program_parameters [2].count  = 1;
    program_parameters [2].offset = offsetof (ProgramParameters, object_matrix);

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
  RasterizerStatePtr CreateRasterizerState (low_level::IDevice& device, low_level::CullMode cull_mode, bool scissor_enable = false)
  {
    low_level::RasterizerDesc rasterizer_desc;

    memset (&rasterizer_desc, 0, sizeof (rasterizer_desc));

    rasterizer_desc.fill_mode               = low_level::FillMode_Solid;
    rasterizer_desc.cull_mode               = cull_mode;
    rasterizer_desc.front_counter_clockwise = true;
    rasterizer_desc.depth_bias              = 0;
    rasterizer_desc.scissor_enable          = scissor_enable;
    rasterizer_desc.multisample_enable      = false;
    rasterizer_desc.antialiased_line_enable = false;

    return RasterizerStatePtr (device.CreateRasterizerState (rasterizer_desc), false);
  }

  //�������� ��������� ������ ������������� ���������
  DepthStencilStatePtr CreateDepthStencilState (low_level::IDevice& device, bool write_enabled)
  {
    low_level::DepthStencilDesc depth_stencil_desc;

    memset (&depth_stencil_desc, 0, sizeof (depth_stencil_desc));

    depth_stencil_desc.depth_test_enable   = true;
    depth_stencil_desc.depth_write_enable  = write_enabled;
    depth_stencil_desc.depth_compare_mode  = low_level::CompareMode_Less;

    return DepthStencilStatePtr (device.CreateDepthStencilState (depth_stencil_desc), false);
  }

  //��������� �������
  math::vec4ub GetPageColor ()
  {
    const math::vec4f& float_page_color = page_curl->PageColor ();

    return math::vec4ub ((unsigned char)(float_page_color.x * 255), (unsigned char)(float_page_color.y * 255), (unsigned char)(float_page_color.z * 255), (unsigned char)(float_page_color.w * 255));
  }

  void GetTexCoords (bool left, float& min_s, float& max_s)
  {
    switch (page_curl->Mode ())
    {
      case PageCurlMode_SinglePage:
      case PageCurlMode_DoublePageDoubleMaterial:
        min_s = 0.f;
        max_s = 1.f;
        break;
      case PageCurlMode_DoublePageSingleMaterial:
        if (left)
        {
          min_s = 0.f;
          max_s = 0.5f;
        }
        else
        {
          min_s = 0.5f;
          max_s = 1.f;
        }
        break;
    }
  }

  scene_graph::PageCurlPageType GetCurledRightPageType ()
  {
    switch (page_curl->Mode ())
    {
      case PageCurlMode_SinglePage:
      case PageCurlMode_DoublePageSingleMaterial:
        return PageCurlPageType_Back;
      default:
        return PageCurlPageType_BackRight;
    }
  }

  scene_graph::PageCurlPageType GetCurledLeftPageType ()
  {
    switch (page_curl->Mode ())
    {
      case PageCurlMode_SinglePage:
        return PageCurlPageType_Back;
      case PageCurlMode_DoublePageSingleMaterial:
        return PageCurlPageType_Front;
      default:
        return PageCurlPageType_FrontLeft;
    }
  }

  //���������
  void Draw (low_level::IDevice& device)
  {
    if (!initialized)
    {
      none_blend_state                   = CreateBlendState              (device, false, low_level::BlendArgument_Zero, low_level::BlendArgument_Zero, low_level::BlendArgument_Zero);
      translucent_blend_state            = CreateBlendState              (device, true, low_level::BlendArgument_SourceAlpha, low_level::BlendArgument_InverseSourceAlpha, low_level::BlendArgument_InverseSourceAlpha);
      mask_blend_state                   = CreateBlendState              (device, true, low_level::BlendArgument_Zero, low_level::BlendArgument_SourceColor, low_level::BlendArgument_SourceAlpha);
      additive_blend_state               = CreateBlendState              (device, true, low_level::BlendArgument_SourceAlpha, low_level::BlendArgument_One, low_level::BlendArgument_One);
      shadow_blend_state                 = CreateBlendState              (device, true, low_level::BlendArgument_One, low_level::BlendArgument_InverseSourceAlpha, low_level::BlendArgument_SourceAlpha);
      input_layout                       = CreateInputLayout             (device);
      default_program                    = CreateProgram                 (device, "render.obsolete.renderer2d.RenderablePageCurl.default_program", DEFAULT_SHADER_SOURCE_CODE);
      program_parameters_layout          = CreateProgramParametersLayout (device);
      constant_buffer                    = CreateConstantBuffer          (device);
      rasterizer_no_cull_state           = CreateRasterizerState         (device, low_level::CullMode_None);
      rasterizer_cull_front_state        = CreateRasterizerState         (device, low_level::CullMode_Front);
      rasterizer_cull_back_state         = CreateRasterizerState         (device, low_level::CullMode_Back);
      rasterizer_scissor_enabled_state   = CreateRasterizerState         (device, low_level::CullMode_None, true);
      depth_stencil_state_write_enabled  = CreateDepthStencilState       (device, true);
      depth_stencil_state_write_disabled = CreateDepthStencilState       (device, false);
      sampler_state                      = CreateSampler                 (device);

      low_level::StateBlockMask state_block_mask;

      state_block_mask.Set ();

      render_state = StateBlockPtr (device.CreateStateBlock (state_block_mask), false);

      low_level::BufferDesc buffer_desc;

      memset (&buffer_desc, 0, sizeof buffer_desc);

      buffer_desc.usage_mode   = low_level::UsageMode_Stream;
      buffer_desc.bind_flags   = low_level::BindFlag_VertexBuffer;
      buffer_desc.access_flags = low_level::AccessFlag_Write;
      buffer_desc.size         = sizeof (RenderableVertex) * 4;

      quad_vertex_buffer = BufferPtr (device.CreateBuffer (buffer_desc), false);

        //�������� ���������� ������

      memset (&buffer_desc, 0, sizeof buffer_desc);

      buffer_desc.usage_mode   = low_level::UsageMode_Stream;
      buffer_desc.bind_flags   = low_level::BindFlag_IndexBuffer;
      buffer_desc.access_flags = low_level::AccessFlag_Write;
      buffer_desc.size         = sizeof (unsigned short) * 6;

      quad_index_buffer = BufferPtr (device.CreateBuffer (buffer_desc), false);

      unsigned short indices [6] = { 0, 1, 2, 1, 2, 3 };

      quad_index_buffer->SetData (0, sizeof (indices), indices);

        //�������� �������� ����

      low_level::TextureDesc texture_desc;

      memset (&texture_desc, 0, sizeof (texture_desc));

      texture_desc.dimension            = low_level::TextureDimension_1D;
      texture_desc.width                = SHADOW_TEXTURE_WIDTH;
      texture_desc.height               = 1;
      texture_desc.layers               = 1;
      texture_desc.format               = low_level::PixelFormat_LA8;
      texture_desc.access_flags         = low_level::AccessFlag_ReadWrite;
      texture_desc.bind_flags           = low_level::BindFlag_Texture;
#ifndef ARM
      texture_desc.generate_mips_enable = true;
#endif

      shadow_texture = DeviceTexturePtr (device.CreateTexture (texture_desc), false);

      unsigned char texture_data [SHADOW_TEXTURE_WIDTH * 2];
      unsigned char *current_texel    = texture_data;
      float         shadow_value      = pow (page_curl->ShadowLogBase (), MAX_SHADOW_LOG_VALUE),
                    shadow_value_step = shadow_value / (SHADOW_TEXTURE_WIDTH - 1),
                    log_delimiter     = log (page_curl->ShadowLogBase ());

      for (size_t i = 0; i < SHADOW_TEXTURE_WIDTH; i++, shadow_value -= shadow_value_step)
      {
        *current_texel++ = 255;                                                             //color
        *current_texel++ = stl::max (0, (int)(log (shadow_value) / log_delimiter * 255));   //alpha
      }

      shadow_texture->SetData (0, 0, 0, 0, texture_desc.width, texture_desc.height, texture_desc.format, texture_data);

      initialized = true;
    }

    render_state->Capture ();

    const math::vec2ui& grid_size = page_curl->GridSize ();

    if (current_page_grid_size != grid_size)
    {
      curled_page = RenderablePageCurlMeshPtr (new RenderablePageCurlMesh (device, grid_size), false);

      current_page_grid_size = grid_size;
    }

    math::vec2f one_page_size = page_curl->Size ();

    if (page_curl->Mode () != PageCurlMode_SinglePage)
      one_page_size.x /= 2;

    curled_page->SetSize  (one_page_size);
    curled_page->SetColor (GetPageColor ());

    ProgramParameters program_parameters;

    program_parameters.view_matrix       = math::translate (-view_point);
    program_parameters.projection_matrix = projection;
    program_parameters.object_matrix     = page_curl->WorldTM () * math::scale (math::vec3f (1, 1, -1)) * math::translate (math::vec3f (-page_curl->Size ().x / 2, -page_curl->Size ().y / 2, 0));

    constant_buffer->SetData (0, sizeof (program_parameters), &program_parameters);

      //��������� ����� ��������

    device.ISSetInputLayout             (input_layout.get ());
    device.SSSetConstantBuffer          (0, constant_buffer.get ());
    device.SSSetProgramParametersLayout (program_parameters_layout.get ());
    device.SSSetSampler                 (0, sampler_state.get ());
    device.SSSetProgram                 (default_program.get ());
    device.OSSetDepthStencilState       (depth_stencil_state_write_enabled.get ());

    switch (page_curl->CurlCorner ())
    {
      case PageCurlCorner_LeftTop:
        DrawLeftTopCornerFlip (device);
        break;
      case PageCurlCorner_LeftBottom:
        DrawLeftBottomCornerFlip (device);
        break;
      case PageCurlCorner_RightTop:
        DrawRightTopCornerFlip (device);
        break;
      case PageCurlCorner_RightBottom:
        DrawRightBottomCornerFlip (device);
        break;
    }

    render_state->Apply ();
  }

  //��������� �����
  void DrawShadows (low_level::IDevice& device, float curl_radius, bool left_side)
  {
    device.OSSetDepthStencilState (depth_stencil_state_write_disabled.get ());
    device.OSSetBlendState        (shadow_blend_state.get ());
    device.SSSetTexture           (0, shadow_texture.get ());

    const math::vec2f& total_size = page_curl->Size ();
    math::vec2f        page_size  = total_size;

    if (page_curl->Mode () != PageCurlMode_SinglePage)
      page_size.x /= 2;

    float x_offset = left_side || page_curl->Mode () == PageCurlMode_SinglePage ? 0 : page_size.x;

    const math::vec3f& left_bottom_corner_position  = curled_page->GetCornerPosition (PageCurlCorner_LeftBottom);
    const math::vec3f& left_top_corner_position     = curled_page->GetCornerPosition (PageCurlCorner_LeftTop);
    const math::vec3f& right_bottom_corner_position = curled_page->GetCornerPosition (PageCurlCorner_RightBottom);
    const math::vec3f& right_top_corner_position    = curled_page->GetCornerPosition (PageCurlCorner_RightTop);
    const math::vec3f& top_corner_position          = left_side ? left_top_corner_position : right_top_corner_position;
    const math::vec3f& bottom_corner_position       = left_side ? left_bottom_corner_position : right_bottom_corner_position;

      //��������� ���� ��� ���������
    if (curled_page->HasBottomSideBendPosition () || curled_page->HasTopSideBendPosition ())
    {
      math::vec3f bottom_side_bend_position = curled_page->HasBottomSideBendPosition () ? curled_page->GetBottomSideBendPosition () : bottom_corner_position;
      math::vec3f top_side_bend_position    = curled_page->HasTopSideBendPosition () ? curled_page->GetTopSideBendPosition () : top_corner_position;
      math::vec3f side_bend_position        = left_side ? curled_page->GetLeftSideBendPosition () : curled_page->GetRightSideBendPosition ();

      math::vec2f top_shadow_corner_position, bottom_shadow_corner_position;

      math::vec2f bottom_side_vector (bottom_corner_position.x - bottom_side_bend_position.x, bottom_corner_position.y - bottom_side_bend_position.y),
                  top_side_vector    (top_corner_position.x - top_side_bend_position.x, top_side_bend_position.y - top_corner_position.y),
                  side_vector        (top_corner_position.x - bottom_corner_position.x, top_corner_position.y - bottom_corner_position.y);

      if (left_side)
      {
        bottom_side_vector.y *= -1;
        top_side_vector.y    *= -1;
      }

      if (fabs (top_corner_position.z) < curl_radius)
        side_vector = bottom_corner_position - side_bend_position;

      if (fabs (bottom_corner_position.z) < curl_radius)
        side_vector = top_corner_position - side_bend_position;

      math::vec2f top_corner_offset    (normalize (math::vec2f (-top_side_vector.x + top_side_vector.y, top_side_vector.y + top_side_vector.x))),
                  bottom_corner_offset (normalize (math::vec2f (-bottom_side_vector.x + bottom_side_vector.y, bottom_side_vector.y + bottom_side_vector.x)));

      if (top_corner_offset.x != top_corner_offset.x)
        top_corner_offset = 0;
      if (bottom_corner_offset.x != bottom_corner_offset.x)
        bottom_corner_offset = 0;

      float shadow_grow_power               = page_curl->ShadowGrowPower (),
            top_shadow_offset_multiplier    = pow (length (side_vector), shadow_grow_power) * pow (top_corner_position.z / (page_curl->CurlRadius () * 2), shadow_grow_power),
            bottom_shadow_offset_multiplier = pow (length (side_vector), shadow_grow_power) * pow (bottom_corner_position.z / (page_curl->CurlRadius () * 2), shadow_grow_power),
            shadow_corner_offset            = page_curl->CornerShadowOffset ();

      if (left_side)
      {
        top_corner_offset.x    = -fabs (top_corner_offset.x);
        bottom_corner_offset.x = -fabs (bottom_corner_offset.x);
      }
      else
      {
        top_corner_offset.x    = fabs (top_corner_offset.x);
        bottom_corner_offset.x = fabs (bottom_corner_offset.x);
        top_corner_offset.y    = -top_corner_offset.y;
        bottom_corner_offset.y = -bottom_corner_offset.y;
      }

      top_shadow_corner_position.x    = top_corner_position.x - top_corner_offset.x * shadow_corner_offset * top_shadow_offset_multiplier;
      top_shadow_corner_position.y    = top_corner_position.y + top_corner_offset.y * shadow_corner_offset * top_shadow_offset_multiplier;
      bottom_shadow_corner_position.x = bottom_corner_position.x - bottom_corner_offset.x * shadow_corner_offset * bottom_shadow_offset_multiplier;
      bottom_shadow_corner_position.y = bottom_corner_position.y - bottom_corner_offset.y * shadow_corner_offset * bottom_shadow_offset_multiplier;

      math::vec2f bottom_curl_position = curled_page->HasBottomSideBendPosition () ? curled_page->GetBottomSideBendPosition () : side_bend_position;
      math::vec2f top_curl_position    = curled_page->HasTopSideBendPosition () ? curled_page->GetTopSideBendPosition () : side_bend_position;
      math::vec2f binding_vector       = math::normalize (top_curl_position - bottom_curl_position);

      top_side_bend_position    += binding_vector * shadow_corner_offset;
      bottom_side_bend_position -= binding_vector * shadow_corner_offset;

      if (fabs (top_corner_position.z) < curl_radius)
      {
        top_shadow_corner_position = side_bend_position;
        top_side_bend_position     = side_bend_position;
      }
      if (fabs (bottom_corner_position.z) < curl_radius)
      {
        bottom_shadow_corner_position = side_bend_position;
        bottom_side_bend_position     = side_bend_position;
      }

      RenderableVertex vertices [4];

      for (size_t i = 0; i < 4; i++)
      {
        vertices [i].color.x = 0;
        vertices [i].color.y = 0;
        vertices [i].color.z = 0;
        vertices [i].color.w = 255;
      }

      vertices [0].position = math::vec3f (bottom_shadow_corner_position.x + x_offset, bottom_shadow_corner_position.y, curl_radius);
      vertices [1].position = math::vec3f (bottom_side_bend_position.x + x_offset, bottom_side_bend_position.y, curl_radius);
      vertices [2].position = math::vec3f (top_shadow_corner_position.x + x_offset, top_shadow_corner_position.y, curl_radius);
      vertices [3].position = math::vec3f (top_side_bend_position.x + x_offset, top_side_bend_position.y, curl_radius);
      vertices [0].texcoord = math::vec2f (0, 0);
      vertices [1].texcoord = math::vec2f (length (bottom_side_vector) / page_size.x, 0);
      vertices [2].texcoord = math::vec2f (0, 1);
      vertices [3].texcoord = math::vec2f (length (top_side_vector) / page_size.x, 1);

      if (fabs (top_corner_position.z) < curl_radius)
        vertices [2].texcoord = math::vec2f (0, length (side_vector) / page_size.y);

      if (fabs (bottom_corner_position.z) < curl_radius)
        vertices [1].texcoord = math::vec2f (length (bottom_side_vector) / page_size.x, 1 - length (side_vector) / page_size.y);

      quad_vertex_buffer->SetData (0, sizeof (vertices), vertices);

      device.ISSetVertexBuffer (0, quad_vertex_buffer.get ());

      device.DrawIndexed (low_level::PrimitiveType_TriangleList, 0, 6, 0);
    }

      //��������� ���� �� ���������
    if (fabs (left_top_corner_position.z) > EPS || fabs (right_top_corner_position.z) > EPS || fabs (left_bottom_corner_position.z) > EPS || fabs (right_bottom_corner_position.z) > EPS)
    {
      unsigned char light = (unsigned char)((1 - page_curl->ShadowDensity () * curl_radius / page_curl->CurlRadius ()) * 255);

      RenderableVertex vertices [4];

      vertices [0].color = math::vec4ub (light, light, light, 255);
      vertices [1].color = math::vec4ub (light, light, light, 255);
      vertices [2].color = math::vec4ub (light, light, light, 255);
      vertices [3].color = math::vec4ub (light, light, light, 255);

      bool               has_side_detach_position        = left_side ? curled_page->HasLeftSideDetachPosition () : curled_page->HasRightSideDetachPosition ();
      bool               has_bottom_side_detach_position = curled_page->HasBottomSideDetachPosition ();
      bool               has_top_side_detach_position    = curled_page->HasTopSideDetachPosition ();
      const math::vec3f& side_detach_position            = left_side ? curled_page->GetLeftSideDetachPosition () : curled_page->GetRightSideDetachPosition ();
      math::vec3f        bottom_detach_position;
      math::vec3f        top_detach_position;

      if (has_bottom_side_detach_position)
      {
        bottom_detach_position = curled_page->GetBottomSideDetachPosition ();
      }
      else
      {
        if (has_side_detach_position)
          bottom_detach_position = side_detach_position;
        else
        {
          if (fabs (left_bottom_corner_position.z) > fabs (right_bottom_corner_position.z))
            bottom_detach_position = right_bottom_corner_position;
          else
            bottom_detach_position = left_bottom_corner_position;
        }
      }

      if (has_top_side_detach_position)
      {
        top_detach_position = curled_page->GetTopSideDetachPosition ();
      }
      else
      {
        if (has_side_detach_position)
          top_detach_position = side_detach_position;
        else
        {
          if (fabs (left_top_corner_position.z) > fabs (right_top_corner_position.z))
            top_detach_position = right_top_corner_position;
          else
            top_detach_position = left_top_corner_position;
        }
      }

      if (!has_bottom_side_detach_position && !has_top_side_detach_position)
      {
        if (fabs (left_top_corner_position.z) > 0 && fabs (right_top_corner_position.z) > 0)
        {
          if (fabs (left_top_corner_position.z) > fabs (right_top_corner_position.z))
            top_detach_position = right_top_corner_position;
          else
            top_detach_position = left_top_corner_position;
        }
        if (fabs (left_bottom_corner_position.z) > 0 && fabs (right_bottom_corner_position.z) > 0)
        {
          if (fabs (left_bottom_corner_position.z) > fabs (right_bottom_corner_position.z))
            bottom_detach_position = right_bottom_corner_position;
          else
            bottom_detach_position = left_bottom_corner_position;
        }
      }

      math::vec2f detach_vec = normalize (math::vec2f (bottom_detach_position.x - top_detach_position.x, bottom_detach_position.y - top_detach_position.y));

      bottom_detach_position += detach_vec * page_curl->CornerShadowOffset ();
      top_detach_position    -= detach_vec * page_curl->CornerShadowOffset ();

      math::vec2f shadow_offset = normalize (math::vec2f (-top_detach_position.y + bottom_detach_position.y, -bottom_detach_position.x + top_detach_position.x));

      if (!left_side)
        shadow_offset *= -1;

      float shadow_width = page_size.x * page_curl->ShadowWidth () * curl_radius / page_curl->CurlRadius ();

      vertices [0].position = math::vec3f (top_detach_position.x + x_offset,                                     top_detach_position.y,                                     BACK_SHADOW_OFFSET);
      vertices [1].position = math::vec3f (bottom_detach_position.x + x_offset,                                  bottom_detach_position.y,                                  BACK_SHADOW_OFFSET);
      vertices [2].position = math::vec3f (top_detach_position.x + shadow_offset.x * shadow_width + x_offset,    top_detach_position.y + shadow_offset.y * shadow_width,    BACK_SHADOW_OFFSET);
      vertices [3].position = math::vec3f (bottom_detach_position.x + shadow_offset.x * shadow_width + x_offset, bottom_detach_position.y + shadow_offset.y * shadow_width, BACK_SHADOW_OFFSET);
      vertices [0].texcoord = math::vec2f (0, 0);
      vertices [1].texcoord = math::vec2f (0, 0);
      vertices [2].texcoord = math::vec2f (1, 1);
      vertices [3].texcoord = math::vec2f (1, 1);

      quad_vertex_buffer->SetData (0, sizeof (vertices), vertices);

      math::vec3f left_bottom_corner_screen = (math::vec3f (-page_curl->Size ().x / 2, -page_curl->Size ().y / 2, 0) * page_curl->WorldTM () - view_point) * projection,
                  right_top_corner_screen   = (math::vec3f (page_curl->Size ().x / 2, page_curl->Size ().y / 2, 0) * page_curl->WorldTM () - view_point) * projection;

      low_level::Rect scissor_rect;

      scissor_rect.x      = ceil (viewport.x + (left_bottom_corner_screen.x + 1) / 2 * viewport.width);
      scissor_rect.y      = ceil (viewport.y + (left_bottom_corner_screen.y + 1) / 2 * viewport.height);
      scissor_rect.width  = floor ((right_top_corner_screen.x - left_bottom_corner_screen.x) / 2 * viewport.width);
      scissor_rect.height = floor ((right_top_corner_screen.y - left_bottom_corner_screen.y) / 2 * viewport.height);

      device.RSSetState (rasterizer_scissor_enabled_state.get ());
      device.RSSetScissor (scissor_rect);

      device.DrawIndexed (low_level::PrimitiveType_TriangleList, 0, 6, 0);
    }
  }

  //��������� ����������� ��� ������� ����
  void DrawLeftTopCornerFlip (low_level::IDevice& device)
  {
    if (page_curl->Mode () == PageCurlMode_SinglePage)
      throw xtl::format_operation_exception ("render::obsolete::render2d::RenderablePageCurl::DrawLeftBottomCornerFlip", "Can't draw flip for left bottom corner in single page mode");

    math::vec2f page_size       = page_curl->Size ();
    math::vec2f corner_position = page_curl->CornerPosition ();

    page_size.x /= 2;

    float x_flip_angle = -stl::max (PI - 2 * atan2 (page_size.y - corner_position.y, corner_position.x),
                                    atan2 (page_size.y - corner_position.y, page_size.x - corner_position.x));

    math::vec2f flip_vec ((page_size.y - corner_position.y) / tan (fabs (x_flip_angle)), corner_position.y - page_size.y);

    float flip_vec_length = math::length (flip_vec),
          curl_radius     = page_curl->CurlRadius ();

    if (flip_vec_length > page_size.x)
    {
      corner_position.x += (flip_vec_length - page_size.x) * flip_vec.x / flip_vec_length;
      corner_position.y -= (flip_vec_length - page_size.x) * flip_vec.y / flip_vec_length;
    }

    math::vec2f curl_corner_position = corner_position;

    curl_corner_position.y = page_size.y - curl_corner_position.y;

    if (curl_corner_position.x > 2 * page_size.x - curl_radius * 2)
      curl_radius *= (2 * page_size.x - curl_corner_position.x) / (curl_radius * 2);

    curl_radius = stl::max (EPS, curl_radius);

    float flip_width  = (page_size.y - corner_position.y) / tan (fabs (x_flip_angle)) + corner_position.x,
          flip_height = flip_width * tan (fabs (x_flip_angle / 2.f)),
          curl_angle  = -atan2 (flip_width, flip_height);

    if (curl_corner_position.y > 0)
      curl_angle += PI;

    float curl_x = (page_size.x - flip_width) * cos (curl_angle);

    curled_page->Curl (curl_corner_position, page_curl->CurlCorner (), curl_x, curl_radius, curl_angle,
                       page_curl->FindBestCurlSteps (), page_curl->BindingMismatchWeight ());

    device.RSSetState (rasterizer_cull_back_state.get ());

    scene_graph::PageCurlPageType curled_right_page_type = GetCurledRightPageType ();

    BindMaterial (device, page_materials [curled_right_page_type].get (), page_textures [curled_right_page_type].get ());

    float min_s, max_s;

    GetTexCoords (false, min_s, max_s);

    curled_page->SetTexCoords (max_s, 0, min_s, 1);

    curled_page->CalculateShadow (true);

    curled_page->Draw (device);

    device.RSSetState (rasterizer_cull_front_state.get ());

    scene_graph::PageCurlPageType curled_left_page_type = GetCurledLeftPageType ();

    BindMaterial (device, page_materials [curled_left_page_type].get (), page_textures [curled_left_page_type].get ());

    curled_page->CalculateShadow (false);

    GetTexCoords (true, min_s, max_s);

    curled_page->SetTexCoords (min_s, 0, max_s, 1);

    curled_page->Draw (device);

    device.RSSetState (rasterizer_no_cull_state.get ());

    DrawStaticPages (device);

    DrawShadows (device, curl_radius, true);
  }

  void DrawLeftBottomCornerFlip (low_level::IDevice& device)
  {
    if (page_curl->Mode () == PageCurlMode_SinglePage)
      throw xtl::format_operation_exception ("render::obsolete::render2d::RenderablePageCurl::DrawLeftBottomCornerFlip", "Can't draw flip for left bottom corner in single page mode");

    math::vec2f page_size       = page_curl->Size ();
    math::vec2f corner_position = page_curl->CornerPosition ();

    page_size.x /= 2;

    float x_flip_angle = stl::max (PI - 2 * atan2 (corner_position.y, corner_position.x),
                                   atan2 (corner_position.y, page_size.x - corner_position.x));

    math::vec2f flip_vec (corner_position.y / tan (x_flip_angle), corner_position.y);

    float flip_vec_length = math::length (flip_vec),
          curl_radius     = page_curl->CurlRadius ();

    if (flip_vec_length > page_size.x)
    {
      corner_position.x += (flip_vec_length - page_size.x) * flip_vec.x / flip_vec_length;
      corner_position.y -= (flip_vec_length - page_size.x) * flip_vec.y / flip_vec_length;
    }

    math::vec2f curl_corner_position = corner_position;

    curl_corner_position.y = page_size.y - curl_corner_position.y;

    if (curl_corner_position.x > 2 * page_size.x - curl_radius * 2)
      curl_radius *= (2 * page_size.x - curl_corner_position.x) / (curl_radius * 2);

    curl_radius = stl::max (EPS, curl_radius);

    float flip_width  = corner_position.y / tan (x_flip_angle) + corner_position.x,
          flip_height = flip_width * tan (x_flip_angle / 2.f),
          curl_angle  = atan2 (flip_width, flip_height);

    if (curl_corner_position.y < page_size.y)
      curl_angle += PI;

    float curl_x = (page_size.x - flip_width) * cos (curl_angle);

    curled_page->Curl (curl_corner_position, page_curl->CurlCorner (), curl_x, curl_radius, curl_angle,
                       page_curl->FindBestCurlSteps (), page_curl->BindingMismatchWeight ());

    device.RSSetState (rasterizer_cull_back_state.get ());

    scene_graph::PageCurlPageType curled_right_page_type = GetCurledRightPageType ();

    BindMaterial (device, page_materials [curled_right_page_type].get (), page_textures [curled_right_page_type].get ());

    float min_s, max_s;

    GetTexCoords (false, min_s, max_s);

    curled_page->SetTexCoords (max_s, 0, min_s, 1);

    curled_page->CalculateShadow (true);

    curled_page->Draw (device);

    device.RSSetState (rasterizer_cull_front_state.get ());

    scene_graph::PageCurlPageType curled_left_page_type = GetCurledLeftPageType ();

    BindMaterial (device, page_materials [curled_left_page_type].get (), page_textures [curled_left_page_type].get ());

    curled_page->CalculateShadow (false);

    GetTexCoords (true, min_s, max_s);

    curled_page->SetTexCoords (min_s, 0, max_s, 1);

    curled_page->Draw (device);

    device.RSSetState (rasterizer_no_cull_state.get ());

    DrawStaticPages (device);

    DrawShadows (device, curl_radius, true);
  }

  void DrawRightTopCornerFlip (low_level::IDevice& device)
  {
    const math::vec2f& total_size      = page_curl->Size ();
    math::vec2f        page_size       = total_size;
    math::vec2f        corner_position = page_curl->CornerPosition ();

    if (page_curl->Mode () != PageCurlMode_SinglePage)
      page_size.x /= 2;

    float left_page_width = page_curl->Mode () == PageCurlMode_SinglePage ? 0 : page_size.x;

    float x_flip_angle = -stl::max (PI - 2 * atan2 (page_size.y - corner_position.y, total_size.x - corner_position.x),
                                    atan2 (page_size.y - corner_position.y, corner_position.x - left_page_width));

    math::vec2f flip_vec ((page_size.y - corner_position.y) / tan (fabs (x_flip_angle)), corner_position.y - page_size.y);

    float flip_vec_length = math::length (flip_vec),
          curl_radius     = page_curl->CurlRadius ();

    if (flip_vec_length > page_size.x)
    {
      corner_position.x -= (flip_vec_length - page_size.x) * flip_vec.x / flip_vec_length;
      corner_position.y -= (flip_vec_length - page_size.x) * flip_vec.y / flip_vec_length;
    }

    if (page_curl->Mode () != PageCurlMode_SinglePage)
      corner_position.x -= page_size.x;

    math::vec2f curl_corner_position = corner_position;

    curl_corner_position.y = page_size.y - curl_corner_position.y;

    if (curl_corner_position.x < -page_size.x + curl_radius * 2)
      curl_radius *= (-page_size.x - curl_corner_position.x) / -(curl_radius * 2);

    curl_radius = stl::max (EPS, curl_radius);

    float distance_to_right_edge = total_size.x - corner_position.x,
          flip_width             = (page_size.y - corner_position.y) / tan (fabs (x_flip_angle)) + distance_to_right_edge,
          flip_height            = flip_width * tan (fabs (x_flip_angle / 2.f)),
          curl_angle             = atan2 (flip_width, flip_height);

    if (curl_corner_position.y <= 0)
      curl_angle += PI;

    float curl_x = (page_size.x - flip_width) * cos (curl_angle);

    curled_page->Curl (curl_corner_position, page_curl->CurlCorner (), curl_x, curl_radius, curl_angle,
                       page_curl->FindBestCurlSteps (), page_curl->BindingMismatchWeight ());

    if (page_curl->Mode () != PageCurlMode_SinglePage)
    {
      ProgramParameters program_parameters;

      program_parameters.view_matrix       = math::translate (-view_point);
      program_parameters.projection_matrix = projection;
      program_parameters.object_matrix     = page_curl->WorldTM () * math::scale (math::vec3f (1, 1, -1)) * math::translate (math::vec3f (page_size.x, 0, 0)) * math::translate (math::vec3f (-page_curl->Size ().x / 2, -page_curl->Size ().y / 2, 0));

      constant_buffer->SetData (0, sizeof (program_parameters), &program_parameters);
    }

    device.RSSetState (rasterizer_cull_back_state.get ());

    scene_graph::PageCurlPageType curled_left_page_type = GetCurledLeftPageType ();

    BindMaterial (device, page_materials [curled_left_page_type].get (), page_textures [curled_left_page_type].get ());

    float min_s, max_s;

    GetTexCoords (true, min_s, max_s);

    if (page_curl->Mode () != PageCurlMode_SinglePage)
      curled_page->SetTexCoords (max_s, 0, min_s, 1);
    else
      curled_page->SetTexCoords (min_s, 0, max_s, 1);

    curled_page->CalculateShadow (true);

    curled_page->Draw (device);

    device.RSSetState (rasterizer_cull_front_state.get ());

    scene_graph::PageCurlPageType curled_right_page_type = GetCurledRightPageType ();

    BindMaterial (device, page_materials [curled_right_page_type].get (), page_textures [curled_right_page_type].get ());

    curled_page->CalculateShadow (false);

    GetTexCoords (false, min_s, max_s);

    curled_page->SetTexCoords (min_s, 0, max_s, 1);

    curled_page->Draw (device);

    device.RSSetState (rasterizer_no_cull_state.get ());

    if (page_curl->Mode () != PageCurlMode_SinglePage)
    {
      ProgramParameters program_parameters;

      program_parameters.view_matrix       = math::translate (-view_point);
      program_parameters.projection_matrix = projection;
      program_parameters.object_matrix     = page_curl->WorldTM () * math::scale (math::vec3f (1, 1, -1)) * math::translate (math::vec3f (-page_curl->Size ().x / 2, -page_curl->Size ().y / 2, 0));

      constant_buffer->SetData (0, sizeof (program_parameters), &program_parameters);
    }

    DrawStaticPages (device);

    DrawShadows (device, curl_radius, false);
  }

  void DrawRightBottomCornerFlip (low_level::IDevice& device)
  {
    const math::vec2f& total_size      = page_curl->Size ();
    math::vec2f        page_size       = total_size;
    math::vec2f        corner_position = page_curl->CornerPosition ();

    if (page_curl->Mode () != PageCurlMode_SinglePage)
      page_size.x /= 2;

    float left_page_width = page_curl->Mode () == PageCurlMode_SinglePage ? 0 : page_size.x;

    float x_flip_angle = stl::max (PI - 2 * atan2 (corner_position.y, total_size.x - corner_position.x),
                                    atan2 (corner_position.y, corner_position.x - left_page_width));

    math::vec2f flip_vec (corner_position.y / tan (x_flip_angle), corner_position.y);

    float flip_vec_length = math::length (flip_vec),
          curl_radius     = page_curl->CurlRadius ();

    if (flip_vec_length > page_size.x)
    {
      corner_position.x -= (flip_vec_length - page_size.x) * flip_vec.x / flip_vec_length;
      corner_position.y -= (flip_vec_length - page_size.x) * flip_vec.y / flip_vec_length;
    }

    if (page_curl->Mode () != PageCurlMode_SinglePage)
      corner_position.x -= page_size.x;

    math::vec2f curl_corner_position = corner_position;

    curl_corner_position.y = page_size.y - curl_corner_position.y;

    if (curl_corner_position.x < -page_size.x + curl_radius * 2)
      curl_radius *= (-page_size.x - curl_corner_position.x) / -(curl_radius * 2);

    curl_radius = stl::max (EPS, curl_radius);

    float distance_to_right_edge = total_size.x - corner_position.x,
          flip_width             = corner_position.y / tan (x_flip_angle) + distance_to_right_edge,
          flip_height            = flip_width * tan (x_flip_angle / 2.f),
          curl_angle             = -atan2 (flip_width, flip_height);

    if (curl_corner_position.y >= page_size.y)
      curl_angle -= PI;

    float curl_x = (page_size.x - flip_width) * cos (curl_angle);

    curled_page->Curl (curl_corner_position, page_curl->CurlCorner (), curl_x, curl_radius, curl_angle,
                       page_curl->FindBestCurlSteps (), page_curl->BindingMismatchWeight ());

    if (page_curl->Mode () != PageCurlMode_SinglePage)
    {
      ProgramParameters program_parameters;

      program_parameters.view_matrix       = math::translate (-view_point);
      program_parameters.projection_matrix = projection;
      program_parameters.object_matrix     = page_curl->WorldTM () * math::scale (math::vec3f (1, 1, -1)) * math::translate (math::vec3f (page_size.x, 0, 0)) * math::translate (math::vec3f (-page_curl->Size ().x / 2, -page_curl->Size ().y / 2, 0));

      constant_buffer->SetData (0, sizeof (program_parameters), &program_parameters);
    }

    device.RSSetState (rasterizer_cull_back_state.get ());

    scene_graph::PageCurlPageType curled_left_page_type = GetCurledLeftPageType ();

    BindMaterial (device, page_materials [curled_left_page_type].get (), page_textures [curled_left_page_type].get ());

    float min_s, max_s;

    GetTexCoords (true, min_s, max_s);

    if (page_curl->Mode () != PageCurlMode_SinglePage)
      curled_page->SetTexCoords (max_s, 0, min_s, 1);
    else
      curled_page->SetTexCoords (min_s, 0, max_s, 1);

    curled_page->CalculateShadow (true);

    curled_page->Draw (device);

    device.RSSetState (rasterizer_cull_front_state.get ());

    scene_graph::PageCurlPageType curled_right_page_type = GetCurledRightPageType ();

    BindMaterial (device, page_materials [curled_right_page_type].get (), page_textures [curled_right_page_type].get ());

    curled_page->CalculateShadow (false);

    GetTexCoords (false, min_s, max_s);

    curled_page->SetTexCoords (min_s, 0, max_s, 1);

    curled_page->Draw (device);

    device.RSSetState (rasterizer_no_cull_state.get ());

    if (page_curl->Mode () != PageCurlMode_SinglePage)
    {
      ProgramParameters program_parameters;

      program_parameters.view_matrix       = math::translate (-view_point);
      program_parameters.projection_matrix = projection;
      program_parameters.object_matrix     = page_curl->WorldTM () * math::scale (math::vec3f (1, 1, -1)) * math::translate (math::vec3f (-page_curl->Size ().x / 2, -page_curl->Size ().y / 2, 0));

      constant_buffer->SetData (0, sizeof (program_parameters), &program_parameters);
    }

    DrawStaticPages (device);

    DrawShadows (device, curl_radius, false);
  }

  //��������� ������� �������
  void DrawStaticPages (low_level::IDevice& device)
  {
    const math::vec2f& size = page_curl->Size ();

    int   left_page_type   = -1,
          right_page_type  = -1;
    float left_page_width  = size.x * 0.5f,
          right_page_width = left_page_width;

    device.ISSetIndexBuffer  (quad_index_buffer.get ());
    device.ISSetVertexBuffer (0, quad_vertex_buffer.get ());

    switch (page_curl->Mode ())
    {
      case PageCurlMode_SinglePage:
        right_page_type  = PageCurlPageType_Front;
        left_page_width  = 0.f;
        right_page_width = size.x;
        break;
      case PageCurlMode_DoublePageSingleMaterial:
        left_page_type  = PageCurlPageType_Back;
        right_page_type = PageCurlPageType_Front;
        break;
      case PageCurlMode_DoublePageDoubleMaterial:
        left_page_type  = PageCurlPageType_BackLeft;
        right_page_type = PageCurlPageType_FrontRight;
        break;
    }

    math::vec4ub page_color (GetPageColor ());

    if (left_page_type >= 0)
    {
      RenderableVertex vertices [4];

      for (size_t i = 0; i < 4; i++)
        vertices [i].color = page_color;

      float min_s, max_s;

      GetTexCoords (true, min_s, max_s);

      vertices [0].position = math::vec3f (0, 0, STATIC_PAGES_Z_OFFSET);
      vertices [1].position = math::vec3f (left_page_width, 0, STATIC_PAGES_Z_OFFSET);
      vertices [2].position = math::vec3f (0, size.y, STATIC_PAGES_Z_OFFSET);
      vertices [3].position = math::vec3f (left_page_width, size.y, STATIC_PAGES_Z_OFFSET);
      vertices [0].texcoord = math::vec2f (min_s, 0);
      vertices [1].texcoord = math::vec2f (max_s, 0);
      vertices [2].texcoord = math::vec2f (min_s, 1.f);
      vertices [3].texcoord = math::vec2f (max_s, 1.f);

      quad_vertex_buffer->SetData (0, sizeof (vertices), vertices);

      BindMaterial (device, page_materials [left_page_type].get (), page_textures [left_page_type].get ());

      device.DrawIndexed (low_level::PrimitiveType_TriangleList, 0, 6, 0);
    }

    if (right_page_type >= 0)
    {
      RenderableVertex vertices [4];

      for (size_t i = 0; i < 4; i++)
        vertices [i].color = page_color;

      float min_s, max_s;

      GetTexCoords (false, min_s, max_s);

      vertices [0].position = math::vec3f (left_page_width, 0, STATIC_PAGES_Z_OFFSET);
      vertices [1].position = math::vec3f (left_page_width + right_page_width, 0, STATIC_PAGES_Z_OFFSET);
      vertices [2].position = math::vec3f (left_page_width, size.y, STATIC_PAGES_Z_OFFSET);
      vertices [3].position = math::vec3f (left_page_width + right_page_width, size.y, STATIC_PAGES_Z_OFFSET);
      vertices [0].texcoord = math::vec2f (min_s, 0);
      vertices [1].texcoord = math::vec2f (max_s, 0);
      vertices [2].texcoord = math::vec2f (min_s, 1.f);
      vertices [3].texcoord = math::vec2f (max_s, 1.f);

      quad_vertex_buffer->SetData (0, sizeof (vertices), vertices);

      BindMaterial (device, page_materials [right_page_type].get (), page_textures [right_page_type].get ());

      device.DrawIndexed (low_level::PrimitiveType_TriangleList, 0, 6, 0);
    }
  }

  void BindMaterial (low_level::IDevice& device, SpriteMaterial* material, ILowLevelTexture* texture)
  {
    static const char* METHOD_NAME = "render::obsolete::render2d::RenderablePageCurl::BindMaterial";

    if (!material)
      throw xtl::make_null_argument_exception (METHOD_NAME, "material");

    if (!texture)
      throw xtl::make_null_argument_exception (METHOD_NAME, "texture");

      //��������� ��������� �� ����

    BlendStatePtr material_blend_state;

    switch (material->BlendMode ())
    {
      case media::rfx::obsolete::SpriteBlendMode_None:
        material_blend_state = none_blend_state;
        break;
      case media::rfx::obsolete::SpriteBlendMode_Translucent:
        material_blend_state = translucent_blend_state;
        break;
      case media::rfx::obsolete::SpriteBlendMode_Mask:
        material_blend_state = mask_blend_state;
        break;
      case media::rfx::obsolete::SpriteBlendMode_Additive:
        material_blend_state = additive_blend_state;
        break;
      default:
        throw xtl::format_operation_exception (METHOD_NAME, "Unsupported material '%s' blend mode, only 'none', 'translucent', 'mask' and 'additive' page material blend mode supported", material->Name ());
    }

    device.OSSetBlendState (material_blend_state.get ());
    device.SSSetTexture    (0, texture->GetTexture ());
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
   ����������
*/

void RenderablePageCurl::Update ()
{
  impl->Update ();
}

/*
    ���������
*/

void RenderablePageCurl::DrawCore (render::obsolete::render2d::IFrame& frame)
{
  frame.GetViewPoint  (impl->view_point);
  frame.GetProjection (impl->projection);
  frame.GetViewport   (impl->viewport);

  impl->low_level_frame->SetRenderTargets (frame.GetRenderTarget (), frame.GetDepthStencilTarget ());
  impl->low_level_frame->SetViewport (impl->viewport);

  impl->render.Renderer ()->AddFrame (impl->low_level_frame.get ());
}
