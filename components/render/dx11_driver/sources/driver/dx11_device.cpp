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

      //���������� ��������

    adapter = in_adapter;

      //�������� ����������

    ID3D11Device*        dx_device = 0;
    ID3D11DeviceContext* dx_context = 0;
    D3D_FEATURE_LEVEL    feature_level;

    check_errors ("::D3D11CreateDevice", D3D11CreateDevice (&adapter->DxAdapter (), adapter->GetModule () ? D3D_DRIVER_TYPE_SOFTWARE : D3D_DRIVER_TYPE_HARDWARE,
      adapter->GetModule (), 0, 0, 0, D3D11_SDK_VERSION, &dx_device, &feature_level, &dx_context));

    if (!dx_device || !dx_context)
      throw xtl::format_operation_exception ("", "::D3D11CreateDevice failed");

    device = dx_device;
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
    throw xtl::make_not_implemented_exception (__FUNCTION__);
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
        throw xtl::make_not_implemented_exception (__FUNCTION__);
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
    throw xtl::make_not_implemented_exception (__FUNCTION__);
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
    throw xtl::make_not_implemented_exception (__FUNCTION__);
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
      case BindFlag_VertexBuffer:   throw xtl::make_not_implemented_exception (__FUNCTION__);
      case BindFlag_IndexBuffer:    throw xtl::make_not_implemented_exception (__FUNCTION__);
      case BindFlag_ConstantBuffer: throw xtl::make_not_implemented_exception (__FUNCTION__);
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
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateSamplerState");
    throw;
  }
}

ITexture* Device::CreateTexture (const TextureDesc& desc)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

ITexture* Device::CreateTexture (const TextureDesc& desc, const TextureData& data)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*ITexture* Device::CreateTexture (const TextureDesc& desc, const TextureData* data)
{
  static const char* METHOD_NAME = "render::low_level::dx11::Device::CreateTexture";

  if (!(desc.bind_flags & (BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil)))
  {
    throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported bindable flags desc.bind_flags=%s",
      get_name ((BindFlag)desc.bind_flags));

    return 0;
  }

  try
  {
    if (desc.bind_flags & BindFlag_Texture)
    {    
      throw xtl::make_not_implemented_exception (__FUNCTION__);
    }
    else if (desc.bind_flags & (BindFlag_RenderTarget | BindFlag_DepthStencil))
    {
      throw xtl::make_not_implemented_exception (__FUNCTION__);
    }
    else return 0;
  }
  catch (xtl::exception& exception)
  {
    exception.touch (METHOD_NAME);

    throw;
  }
}*/

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
    throw xtl::make_not_implemented_exception (__FUNCTION__);
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
    throw xtl::make_not_implemented_exception (__FUNCTION__);
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
    throw xtl::make_not_implemented_exception (__FUNCTION__);
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
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ������ ������� ���������� ���������
*/

IPropertyList* Device::GetProperties ()
{
  return &properties;
}
