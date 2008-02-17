#include <stl/string>
#include <stl/hash_map>
#include <stl/vector>

#include <xtl/bind.h>

#include <common/hash.h>

#include "shared.h"

using namespace stl;

using namespace common;

using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

void get_base_profile (const char* source, string& profile)
{
  char* point = (char*)strchr (source, '.');

  if (point)
    profile.assign (source, point - source);
  else
    profile.assign (source);
}

}

/*
    �������� ���������� ShaderStage
*/

struct ShaderStage::Impl: public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Impl  (const ContextManager& context_manager);
    ~Impl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������, �������� � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IShaderParametersLayout* CreateShaderParametersLayout (const ShaderParametersLayoutDesc&);
    IProgram*                CreateProgram                (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������, �������� � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetProgram                (IProgram* program);
    void SetShaderParametersLayout (IShaderParametersLayout* parameters_layout);
    void SetConstantBuffer         (size_t buffer_slot, IBuffer* buffer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������, �������� � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IShaderParametersLayout* GetShaderParametersLayout () const;
    IProgram*                GetProgram                () const;
    IBuffer*                 GetConstantBuffer         (size_t buffer_slot) const;

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� � ��� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddShaderManager (ShaderManager*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� �� ���� ��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RemoveShaderByHash (size_t hash);

  private:    
    typedef xtl::trackable_ptr<ShaderParametersLayout>                ShaderParametersLayoutPtr;
    typedef xtl::trackable_ptr<Program>                               ProgramPtr;
    typedef stl::vector <ShaderManager*>                              ShaderManagerArray;
    typedef stl::hash_map<stl::hash_key<const char*>, ShaderManager*> ShaderManagerMap;
    typedef stl::hash_map<size_t, Shader*>                            ShaderMap;

  public:
    ShaderParametersLayoutPtr parameters_layout;                                    //������������ ���������� �������
    ProgramPtr                program;                                              //������
    ConstantBufferPtr         constant_buffers[DEVICE_CONSTANT_BUFFER_SLOTS_COUNT]; //����������� ������ �������
    ShaderMap                 shaders_map;                                          //���������������� �������
    ShaderManagerArray        shader_managers;                                      //������ ����������
    ShaderManagerMap          shader_managers_map;                                  //������������ �������/��������
};


/*
   ����������� / ����������
*/

ShaderStage::Impl::Impl (const ContextManager& context_manager)
  : ContextObject (context_manager)
{
  AddShaderManager (create_glsl_shader_manager (context_manager));
}

ShaderStage::Impl::~Impl ()
{
  for (ShaderManagerArray::iterator i = shader_managers.begin (); i != shader_managers.end (); ++i)
    delete *i;
}

/*
   ������� ���������, �������� � ���������
*/

void ShaderStage::Impl::Bind ()
{
  if (!program)
    RaiseInvalidOperation ("render::low_level::opengl::ShaderStage::Impl::Bind", "Null program");

  program->Bind (constant_buffers, parameters_layout.get ());
}

/*
   �������� ��������
*/

IShaderParametersLayout* ShaderStage::Impl::CreateShaderParametersLayout (const ShaderParametersLayoutDesc& desc)
{
  IShaderParametersLayout* ret_value = new ShaderParametersLayout (GetContextManager ());

  ret_value->SetDesc (desc);

  return ret_value;
}

IProgram* ShaderStage::Impl::CreateProgram (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log)
{
  static const char* METHOD_NAME = "render::low_level::opengl::ShaderStage::Impl::CreateProgram";

  if (!shader_descs)
    RaiseNullArgument (METHOD_NAME, "shader_descs");

  if (!shaders_count)
    RaiseNullArgument (METHOD_NAME, "shaders_count");

  string        profile;
  ShaderManager *manager = NULL;

  ShaderManagerMap::iterator search_result;

  for (size_t i = 0; i < shaders_count; i++)
  {
    get_base_profile (shader_descs[i].profile, profile);

    search_result = shader_managers_map.find (profile.c_str ());

    if (search_result == shader_managers_map.end ())
      RaiseNotSupported (METHOD_NAME, "Shader profile '%s' not supported (no registered shader manager for this profile)", profile.c_str ());

    if (manager && (search_result->second != manager))
      RaiseInvalidArgument (METHOD_NAME, "Can't create shader, detected profiles of different shader managers");

    manager = search_result->second;
  }

  ShaderMap::iterator shader_iterator;

  vector<ShaderPtr> program_shaders (shaders_count);

  for (size_t i = 0; i < shaders_count; i++)
  {
    size_t hash = crc32 (&shader_descs[i], sizeof (ShaderDesc));

    shader_iterator = shaders_map.find (hash);

    if (shader_iterator == shaders_map.end ())
    {
      ShaderPtr shader (manager->CreateShader (shader_descs[i], error_log), false);

      shader->RegisterDestroyHandler (xtl::bind (&Impl::RemoveShaderByHash, this, hash), GetTrackable ());

      shaders_map [hash] = shader.get ();
      program_shaders [i] = shader;
    }
    else
      program_shaders [i] = shader_iterator->second;
  }

  return manager->CreateProgram (shaders_count, &program_shaders[0], error_log);
}
    
/*
   ��������� ���������, �������� � ���������
*/

void ShaderStage::Impl::SetShaderParametersLayout (IShaderParametersLayout* in_parameters_layout)
{
  static const char* METHOD_NAME = "render::low_level::opengl::ShaderStage::Impl::SetShaderParametersLayout";

  if (!in_parameters_layout)
    RaiseNullArgument (METHOD_NAME, "in_parameters_layout");

  parameters_layout = cast_object <ShaderParametersLayout> (in_parameters_layout, METHOD_NAME, "in_parameters_layout");
}

void ShaderStage::Impl::SetProgram (IProgram* in_program)
{
  static const char* METHOD_NAME = "render::low_level::opengl::ShaderStage::Impl::SetProgram";

  if (!in_program)
    RaiseNullArgument (METHOD_NAME, "in_program");

  program = cast_object <Program> (in_program, METHOD_NAME, "in_program");
}

void ShaderStage::Impl::SetConstantBuffer (size_t buffer_slot, IBuffer* buffer)
{
  static const char* METHOD_NAME = "render::low_level::opengl::ShaderStage::Impl::SetConstantBuffer";

  if (!buffer)
    RaiseNullArgument (METHOD_NAME, "buffer");

  if (buffer_slot >= DEVICE_CONSTANT_BUFFER_SLOTS_COUNT)
    RaiseNotSupported (METHOD_NAME, "Can't set constant buffer to slot %u (maximum supported slots %u)", buffer_slot, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT);

  constant_buffers [buffer_slot] = cast_object <IBindableBuffer> (buffer, METHOD_NAME, "buffer");
}

/*
   ��������� ���������, �������� � ���������
*/

IShaderParametersLayout* ShaderStage::Impl::GetShaderParametersLayout () const
{
  return parameters_layout.get ();
}

IProgram* ShaderStage::Impl::GetProgram () const
{
  return program.get ();
}

IBuffer* ShaderStage::Impl::GetConstantBuffer (size_t buffer_slot) const
{
  if (buffer_slot >= DEVICE_CONSTANT_BUFFER_SLOTS_COUNT)
    RaiseNotSupported ("render::low_level::opengl::ShaderStage::Impl::GetConstantBuffer", "Can't get constant buffer from slot %u (maximum supported slots %u)", buffer_slot, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT);

  return constant_buffers [buffer_slot].get ();
}

/*
   ���������� ��������� � ��� ��������
*/

void ShaderStage::Impl::AddShaderManager (ShaderManager* manager)
{
  if (!manager)
    RaiseNullArgument ("render::low_level::opengl::ShaderStage::Impl::AddShaderManager", "manager");

  pair<ShaderManagerMap, bool> insert_result;

  shader_managers.push_back (manager);

  for (size_t i = 0; i < manager->GetProfilesCount (); i++)
  {
    try
    {
      shader_managers_map[manager->GetProfile (i)] = manager;
    }
    catch (std::exception &e)
    {
      shader_managers.pop_back ();
      throw e;
    }
  }
}

/*
   �������� ������� �� ���� ��������� ����
*/

void ShaderStage::Impl::RemoveShaderByHash (size_t hash)
{
  shaders_map.erase (hash);
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

IProgram* ShaderStage::CreateProgram (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log)
{
  return impl->CreateProgram (shaders_count, shader_descs, error_log);
}
   
/*
   ��������� ���������, �������� � ���������
*/

void ShaderStage::SetProgram (IProgram* program)
{
  impl->SetProgram (program);
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

IProgram* ShaderStage::GetProgram () const
{
  return impl->GetProgram ();
}

IBuffer* ShaderStage::GetConstantBuffer (size_t buffer_slot) const
{
  return impl->GetConstantBuffer (buffer_slot);
}
