#ifndef RENDER_GL_DRIVER_SHADER_STAGE_SHARED_HEADER
#define RENDER_GL_DRIVER_SHADER_STAGE_SHARED_HEADER

#include <render/low_level/utils.h>

#include <shared/shader_stage.h>
#include <shared/input_stage.h>

#include <common/exception.h>

#include <stl/vector>

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
///�������� ������� ��������� ������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ShaderStageCache
{
  ShaderStageCache_UsedProgram,    //ID ������� ������������ ���������
};

struct ProgramParameterGroup
{
  size_t            slot;       //����� ����� � ���������� �������
  size_t            count;      //���������� ��������� ������
  ProgramParameter* parameters; //��������� �� ������ ������� � ����������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ProgramParametersLayout : virtual public IProgramParametersLayout, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ProgramParametersLayout (const ProgramParametersLayoutDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetDesc (const ProgramParametersLayoutDesc& in_desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
          ///�������� ��������� Get!!!
    size_t                 ParametersCount () {return parameters.size ();}
    size_t                 GroupsCount ();
    ProgramParameterGroup& ParametersGroup (size_t index);

    size_t                 GetGroupsCount     () const { return parameter_groups.size (); }
    size_t                 GetParametersCount () const { return parameter_groups.size (); }    
    ProgramParameterGroup* GetGroups          ()       { return &parameter_groups [0]; }
    ProgramParameter*      GetParameters      ()       { return &parameters [0]; }

  private:
    stl::vector<ProgramParameterGroup> parameter_groups;
    stl::vector<ProgramParameter>      parameters;
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
class Program : virtual public IProgram, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Bind (ConstantBufferPtr* constant_buffers, ProgramParametersLayout* parameters_layout) = 0;
    
  protected:
    Program (const ContextManager& manager) : ContextObject (manager) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ShaderManager : virtual public IObject
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
ShaderManager* create_fpp_shader_manager  (const ContextManager& manager);
ShaderManager* create_glsl_shader_manager (const ContextManager& manager);

}

}

}

#endif
