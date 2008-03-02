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

/*
    ��������� ������ ��������
*/

class ShaderStageState: public IStageState
{
  public:  
      //�����������
    ShaderStageState (ShaderStageState* in_main_state = 0) : main_state (in_main_state) {}

      //��������� ���������
    void SetProgram (Program* in_program)
    {
      program = in_program;
    }

      //��������� ���������
    Program* GetProgram () const
    {
      return program.get ();
    }

      //��������� ������������ �������
    void SetConstantBuffer (size_t buffer_slot, IBindableBuffer* buffer)
    {
      constant_buffers [buffer_slot] = buffer;
    }

      //��������� ������������ �������
    IBindableBuffer* GetConstantBuffer (size_t buffer_slot) const
    {
      if (buffer_slot >= DEVICE_CONSTANT_BUFFER_SLOTS_COUNT)
        RaiseNotSupported ("render::low_level::opengl::ShaderStage::Impl::GetConstantBuffer", "Can't get constant buffer from slot %u (maximum supported slots %u)", buffer_slot, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT);

      return constant_buffers [buffer_slot].get ();
    }
    
      //��������� ������������ ���������� �������
    void SetProgramParametersLayout (ProgramParametersLayout* in_parameters_layout)
    {
      parameters_layout = in_parameters_layout;
    }

      //��������� ������������ ���������� �������
    ProgramParametersLayout* GetProgramParametersLayout () const
    {
      return parameters_layout.get ();
    }

       //�������

    void Bind ()
    {
        //���� NULL, �� ����������� ��������
        
      static const char* METHOD_NAME = "render::low_level::opengl::ShaderStage::Bind";

      if (!program)
        RaiseInvalidOperation (METHOD_NAME, "Null program");

      if (!parameters_layout)
        RaiseInvalidOperation (METHOD_NAME, "Null program parameters layout");

      program->Bind (constant_buffers, parameters_layout.get ());
    }

      //������ ���������
    void Capture (const StateBlockMask& mask)
    {
      if (main_state)
        Copy (*main_state, mask);
    }
    
      //�������������� ���������
    void Apply (const StateBlockMask& mask)
    {
      if (main_state)
        main_state->Copy (*this, mask);
    }
    
  private:
      //�����������
    void Copy (const ShaderStageState& source, const StateBlockMask& mask)
    {
      if (mask.ss_program)
        program = source.GetProgram ();

      if (mask.ss_program_parameters_layout)
        parameters_layout = source.GetProgramParametersLayout ();

      for (size_t i = 0; i < DEVICE_CONSTANT_BUFFER_SLOTS_COUNT; i++)
        if (mask.ss_constant_buffers [i])
          constant_buffers[i] = source.GetConstantBuffer (i);
    }

  private:
    typedef xtl::trackable_ptr<ShaderStageState>        ShaderStageStatePtr;
    typedef xtl::trackable_ptr<ProgramParametersLayout> ProgramParametersLayoutPtr;
    typedef xtl::trackable_ptr<Program>                 ProgramPtr;

  private:
    ShaderStageStatePtr        main_state;                                           //�������� ��������� ������
    ProgramParametersLayoutPtr parameters_layout;                                    //������������ ���������� �������
    ProgramPtr                 program;                                              //������
    ConstantBufferPtr          constant_buffers[DEVICE_CONSTANT_BUFFER_SLOTS_COUNT]; //����������� ������ �������
};

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
///��������� ��������� ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ShaderStageState& GetState () { return state; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IProgramParametersLayout* CreateProgramParametersLayout (const ProgramParametersLayoutDesc&);
    IProgram*                 CreateProgram                 (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������, ���������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetProgram (IProgram* program) 
    {
      static const char* METHOD_NAME = "render::low_level::opengl::ShaderStage::Impl::SetProgram";

      state.SetProgram (cast_object <Program> (*this, program, METHOD_NAME, "program"));
    }

    void SetProgramParametersLayout (IProgramParametersLayout* parameters_layout) 
    {
      static const char* METHOD_NAME = "render::low_level::opengl::ShaderStage::Impl::SetProgramParametersLayout";

      state.SetProgramParametersLayout (cast_object <ProgramParametersLayout> (parameters_layout, METHOD_NAME, "parameters_layout"));
    }

    void SetConstantBuffer (size_t buffer_slot, IBuffer* buffer)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::ShaderStage::Impl::SetConstantBuffer";

      if (buffer_slot >= DEVICE_CONSTANT_BUFFER_SLOTS_COUNT)
        RaiseNotSupported (METHOD_NAME, "Can't set constant buffer to slot %u (maximum supported slots %u)", buffer_slot, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT);

      cast_object<ContextObject> (*this, buffer, METHOD_NAME, "buffer");

      state.SetConstantBuffer (buffer_slot, cast_object<IBindableBuffer> (buffer, METHOD_NAME, "buffer"));
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������, ���������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IProgramParametersLayout* GetProgramParametersLayout () const {return state.GetProgramParametersLayout ();}
    IProgram*                 GetProgram                 () const {return state.GetProgram ();}
    IBuffer*                  GetConstantBuffer          (size_t buffer_slot) const {return state.GetConstantBuffer (buffer_slot);}

  private:    
    typedef xtl::com_ptr<ShaderManager>                               ShaderManagerPtr;
    typedef stl::vector <ShaderManagerPtr>                            ShaderManagerArray;
    typedef stl::hash_map<stl::hash_key<const char*>, ShaderManager*> ShaderManagerMap;
    typedef stl::hash_map<size_t, Shader*>                            ShaderMap;

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� � ��� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddShaderManager (const ShaderManagerPtr& manager);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� �� ���� ��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RemoveShaderByHash (size_t hash);

  public:
    ShaderStageState           state;               //��������� ������
    ShaderMap                  shaders_map;         //���������������� �������
    ShaderManagerArray         shader_managers;     //������ ����������
    ShaderManagerMap           shader_managers_map; //������������ �������/��������
};


/*
   ����������� / ����������
*/

ShaderStage::Impl::Impl (const ContextManager& context_manager)
  : ContextObject (context_manager)
{
  if (GetCaps ().has_arb_shading_language_100)
    AddShaderManager (ShaderManagerPtr (create_glsl_shader_manager (context_manager), false));

  AddShaderManager (ShaderManagerPtr (create_fpp_shader_manager (context_manager), false));
}

ShaderStage::Impl::~Impl ()
{
}

/*
   �������
*/

void ShaderStage::Impl::Bind ()
{
  state.Bind ();
}

/*
   �������� ��������
*/

IProgramParametersLayout* ShaderStage::Impl::CreateProgramParametersLayout (const ProgramParametersLayoutDesc& desc)
{
  return new ProgramParametersLayout (desc);
}

IProgram* ShaderStage::Impl::CreateProgram (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log)
{
  static const char* METHOD_NAME = "render::low_level::opengl::ShaderStage::Impl::CreateProgram";

  if (!shader_descs)
    RaiseNullArgument (METHOD_NAME, "shader_descs");

  if (!shaders_count)
    RaiseNullArgument (METHOD_NAME, "shaders_count");
    
      //������� �������� � ������!!!

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
      ShaderDesc shader_desc = shader_descs [i];
      
      if (!shader_desc.name)
        shader_desc.name = "__unnamed__";
        
      if (!shader_desc.source_code)
        Raise<ArgumentNullException> (METHOD_NAME, "shader_descs[%u].source_code", i);
        
      if (shader_desc.source_code_size == ~0)
        shader_desc.source_code_size = strlen (shader_desc.source_code);
      
      ShaderPtr shader (manager->CreateShader (shader_desc, error_log), false);      

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
   ���������� ��������� � ��� ��������
*/

void ShaderStage::Impl::AddShaderManager (const ShaderManagerPtr& manager)
{
  if (!manager)
    RaiseNullArgument ("render::low_level::opengl::ShaderStage::Impl::AddShaderManager", "manager");

  shader_managers.push_back (manager);

  for (size_t i = 0; i < manager->GetProfilesCount (); i++)
  {
    try
    {
      shader_managers_map[manager->GetProfile (i)] = manager.get (); //��������� ��������� ���������� � ����������!!!!
    }
    catch (...)
    {
      for (size_t j = 0; j < manager->GetProfilesCount (); j++)
        shader_managers_map.erase (manager->GetProfile (i));

      shader_managers.pop_back ();
      throw;
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
   �������� ������� ��������� ������
*/

IStageState* ShaderStage::CreateStageState ()
{
  return new ShaderStageState (&impl->GetState ());
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

IProgramParametersLayout* ShaderStage::CreateProgramParametersLayout (const ProgramParametersLayoutDesc& desc)
{
  return impl->CreateProgramParametersLayout (desc);
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

void ShaderStage::SetProgramParametersLayout (IProgramParametersLayout* parameters_layout)
{
  impl->SetProgramParametersLayout (parameters_layout);
}

void ShaderStage::SetConstantBuffer (size_t buffer_slot, IBuffer* buffer)
{
  impl->SetConstantBuffer (buffer_slot, buffer);
}

/*
   ��������� ���������, �������� � ���������
*/

IProgramParametersLayout* ShaderStage::GetProgramParametersLayout () const
{
  return impl->GetProgramParametersLayout ();
}

IProgram* ShaderStage::GetProgram () const
{
  return impl->GetProgram ();
}

IBuffer* ShaderStage::GetConstantBuffer (size_t buffer_slot) const
{
  return impl->GetConstantBuffer (buffer_slot);
}
