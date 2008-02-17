#ifndef RENDER_GL_DRIVER_SHADER_STAGE_SHARED_HEADER
#define RENDER_GL_DRIVER_SHADER_STAGE_SHARED_HEADER

#include <shared/shader_stage.h>
#include <shared/input_stage.h>

#include <common/exception.h>

#include <xtl/intrusive_ptr.h>
#include <xtl/trackable_ptr.h>
#include <xtl/uninitialized_storage.h>

namespace render
{

namespace low_level
{

namespace opengl
{

typedef xtl::trackable_ptr<IBindableBuffer> ConstantBufferPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ShaderParametersLayout : virtual public IShaderParametersLayout, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ShaderParametersLayout (const ContextManager& manager);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetDesc (const ShaderParametersLayoutDesc& in_desc);
    ShaderParametersLayoutDesc& GetDesc ();

  private:
    ShaderParametersLayoutDesc desc;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Shader : virtual public IObject
{
};

typedef xtl::com_ptr<Shader> ShaderPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Program : virtual public IProgram
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Bind (ConstantBufferPtr* constant_buffers, ShaderParametersLayout* shader_parameters_layout) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ShaderManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
   virtual ~ShaderManager () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
   virtual size_t GetProfilesCount () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
   virtual const char* GetProfile (size_t index) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
   virtual Shader*  CreateShader  (const ShaderDesc& shader_desc, const LogFunction& error_log) = 0;
   virtual Program* CreateProgram (size_t shaders_count, ShaderPtr* shaders, const LogFunction& error_log) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
ShaderManager* create_glsl_shader_manager (const ContextManager& manager);

}

}

}

#endif
