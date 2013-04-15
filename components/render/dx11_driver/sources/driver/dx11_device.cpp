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

    check_errors ("::D3D11CreateDevice", D3D11CreateDevice (adapter->GetModule () ? (IDXGIAdapter*)0 : &adapter->GetHandle (), 
      adapter->GetModule () ? D3D_DRIVER_TYPE_SOFTWARE : D3D_DRIVER_TYPE_UNKNOWN,
      adapter->GetModule (), 0, feature_levels_requested, feature_levels_requested_count, D3D11_SDK_VERSION, &dx_device, &feature_level, &dx_context));

    if (!dx_device || !dx_context)
      throw xtl::format_operation_exception ("", "::D3D11CreateDevice failed");

    device = DxDevicePtr (dx_device, false);

    device_manager.reset (new DeviceManager (device));

    immediate_context = ContextPtr (new Context (DxContextPtr (dx_context, false), *device_manager), false);

      //������������� ����������

    render_target_manager.reset (new RenderTargetManager (*device_manager));
    texture_manager.reset       (new TextureManager (*device_manager));
    input_manager.reset         (new InputManager (*device_manager));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Device::Device");
    throw;
  }
}

Device::~Device ()
{
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
        return "hlsl.vs hlsl.ps hlsl";
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
  throw xtl::make_not_implemented_exception ("render::low_level::dx11::Device::CreateQuery");
}

/*
    ���������� ����������� ���������
*/

IPredicate* Device::CreatePredicate ()
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
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
    throw xtl::make_not_implemented_exception (__FUNCTION__);
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
    return input_manager->CreateBuffer (desc);
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
    throw xtl::make_not_implemented_exception (__FUNCTION__);
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
    throw xtl::make_not_implemented_exception (__FUNCTION__);
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
    throw xtl::make_not_implemented_exception (__FUNCTION__);
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
    throw xtl::make_not_implemented_exception (__FUNCTION__);
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
    throw xtl::make_not_implemented_exception (__FUNCTION__);
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
    ID3D11DeviceContext* context = 0;

    check_errors ("ID3D11Device::CreateDeferredContext", device->CreateDeferredContext (0, &context));    

    if (!context)
      throw xtl::format_operation_exception ("", "ID3D11Device::CreateDeferredContext failed");

    return new Context (DxContextPtr (context, false), *device_manager);
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
