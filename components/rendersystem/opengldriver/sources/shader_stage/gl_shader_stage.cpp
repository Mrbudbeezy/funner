#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    �������� ���������� ShaderStage
*/

struct ShaderStage::Impl: public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Impl (const ContextManager& context_manager);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������, �������� � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IShaderParametersLayout* CreateShaderParametersLayout (const ShaderParametersLayoutDesc&);
    IShader*                 CreateShader                 (const ShaderDesc& desc, const LogFunction& error_log);
    IBuffer*                 CreateConstantBuffer         (const BufferDesc& desc);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������, �������� � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetShader                 (IShader* shader);
    void SetShaderParametersLayout (IShaderParametersLayout* parameters_layout);
    void SetConstantBuffer         (size_t buffer_slot, IBuffer* buffer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������, �������� � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IShaderParametersLayout* GetShaderParametersLayout () const;
    IShader*                 GetShader                 () const;
    IBuffer*                 GetConstantBufer          (size_t buffer_slot) const;

//  public:
};


/*
   �����������
*/

ShaderStage::Impl::Impl (const ContextManager& context_manager)
  : ContextObject (context_manager)
{
}

/*
   ������� ���������, �������� � ���������
*/

void ShaderStage::Impl::Bind ()
{
  RaiseNotImplemented ("render::low_level::opengl::ShaderStage::Impl::Bind");
}

/*
   �������� ��������
*/

IShaderParametersLayout* ShaderStage::Impl::CreateShaderParametersLayout (const ShaderParametersLayoutDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::ShaderStage::Impl::CreateShaderParametersLayout");
  return 0;
}

IShader* ShaderStage::Impl::CreateShader (const ShaderDesc& desc, const LogFunction& error_log)
{
  RaiseNotImplemented ("render::low_level::opengl::ShaderStage::Impl::CreateShader");
  return 0;
}

IBuffer* ShaderStage::Impl::CreateConstantBuffer (const BufferDesc& desc)
{
  RaiseNotImplemented ("render::low_level::opengl::ShaderStage::Impl::CreateConstantBuffer");
  return 0;
}
    
/*
   ��������� ���������, �������� � ���������
*/

void ShaderStage::Impl::SetShader (IShader* shader)
{
  RaiseNotImplemented ("render::low_level::opengl::ShaderStage::Impl::SetShader");
}

void ShaderStage::Impl::SetShaderParametersLayout (IShaderParametersLayout* parameters_layout)
{
  RaiseNotImplemented ("render::low_level::opengl::ShaderStage::Impl::SetShaderParametersLayout");
}

void ShaderStage::Impl::SetConstantBuffer (size_t buffer_slot, IBuffer* buffer)
{
  RaiseNotImplemented ("render::low_level::opengl::ShaderStage::Impl::SetConstantBuffer");
}

/*
   ��������� ���������, �������� � ���������
*/

IShaderParametersLayout* ShaderStage::Impl::GetShaderParametersLayout () const
{
  RaiseNotImplemented ("render::low_level::opengl::ShaderStage::Impl::GetShaderParametersLayout");
  return 0;
}

IShader* ShaderStage::Impl::GetShader () const
{
  RaiseNotImplemented ("render::low_level::opengl::ShaderStage::Impl::GetShader");
  return 0;
}

IBuffer* ShaderStage::Impl::GetConstantBufer (size_t buffer_slot) const
{
  RaiseNotImplemented ("render::low_level::opengl::ShaderStage::Impl::GetConstantBufer");
  return 0;
}

/*
   ����������� / ����������
*/

ShaderStage::ShaderStage (const ContextManager& context_manager)
  : impl (new Impl (context_manager))
  {}

ShaderStage::~ShaderStage ()
{
}

/*
   ������� ���������, �������� � ���������
*/

void ShaderStage::Bind ()
{
  impl->Bind ();
}

/*
   �������� ��������
*/

IShaderParametersLayout* ShaderStage::CreateShaderParametersLayout (const ShaderParametersLayoutDesc& desc)
{
  return impl->CreateShaderParametersLayout (desc);
}

IShader* ShaderStage::CreateShader (const ShaderDesc& desc, const LogFunction& error_log)
{
  return impl->CreateShader (desc, error_log);
}

IBuffer* ShaderStage::CreateConstantBuffer (const BufferDesc& desc)
{
  return impl->CreateConstantBuffer (desc);
}
    
/*
   ��������� ���������, �������� � ���������
*/

void ShaderStage::SetShader (IShader* shader)
{
  impl->SetShader (shader);
}

void ShaderStage::SetShaderParametersLayout (IShaderParametersLayout* parameters_layout)
{
  impl->SetShaderParametersLayout (parameters_layout);
}

void ShaderStage::SetConstantBuffer (size_t buffer_slot, IBuffer* buffer)
{
  impl->SetConstantBuffer (buffer_slot, buffer);
}

/*
   ��������� ���������, �������� � ���������
*/

IShaderParametersLayout* ShaderStage::GetShaderParametersLayout () const
{
  return impl->GetShaderParametersLayout ();
}

IShader* ShaderStage::GetShader () const
{
  return impl->GetShader ();
}

IBuffer* ShaderStage::GetConstantBufer (size_t buffer_slot) const
{
  return impl->GetConstantBufer (buffer_slot);
}
