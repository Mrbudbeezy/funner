#include <stl/string>
#include <stl/hash_map>

#include <xtl/bind.h>

#include <common/hash.h>

#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

/*
    ��������� ������ ��������
*/

class ShaderStageState: public IStageState
{
  public:  
      //������������
    ShaderStageState (ShaderStageState* in_main_state) : owner (0), main_state (in_main_state) {}
    ShaderStageState (ContextObject* in_owner) : owner (in_owner), main_state (0) {}    

      //��������� ���������
    void SetProgram (ICompiledProgram* in_program)
    {
      if (in_program == program)
        return;

      program = in_program;

      UpdateNotify ();
    }

      //��������� ���������
    ICompiledProgram* GetProgram () const
    {
      return program.get ();
    }

      //��������� ������������ �������
    void SetConstantBuffer (size_t buffer_slot, IBindableBuffer* buffer)
    {
      if (buffer == constant_buffers [buffer_slot])
        return;
      
      constant_buffers [buffer_slot] = buffer;

      UpdateNotify ();
    }

      //��������� ������������ �������
    IBindableBuffer* GetConstantBuffer (size_t buffer_slot) const
    {
      return constant_buffers [buffer_slot].get ();
    }
    
      //��������� ������� ���������� �� ����������� ������
    ConstantBufferPtr* GetConstantBuffers ()
    {
      return &constant_buffers [0];
    }
    
      //��������� ������������ ���������� �������
    void SetProgramParametersLayout (ProgramParametersLayout* in_parameters_layout)
    {
      if (in_parameters_layout == parameters_layout)
        return;
      
      parameters_layout = in_parameters_layout;
      
      UpdateNotify ();
    }

      //��������� ������������ ���������� �������
    ProgramParametersLayout* GetProgramParametersLayout () const
    {
      return parameters_layout.get ();
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
        SetProgram (source.GetProgram ());

      if (mask.ss_program_parameters_layout)
        SetProgramParametersLayout (source.GetProgramParametersLayout ());

      for (size_t i = 0; i < DEVICE_CONSTANT_BUFFER_SLOTS_COUNT; i++)
        if (mask.ss_constant_buffers [i])
          SetConstantBuffer (i, source.GetConstantBuffer (i));
    }

      //���������� �� ��������� ���������
    void UpdateNotify ()
    {
      if (!owner)
        return;

      owner->GetContextManager ().StageRebindNotify (Stage_Shading);
    }

  private:
    typedef xtl::trackable_ptr<ShaderStageState>        ShaderStageStatePtr;
    typedef xtl::trackable_ptr<ProgramParametersLayout> ProgramParametersLayoutPtr;
    typedef xtl::trackable_ptr<ICompiledProgram>        ProgramPtr;

  private:
    ContextObject*             owner;                                                 //�������� ���������
    ShaderStageStatePtr        main_state;                                            //�������� ��������� ������
    ProgramParametersLayoutPtr parameters_layout;                                     //������������ ���������� �������
    ProgramPtr                 program;                                               //������
    ConstantBufferPtr          constant_buffers [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT]; //����������� ������ �������
};

/*
    ��������� �������� � �����������
*/

struct ProgramWithParameters
{
  ICompiledProgram*        program;           //��������� ��������
  ProgramParametersLayout* parameters_layout; //������������ ����������
  
///�����������
  ProgramWithParameters (ICompiledProgram* in_program, ProgramParametersLayout* layout) : program (in_program), parameters_layout (layout) {}
  
///���������
  bool operator == (const ProgramWithParameters& p) const { return program == p.program && parameters_layout == p.parameters_layout; }  
};

//��������� ����
size_t hash (const ProgramWithParameters& p)
{
  return stl::hash (p.program, stl::hash (p.parameters_layout));
}

}

/*
    �������� ���������� ShaderStage
*/

struct ShaderStage::Impl: public ContextObject, public ShaderStageState
{
  public:
///�����������
    Impl (const ContextManager& context_manager)
      : ContextObject (context_manager),
        ShaderStageState (static_cast<ContextObject*> (this))
    {
      try
      {
          //���������� ���������� ��������
          
#ifndef OPENGL_ES_SUPPORT

        if (GetCaps ().has_arb_shading_language_100)
          RegisterManager (ShaderManagerPtr (create_glsl_shader_manager (context_manager), false));

#endif

        if (GetCaps ().has_ffp)
        {
          RegisterManager (ShaderManagerPtr (create_ffp_shader_manager (context_manager), false));

            //����������� ��������� "�� ���������"

          ShaderDesc shader_desc;

          memset (&shader_desc, 0, sizeof (shader_desc));

          shader_desc.name             = "Default shader-stage program";
          shader_desc.source_code_size = ~0;
          shader_desc.source_code      = "";
          shader_desc.profile          = "ffp";
          
          default_program = ProgramPtr (CreateProgram (1, &shader_desc, xtl::bind (&Impl::LogShaderMessage, this, _1)), false);
        }

#ifndef OPENGL_ES_SUPPORT

        if (!default_program && GetCaps ().has_arb_shading_language_100)
        {
            //����������� ��������� "�� ���������"

            const char* PIXEL_SHADER  = "void main (void) {gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0);}";
            const char* VERTEX_SHADER = "void main (void) {gl_Position = gl_Vertex;}";

            ShaderDesc shader_descs [] = {
              {"Default shader-stage pixel shader", strlen (PIXEL_SHADER), PIXEL_SHADER, "glsl.ps", ""},
              {"Default shader-stage vertex shader", strlen (VERTEX_SHADER), VERTEX_SHADER, "glsl.vs", ""}
            };

            default_program = ProgramPtr (CreateProgram (sizeof shader_descs / sizeof *shader_descs, shader_descs, xtl::bind (&Impl::LogShaderMessage, this, _1)), false);
        }

#endif

        if (!default_program)
          throw ("", "Can't create default program, no supported profiles");
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::ShaderStage::Impl::Impl");
        throw;
      }
    }

///��������� ��������� � �������� OpenGL
    void Bind ()
    {
      try
      {
          //��������� ���������, ��������������� � �������� OpenGL

        IBindableProgram& bindable_program = GetBindableProgram ();

          //��������� ��������� � �������� OpenGL

        bindable_program.Bind (GetConstantBuffers ());
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::ShaderStage::Impl::Bind");
        throw;
      }
    }       

///�������� ���������
    ICompiledProgram* CreateProgram (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log)
    {
      try
      {
          //�������� ������������ ����������        

        if (!shader_descs)
          throw xtl::make_null_argument_exception ("", "shader_descs");

        if (!shaders_count)
          throw xtl::make_null_argument_exception ("", "shaders_count");

        IShaderManager* manager = 0;

        for (size_t i=0; i<shaders_count; i++)
        {
          const ShaderDesc& desc = shader_descs [i];

          if (!desc.source_code)
            throw xtl::format_exception<xtl::null_argument_exception> ("", "shader_descs[%u].source_code", i);

            //��������� ����� �������� �������

          if (!desc.profile)
            throw xtl::format_exception<xtl::null_argument_exception> ("", "Null argument 'shader_descs[%u].profile'", i);

            //����� ��������� �������� �� ����� �������� �������

          ProfileMap::iterator iter = profiles.find (desc.profile);

          if (iter == profiles.end ())
            throw xtl::format_not_supported_exception ("", "Shader profile '%s' not supported", desc.profile);

          if (manager && iter->second != manager)
            throw xtl::format_exception<xtl::argument_exception> ("", "Invalid argument 'shader_descs[%u].profile'='%s'. "
              "Incompatible shader profiles (previous profile is '%s')", i, desc.profile, shader_descs [0].profile);
              
            //���������� ���������

          manager = iter->second.get ();
        }
        
          //���������� � ������������� ���������� ������

        StageRebindNotify (Stage_Shading);        

          //���������� ��������        

        stl::vector<ShaderPtr> program_shaders;

        program_shaders.reserve (shaders_count);

        for (size_t i=0; i<shaders_count; i++)
        {
            //������������� �������� �������

          ShaderDesc desc = shader_descs [i];

          if (!desc.name)
            desc.name = "__unnamed__";

          if (!desc.options)
            desc.options = "";

          if (desc.source_code_size == size_t (~0))
            desc.source_code_size = strlen (desc.source_code);

            //���������� ����������������� ������� � �������� ���������

          program_shaders.push_back (GetShader (desc, *manager, error_log));
        }

          //������������ ������� ���������� �� �������

        stl::vector<IShader*> program_shader_pointers;

        program_shader_pointers.resize (program_shaders.size ());

        for (size_t i=0; i<program_shaders.size (); i++)
          program_shader_pointers [i] = program_shaders [i].get ();

          //�������� ��������� ��������

        return manager->CreateProgram (program_shader_pointers.size (), &program_shader_pointers [0], error_log);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::ShaderStage::Impl::CreateProgram");
        throw;
      }
    }
    
///��������� ������ �������������� �������� �������
    const char* GetShaderProfilesString () const
    {
      return supported_profiles.c_str ();
    }
    
  private:
    typedef xtl::com_ptr<IShaderManager>                                ShaderManagerPtr;
    typedef xtl::com_ptr<IShader>                                       ShaderPtr;
    typedef stl::hash_map<size_t, IShader*>                             ShaderMap;
    typedef xtl::com_ptr<ICompiledProgram>                              ProgramPtr;
    typedef xtl::com_ptr<IBindableProgram>                              BindableProgramPtr;
    typedef stl::hash_map<ProgramWithParameters, BindableProgramPtr>    BindableProgramMap;
    typedef stl::hash_map<stl::hash_key<const char*>, ShaderManagerPtr> ProfileMap;

  private:
///����������� ��������� ��������
    void RegisterManager (const ShaderManagerPtr& manager)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::ShaderStage::Impl::RegisterManager";

        //�������� ������������ ����������

      if (!manager)
        throw xtl::make_null_argument_exception (METHOD_NAME, "manager");

        //����������� ��������
        
      size_t start_profiles_string_size = supported_profiles.size ();

      for (size_t i=0; i<manager->GetProfilesCount (); i++)
      {
        try
        {
          const char* profile = manager->GetProfile (i);
          
          if (!profile)
            throw xtl::format_operation_exception (METHOD_NAME, "Null profile");

          ProfileMap::iterator iter = profiles.find (profile);

          if (iter != profiles.end ())
            throw xtl::format_operation_exception (METHOD_NAME, "Profile '%s' already registered", profile);

          profiles [profile] = manager.get ();

          if (!supported_profiles.empty ())
            supported_profiles += ' ';

          supported_profiles += profile;
        }
        catch (...)
        {
          while (i--)
            profiles.erase (manager->GetProfile (i));

          supported_profiles.resize (start_profiles_string_size);

          throw;
        }
      }      
    }  
  
///��������� ���� �������
    size_t GetShaderHash (const ShaderDesc& desc)
    {
      return common::crc32 (desc.source_code, desc.source_code_size, common::strhash (desc.profile, common::strhash (desc.options)));
    }

///��������� �������
    ShaderPtr GetShader (const ShaderDesc& desc, IShaderManager& manager, const LogFunction& error_log)
    {
      try
      {
          //��������� ���� �������

        size_t shader_hash = GetShaderHash (desc);

          //����� ������� � ����� ���������������� ��������

        ShaderMap::iterator iter = shaders.find (shader_hash);

        if (iter != shaders.end ())
          return iter->second;
          
          //�������� ������ �������

        ShaderPtr shader (manager.CreateShader (desc, error_log), false);

          //����������� ����������������� �������

        shader->RegisterDestroyHandler (xtl::bind (&Impl::RemoveShaderByHash, this, shader_hash), GetTrackable ());

        shaders.insert_pair (shader_hash, shader.get ());

        return shader;
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::ShaderStage::Impl::GetShader");
        throw;
      }
    }    
    
///�������� ������� �� ����
    void RemoveShaderByHash (size_t shader_hash)
    {
      shaders.erase (shader_hash);
    }
    
///��������� ��������� � ������������ ��������� � �������� OpenGL
    IBindableProgram& GetBindableProgram (ICompiledProgram* program, ProgramParametersLayout* layout)
    {
      try
      {
          //����� ��������� � �����

        ProgramWithParameters parametrized_program (program, layout);

        BindableProgramMap::iterator iter = bindable_programs.find (parametrized_program);

        if (iter != bindable_programs.end ())
          return *iter->second;

          //�������� ������������ ����������

        if (!program)
          throw xtl::make_null_argument_exception ("", "program");
          
          //�������� ���������

        BindableProgramPtr bindable_program (program->CreateBindableProgram (layout), false);
        
          //����������� � �����          

        iter = bindable_programs.insert_pair (parametrized_program, bindable_program).first;
        
          //����������� ������������ ���������� �� �������� ���������
          
        xtl::connection c;

        try
        {
          xtl::trackable::function_type on_destroy_program (xtl::bind (&Impl::RemoveBindableProgram, this, program, (ProgramParametersLayout*)0));
          xtl::trackable::function_type on_destroy_layout (xtl::bind (&Impl::RemoveBindableProgram, this, (ICompiledProgram*)0, layout));

          c = program->RegisterDestroyHandler (on_destroy_program, GetTrackable ());

          if (layout)
            layout->RegisterDestroyHandler (on_destroy_layout, GetTrackable ());
        }
        catch (...)
        {
          c.disconnect ();
          bindable_programs.erase (iter);
          throw;
        }

        return *bindable_program;
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::ShaderStage::Impl::CreateShadingState");
        throw;
      }
    }

    IBindableProgram& GetBindableProgram ()
    {
      ICompiledProgram* program = GetProgram ();
      
      if (!program)
        program = default_program.get ();

      return GetBindableProgram (program, GetProgramParametersLayout ());
    }
    
///�������� ���������, ��������������� � �������� OpenGL
    void RemoveBindableProgram (ICompiledProgram* program, ProgramParametersLayout* layout)
    {
      for (BindableProgramMap::iterator iter = bindable_programs.begin (), end = bindable_programs.end (); iter != end;)
      {
        if (iter->first.program == program || iter->first.parameters_layout == layout)
        {
          BindableProgramMap::iterator next = iter;

          ++next;

          bindable_programs.erase (iter);

          iter = next;
        }
        else
          ++iter;
      }
    }

///���������������� ������ �������
    void LogShaderMessage (const char* message)
    {
      if (!message || !*message)
        return;

      LogPrintf ("%s", message);
    }

  private:
    ShaderMap          shaders;            //���������������� �������
    BindableProgramMap bindable_programs;  //����� �������� �������� � ������������ ��������� � �������� OpenGL
    ProfileMap         profiles;           //������������ ������� -> �������� ��������
    ProgramPtr         default_program;    //��������� "�� ���������"
    stl::string        supported_profiles; //�������������� ������� ��������
};

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
  return new ShaderStageState (static_cast<ShaderStageState*> (&*impl));
}

/*
    ��������� ��������� ������ � �������� OpenGL
*/

void ShaderStage::Bind ()
{
  impl->Bind ();
}

/*
    �������� �������� ������ ��������
*/

IProgramParametersLayout* ShaderStage::CreateProgramParametersLayout (const ProgramParametersLayoutDesc& desc)
{
  try
  {    
    return new ProgramParametersLayout (impl->GetContextManager (), desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::ShaderStage::CreateProgramParametersLayout");
    throw;
  }
}

IProgram* ShaderStage::CreateProgram (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log)
{
  return impl->CreateProgram (shaders_count, shader_descs, error_log);
}
   
/*
    ��������� ���������� ������ ��������
*/

void ShaderStage::SetProgram (IProgram* program)
{
  try
  {
      //�������� ������������� ����������

    ContextObject* program_object = cast_object<ContextObject> (program, "", "program");

    if (program_object && !program_object->IsCompatible (*impl))
      throw xtl::format_exception<xtl::bad_argument> ("", "Argument 'program' is incompatible with target IDevice");

      //��������� ���������

    impl->SetProgram (cast_object<ICompiledProgram> (program, "", "program"));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::ShaderStage::SetProgram");
    throw;
  }
}

void ShaderStage::SetProgramParametersLayout (IProgramParametersLayout* parameters_layout)
{
  impl->SetProgramParametersLayout (cast_object<ProgramParametersLayout> (*impl, parameters_layout, "render::low_level::opengl::ShaderStage::SetProgramParametersLayout", "parameters_layout"));
}

void ShaderStage::SetConstantBuffer (size_t buffer_slot, IBuffer* buffer)
{
  try
  {
      //�������� ������������ ������� ������������ ������
    
    if (buffer_slot >= DEVICE_CONSTANT_BUFFER_SLOTS_COUNT)
      throw xtl::make_range_exception ("", "buffer_slot", buffer_slot, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT);

      //�������� ������������� ����������

    ContextObject* buffer_object = cast_object<ContextObject> (buffer, "", "buffer");      

    if (buffer_object && !buffer_object->IsCompatible (*impl))
      throw xtl::format_exception<xtl::bad_argument> ("", "Argument 'buffer' is incompatible with target IDevice");

      //��������� ������

    impl->SetConstantBuffer (buffer_slot, cast_object<IBindableBuffer> (buffer, "", "buffer"));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::ShaderStage::SetConstantBuffer");
    throw;
  }
}

/*
    ��������� ���������� ������ ��������
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
  if (buffer_slot >= DEVICE_CONSTANT_BUFFER_SLOTS_COUNT)
    throw xtl::make_range_exception ("render::low_level::opengl::ShaderStage::GetConstantBuffer", "buffer_slot", buffer_slot, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT);

  return impl->GetConstantBuffer (buffer_slot);
}

/*
    ��������� ������ �������������� ���������� �������� ��������
*/

const char* ShaderStage::GetShaderProfilesString () const
{
  return impl->GetShaderProfilesString ();
}
