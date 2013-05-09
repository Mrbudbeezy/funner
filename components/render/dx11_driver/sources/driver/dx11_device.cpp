#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    ����������� / ����������
*/

Device::Device (const AdapterPtr& in_adapter, const char* init_string)
{
  try
  {
      //�������� ������������ ����������

    if (!init_string)
      init_string = "";

    if (!in_adapter)
      throw xtl::make_null_argument_exception ("", "adapter");

      //������ ������ �������������

    common::PropertyMap init_properties = common::parse_init_string (init_string);

    bool is_debug = init_properties.IsPresent ("debug") && init_properties.GetInteger ("debug");

      //���������� ����������

    adapter = in_adapter;

      //�������� ����������

    ID3D11Device*        dx_device = 0;
    ID3D11DeviceContext* dx_context = 0;
    D3D_FEATURE_LEVEL    feature_level;

    static const D3D_FEATURE_LEVEL feature_levels_requested [] = {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0,
      D3D_FEATURE_LEVEL_9_3,
      D3D_FEATURE_LEVEL_9_2,
      D3D_FEATURE_LEVEL_9_1,
    };

    static const size_t feature_levels_requested_count = sizeof (feature_levels_requested) / sizeof (*feature_levels_requested);

    UINT flags = 0;

    if (is_debug) flags |= D3D11_CREATE_DEVICE_DEBUG;

    check_errors ("::D3D11CreateDevice", D3D11CreateDevice (adapter->GetModule () ? (IDXGIAdapter*)0 : &adapter->GetHandle (), 
      adapter->GetModule () ? D3D_DRIVER_TYPE_SOFTWARE : D3D_DRIVER_TYPE_UNKNOWN,
      adapter->GetModule (), flags, feature_levels_requested, feature_levels_requested_count, D3D11_SDK_VERSION, &dx_device, &feature_level, &dx_context));

    if (!dx_device || !dx_context)
      throw xtl::format_operation_exception ("", "::D3D11CreateDevice failed");

    device = DxDevicePtr (dx_device, false);
    
    DxContextPtr dx_immediate_context (dx_context, false);

    device_manager.reset (new DeviceManager (device));

      //������������� ����������

    render_target_manager.reset (new RenderTargetManager (*device_manager));
    texture_manager.reset       (new TextureManager (*device_manager));
    input_manager.reset         (new InputManager (*device_manager));
    shader_manager.reset        (new ShaderManager (*device_manager));
    output_manager.reset        (new OutputManager (*device_manager));
    query_manager.reset         (new QueryManager (*device_manager));

      //������������� �������� �� ���������

    InitDefaults ();

      //�������� ���������

    immediate_context = ContextPtr (new Context (dx_immediate_context, *device_manager, shader_manager->GetShaderLibrary (), initial_resources, default_resources), false);      

      //�������� ����������� ������

    if (is_debug)
      StartDebugLayer ();

      //���������� �������

    properties.AddProperty ("init_string", init_string);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Device::Device");
    throw;
  }
}

Device::~Device ()
{
  StopDebugLayer ();
}

/*
    ������������� �������� �� ���������
*/

void Device::InitDefaults ()
{
  try
  {    
      //�������� �������� ������� �� ���������

    InputLayoutDesc default_layout_desc;
    
    memset (&default_layout_desc, 0 , sizeof default_layout_desc);

    VertexAttribute va [2];

    memset (&va, 0, sizeof (va));

    va [0].semantic = "POSITION";
    va [0].format   = InputDataFormat_Vector3;
    va [0].type     = InputDataType_Float;
    va [0].offset   = 0;
    va [0].stride   = sizeof (float) * 7;

    va [1].semantic = "COLOR";
    va [1].format   = InputDataFormat_Vector4;
    va [1].type     = InputDataType_Float;
    va [1].offset   = 0;
    va [1].stride   = sizeof (float) * 7;

    default_layout_desc.vertex_attributes       = &va [0];
    default_layout_desc.vertex_attributes_count = sizeof (va) / sizeof (*va);
    default_layout_desc.index_type              = InputDataType_UShort;

    default_resources.input_layout = InputLayoutPtr (input_manager->CreateInputLayout (default_layout_desc), false);
    initial_resources.input_layout = default_resources.input_layout;

      //�������� ��������� �� ���������

    ShaderDesc shader_descs [2];

    static const char* DEFAULT_VERTEX_SHADER = 
      "struct VS_INPUT   { float4 Position : POSITION; float4 Color : COLOR; };\n"
      "struct VS_OUTPUT  { float4 Position : SV_POSITION; float4 Color : COLOR; };\n"

      "VS_OUTPUT main (in VS_INPUT In) {\n"
      "  VS_OUTPUT Out;\n"
      "  Out.Position = In.Position;\n"
      "  Out.Color    = In.Color;\n"
      "  return Out;\n"
      "}\n";

    static const char* DEFAULT_PIXEL_SHADER = 
      "struct PS_INPUT { float4 Position : SV_POSITION; float4 Color : COLOR; };\n"
      "float4 main (PS_INPUT Input) : SV_TARGET {\n"
      "    return Input.Color;\n"
      "}\n";

    memset (&shader_descs, 0, sizeof (shader_descs));

    shader_descs [0].name             = "Default vertex shader";
    shader_descs [0].source_code_size = ~0;
    shader_descs [0].source_code      = DEFAULT_VERTEX_SHADER;
    shader_descs [0].profile          = "hlsl.vs";
    shader_descs [1].name             = "Default pixel shader";
    shader_descs [1].source_code_size = ~0;
    shader_descs [1].source_code      = DEFAULT_PIXEL_SHADER;
    shader_descs [1].profile          = "hlsl.ps";

    default_resources.program = IProgramPtr (CreateProgram (sizeof (shader_descs) / sizeof (*shader_descs), &shader_descs [0], LogFunction ()), false);    

      //������������� BlendState
       
    BlendDesc blend_desc;
    
    memset (&blend_desc, 0, sizeof (blend_desc));

    blend_desc.render_target [0].blend_enable     = false;
    blend_desc.render_target [0].color_write_mask = ColorWriteFlag_All;
    blend_desc.independent_blend_enable           = false;

    initial_resources.blend_state = IBlendStatePtr (CreateBlendState (blend_desc), false);
    
    blend_desc.render_target [0].color_write_mask = 0;
    
    default_resources.blend_state = IBlendStatePtr (CreateBlendState (blend_desc), false);

      //������������� DepthStencilState
      
    DepthStencilDesc depth_stencil_desc;
    
    memset (&depth_stencil_desc, 0, sizeof (depth_stencil_desc));
    
    depth_stencil_desc.depth_test_enable  = true;
    depth_stencil_desc.depth_write_enable = true;
    depth_stencil_desc.depth_compare_mode = CompareMode_Less;
    
    initial_resources.depth_stencil_state = IDepthStencilStatePtr (CreateDepthStencilState (depth_stencil_desc), false);
    
    depth_stencil_desc.depth_test_enable  = false;
    depth_stencil_desc.depth_write_enable = false;
    depth_stencil_desc.depth_compare_mode = CompareMode_AlwaysPass;
    
    default_resources.depth_stencil_state = IDepthStencilStatePtr (CreateDepthStencilState (depth_stencil_desc), false);

      //������������� ��������� ������������� �� ���������

    RasterizerDesc rasterizer_desc;

    memset (&rasterizer_desc, 0, sizeof rasterizer_desc);

    rasterizer_desc.fill_mode               = FillMode_Solid;
    rasterizer_desc.cull_mode               = CullMode_None;
    rasterizer_desc.front_counter_clockwise = true;
    rasterizer_desc.depth_bias              = 0;
    rasterizer_desc.scissor_enable          = false;
    rasterizer_desc.multisample_enable      = false;
    rasterizer_desc.antialiased_line_enable = false;

    initial_resources.rasterizer_state = IRasterizerStatePtr (CreateRasterizerState (rasterizer_desc), false);
    default_resources.rasterizer_state = initial_resources.rasterizer_state;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11:Device::InitDefaults");
    throw;
  }
}

void Device::InitRenderTargets (SwapChain& swap_chain)
{
  try
  {
    if (initial_resources.render_target_view || initial_resources.depth_stencil_view)
      return;

      //������������� �����������

    SwapChainDesc swap_chain_desc;

    swap_chain.GetDesc (swap_chain_desc);        

    xtl::com_ptr<ITexture> color_texture (CreateRenderTargetTexture (&swap_chain, 0), false),
                           depth_stencil_texture (CreateDepthStencilTexture (&swap_chain), false);

    ViewDesc view_desc;

    memset (&view_desc, 0, sizeof (view_desc));

    initial_resources.render_target_view = IViewPtr (CreateView (color_texture.get (), view_desc), false);
    initial_resources.depth_stencil_view = IViewPtr (CreateView (depth_stencil_texture.get (), view_desc), false);

      //������������� ������� ������

    Viewport default_viewport;

    memset (&default_viewport, 0, sizeof default_viewport);

    default_viewport.width     = swap_chain_desc.frame_buffer.width;
    default_viewport.height    = swap_chain_desc.frame_buffer.height;
    default_viewport.min_depth = 0.0f;
    default_viewport.max_depth = 1.0f;

    initial_resources.viewport = default_viewport;

      //������������� ������� ���������

    Rect default_scissor;      
    
    memset (&default_scissor, 0, sizeof default_scissor);      

    default_scissor.width  = swap_chain_desc.frame_buffer.width;
    default_scissor.height = swap_chain_desc.frame_buffer.height;        

    initial_resources.scissor = default_scissor;

    if (!immediate_context->RenderTargetsChanged ())
    {
      immediate_context->OSSetRenderTargetView (0, initial_resources.render_target_view.get ());
      immediate_context->OSSetDepthStencilView (initial_resources.depth_stencil_view.get ());      
      immediate_context->RSSetViewport         (0, initial_resources.viewport);
      immediate_context->RSSetScissor          (0, initial_resources.scissor);      
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11:Device::InitRenderTargets");
    throw;
  }
}

void Device::StartDebugLayer ()
{
  try
  {
    StopDebugLayer ();

    debug_layer.reset (new DeviceDebugLayer (device));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Device::StartDebugLayer");
    throw;
  }
}

void Device::StopDebugLayer  ()
{
  try
  {
    debug_layer.reset ();
  }
  catch (...)
  {
  }
}

/*
    ��������� ����������� ���������� / ��������
*/

ID3D11Device& Device::GetHandle ()
{
  return *device;
}

AdapterPtr Device::GetAdapter ()
{
  return adapter;
}

/*
    ��� ����������
*/

const char* Device::GetName ()
{
  return "render::low_level::dx11::Device";
}

/*
    ��������� ������������ ����������
*/

void Device::GetCaps (DeviceCaps& caps)
{
  try
  {
    memset (&caps, 0, sizeof (caps));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::GetCaps");
    throw;
  }
}

const char* Device::GetCapString (DeviceCapString cap_string)
{
  try
  {
    switch (cap_string)
    {
      case DeviceCapString_ShaderProfiles:
        return shader_manager->GetShaderProfilesString ();
      default:
        throw xtl::make_argument_exception ("", "cap_string", cap_string);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::GetCapString");
    throw;
  }
}

const char* Device::GetVertexAttributeSemanticName (VertexAttributeSemantic semantic)
{
  try
  {
    return InputManager::GetVertexAttributeSemanticName (semantic);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Device::GetVertexAttributeSemanticName");
    throw;
  }
}

/*
    ���������� ���������
*/

IQuery* Device::CreateQuery (QueryType type)
{
  try
  {
    return query_manager->CreateQuery (type);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateQuery");
    throw;
  }
}

/*
    ���������� ����������� ���������
*/

IPredicate* Device::CreatePredicate ()
{
  try
  {
    return query_manager->CreatePredicate ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreatePredicate");
    throw;
  }
}

/*
    ������ � ������� ���������
*/

IStateBlock* Device::CreateStateBlock (const StateBlockMask& mask)
{
  try
  {
    return new StateBlock (*device_manager, mask);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateStateBlock");
    throw;
  }
}

/*
    ���������� ������� ������� (input-stage)
*/

IInputLayout* Device::CreateInputLayout (const InputLayoutDesc& desc)
{
  try
  {
    return input_manager->CreateInputLayout (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateInputLayout");
    throw;
  }
}

IBuffer* Device::CreateBuffer (const BufferDesc& desc)
{
  static const char* METHOD_NAME = "render::low_level::dx11::Device::CreateBuffer";

  try
  {
    static const size_t BAD_FLAGS = BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil;

    if (desc.bind_flags & BAD_FLAGS)
      throw xtl::make_argument_exception ("", "desc.bind_flags", get_name ((BindFlag)desc.bind_flags));

    switch (desc.bind_flags)
    {
      case BindFlag_VertexBuffer:
      case BindFlag_IndexBuffer:    return input_manager->CreateBuffer (desc);
      case BindFlag_ConstantBuffer: return shader_manager->CreateConstantBuffer (desc);
      default:
        throw xtl::format_not_supported_exception ("", "Incompatible desc.bind_flags=%s", get_name ((BindFlag)desc.bind_flags));
    }
  }
  catch (xtl::exception& e)
  {
    e.touch (METHOD_NAME);
    throw;
  }
}

/*
    ���������� ���������� �������� (shader-stage)
*/

IProgramParametersLayout* Device::CreateProgramParametersLayout (const ProgramParametersLayoutDesc& desc)
{
  try
  {
    return shader_manager->CreateProgramParametersLayout (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateProgramParametersLayout");
    throw;
  }
}

IProgram* Device::CreateProgram (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log)
{
  try
  {
    return shader_manager->CreateProgram (shaders_count, shader_descs, error_log);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateProgram");
    throw;
  }
}

/*
    ���������� ����������
*/

ISamplerState* Device::CreateSamplerState (const SamplerDesc& desc)
{
  try
  {
    return texture_manager->CreateSamplerState (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateSamplerState");
    throw;
  }
}

ITexture* Device::CreateTexture (const TextureDesc& desc)
{
  return CreateTexture (desc, 0);
}

ITexture* Device::CreateTexture (const TextureDesc& desc, const TextureData& data)
{
  return CreateTexture (desc, &data);
}

ITexture* Device::CreateTexture (const TextureDesc& desc, const TextureData* data)
{
  try
  {
    if (!(desc.bind_flags & (BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil)))
      throw xtl::format_not_supported_exception ("", "Unsupported bindable flags desc.bind_flags=%s", get_name ((BindFlag)desc.bind_flags));

    return texture_manager->CreateTexture (desc, data);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Device::CreateTexture");
    throw;
  }
}

ITexture* Device::CreateTexture (const TextureDesc&, IBuffer* buffer, size_t buffer_offset, const size_t* mip_sizes)
{
  throw xtl::make_not_implemented_exception ("render::low_level::dx11::Device::CreateTexture");
}

/*
    ���������� �������������� (rasterizer-stage)
*/

IRasterizerState* Device::CreateRasterizerState (const RasterizerDesc& desc)
{
  try
  {
    return output_manager->CreateRasterizerState (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateRasterizerState");
    throw;
  }
}

/*
    ���������� �������� ������� (output-stage)
*/

IBlendState* Device::CreateBlendState (const BlendDesc& desc)
{
  try
  {
    return output_manager->CreateBlendState (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateBlendState");
    throw;
  }
}

IDepthStencilState* Device::CreateDepthStencilState (const DepthStencilDesc& desc)
{
  try
  {
    return output_manager->CreateDepthStencilState (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateDepthStencilState");
    throw;
  }
}

/*
    ���������� ���������� ������� ������� ���������
*/

ITexture* Device::CreateRenderTargetTexture (ISwapChain* swap_chain, size_t buffer_index)
{
  try
  {
    return render_target_manager->CreateRenderTargetTexture (swap_chain, buffer_index);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateRenderTargetTexture");
    throw;
  }
}

ITexture* Device::CreateDepthStencilTexture (ISwapChain* swap_chain)
{
  try
  {
    return render_target_manager->CreateDepthStencilTexture (swap_chain);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateDepthStencilTexture");
    throw;
  }
}

IView* Device::CreateView (ITexture* texture, const ViewDesc& desc)
{
  try
  {
    return render_target_manager->CreateView (texture, desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateView");
    throw;
  }
}

/*
    �������� ���������
*/

IDeviceContext* Device::CreateDeferredContext ()
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__); //can't be implemented only via DX deferred contexts (need MT guard)
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Device::CreateDeferredContext");
    throw;
  }
}

/*
    ��������� ����������������� ��������� ���������� ��������
*/

IDeviceContext* Device::GetImmediateContext ()
{
  return &*immediate_context;
}

/*
    ������ ������� ���������� ���������
*/

IPropertyList* Device::GetProperties ()
{
  return &properties;
}
