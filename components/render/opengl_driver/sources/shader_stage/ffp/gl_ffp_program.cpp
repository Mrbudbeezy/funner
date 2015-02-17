#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

namespace
{

/*
    ���������
*/

const unsigned int FFP_DYNAMIC_PARAMETER_FIELDS_RESERVE_SIZE = 8; //������������� ���������� �������� ������������ ����������

/*
    ����: ��� - ��������
*/

struct Tag2Value
{
  const char* tag;
  int         value;
};

/*
    ������ FFP-��������
*/

class FfpProgramParser
{
  public:
    FfpProgramParser (FfpState& in_base_state, FfpDynamicParameterMap& in_dynamic_parameters, const ShaderDesc& shader_desc, const LogFunction& error_log)
     : base_state (in_base_state),
       dynamic_parameters (in_dynamic_parameters),
       parser (shader_desc.name, shader_desc.source_code_size, shader_desc.source_code, "wxf")
    {
        //������ �������

      ParseShader ();

        //�������� ������ �������
        
      ParseLog log = parser.Log ();

      for (size_t i=0, count=log.MessagesCount (); i<count; i++)
        error_log (log.Message (i));

      if (log.HasErrors ())
        throw xtl::format_exception<xtl::bad_argument> ("render::low_level::opengl::FfpProgramParser::FfpProgramParser", "Errors in shader '%s'", shader_desc.name);
    }
    
  private:
      //���������� ������������� ���������
    void AddDynamicParameter (Parser::Iterator line_iter, const char* name, FfpDynamicParameterType type, unsigned int count)
    {
      FfpDynamicParameterMap::iterator iter = dynamic_parameters.find (name);

      if (iter != dynamic_parameters.end ())
      {
        parser.Log ().Error (*line_iter, "Parameter '%s' has been already defined", name);
        return;
      }

      FfpDynamicParameter& param = dynamic_parameters.insert_pair (name, FfpDynamicParameter ()).first->second;

      param.type   = type;
      param.count  = count;      

      param.field_offsets.reserve (FFP_DYNAMIC_PARAMETER_FIELDS_RESERVE_SIZE);
    }
    
      //���������� �������� �� ������������ ��������
    void AddDynamicParameterField (Parser::Iterator line_iter, const char* name, unsigned int offset, FfpDynamicParameterType type, unsigned int count)
    {
      FfpDynamicParameterMap::iterator iter = dynamic_parameters.find (name);

      if (iter == dynamic_parameters.end ())
      {
        parser.Log ().Error (*line_iter, "Undefined parameter '%s'", name);
        return;
      }
      
      FfpDynamicParameter& param = iter->second;
      
      if (param.type != type)
      {
        parser.Log ().Error (*line_iter, "Type of parameter '%s' mismatch", name);
        return;
      }
      
      if (param.count < count)      
      {
        parser.Log ().Error (*line_iter, "Parameter '%s' attributes count %u is less than expected attributes count %u",
          name, param.count, count);
        return;
      }
      
      param.field_offsets.push_back (offset);
    }  
    
      //������ ��������
    template <class T, FfpDynamicParameterType Type>
    void ParseValues (Parser::Iterator iter, const char* node_name, unsigned int offset, unsigned int count)
    {
      iter = iter->First (node_name);
      
      if (!iter)
        return;        

      size_t       attributes_count = iter->AttributesCount ();
      const char** attributes       = iter->Attributes ();      

      if (attributes_count == 1 && isalpha (*attributes [0]))
      {
          //���������� ������������� ���������

        AddDynamicParameterField (iter, attributes [0], offset, Type, count);

        return;
      }

        //�������� ������������ ����� ���������

      if (attributes_count < count)
      {
        parser.Log ().Error (*iter, "Too few attributes (%u attributes expected)", count);

        return;
      }

        //������ ���������

      Parser::AttributeIterator attr_iter = xtl::io::make_token_iterator (attributes, attributes + count);

      size_t parsed_attributes_count = read_range (attr_iter, (T*)((char*)&base_state + offset));

      if (parsed_attributes_count != count)
        parser.Log ().Error (*iter, "Error at read attribute #%u", parsed_attributes_count);
    }

      //������ ������������� ��������
    void ParseIntegerValues (Parser::Iterator iter, const char* node_name, unsigned int offset, unsigned int count)
    {
      ParseValues<int, FfpDynamicParameterType_Int> (iter, node_name, offset, count);
    }

      //������ ������������ ��������
    void ParseFloatValues (Parser::Iterator iter, const char* node_name, unsigned int offset, unsigned int count)
    {
      ParseValues<float, FfpDynamicParameterType_Float> (iter, node_name, offset, count);
    }
    
      //������ ��������
    void ParseVector4f (Parser::Iterator iter, const char* node_name, unsigned int offset)
    {
      ParseFloatValues (iter, node_name, offset, 4);
    }

    void ParseVector3f (Parser::Iterator iter, const char* node_name, unsigned int offset)
    {
      ParseFloatValues (iter, node_name, offset, 3);
    }

      //������ �������
    void ParseMatrix4f (Parser::Iterator iter, const char* node_name, unsigned int offset)
    {
      ParseFloatValues (iter, node_name, offset, 16);
    }
    
      //������ ���������� ����������
    void ParseParameterDeclarations (Parser::Iterator params_iter, const char* name, FfpDynamicParameterType type, unsigned int count)
    {
      for (Parser::NamesakeIterator iter=params_iter->First (name); iter; ++iter)
      {
        if (!iter->AttributesCount ())
        {
          parser.Log ().Error (*iter, "Parameter names expected");
          continue;
        }

        for (unsigned int i=0; i<iter->AttributesCount (); i++)
          AddDynamicParameter (iter, iter->Attribute (i), type, count);
      }
    }
    
      //������ ���������� ��������
    void ParseEnum (Parser::Iterator iter, const char* node_name, unsigned int offset, const Tag2Value* pairs)
    {
      iter = iter->First (node_name);

      if (!iter)
        return;
        
      if (!iter->AttributesCount ())
      {
        parser.Log ().Error (*iter, "Too few attributes (1 attribute expected)");
        return;
      }

        //����� ��������, ���������������� ����������� ������

      const char* value = iter->Attribute (0);

      for (const Tag2Value* i=pairs; i->tag; i++)
        if (!xtl::xstricmp (i->tag, value))
        {
          *(int*)((char*)&base_state + offset) = i->value;

          return;
        }

        //���� ������������ �� ������� - �������� �� ������

      parser.Log ().Error (*iter, "Unknown value '%s'", value);
    }
    
      //������ ���������� ��������� ���������
    void ParseLight (Parser::Iterator light_iter, unsigned int base_offset)
    {
      static const Tag2Value light_types [] = {
        {"Point",  LightType_Point},
        {"Remote", LightType_Remote},
        {"Spot",   LightType_Spot},
        {0, 0}
      };
      
      ParseEnum          (light_iter, "Type",                 base_offset + offsetof (LightDesc, type), light_types);
      ParseIntegerValues (light_iter, "Enable",               base_offset + offsetof (LightDesc, enable), 1);
      ParseVector3f      (light_iter, "Position",             base_offset + offsetof (LightDesc, position));
      ParseVector3f      (light_iter, "Direction",            base_offset + offsetof (LightDesc, direction));
      ParseVector4f      (light_iter, "AmbientColor",         base_offset + offsetof (LightDesc, ambient_color));
      ParseVector4f      (light_iter, "DiffuseColor",         base_offset + offsetof (LightDesc, diffuse_color));
      ParseVector4f      (light_iter, "SpecularColor",        base_offset + offsetof (LightDesc, specular_color));
      ParseFloatValues   (light_iter, "Angle",                base_offset + offsetof (LightDesc, angle), 1);
      ParseFloatValues   (light_iter, "Exponent",             base_offset + offsetof (LightDesc, exponent), 1);
      ParseFloatValues   (light_iter, "ContantAttenuation",   base_offset + offsetof (LightDesc, constant_attenuation), 1);
      ParseFloatValues   (light_iter, "LinearAttenuation",    base_offset + offsetof (LightDesc, linear_attenuation), 1);
      ParseFloatValues   (light_iter, "QuadraticAttenuation", base_offset + offsetof (LightDesc, quadratic_attenuation), 1);
    }    
  
      //������ ���������� ��������������
    void ParseTexmap (Parser::Iterator texmap_iter, unsigned int base_offset)
    {
      ParseMatrix4f (texmap_iter, "Transform", base_offset + (unsigned int)offsetof (TexmapDesc, transform));
      ParseMatrix4f (texmap_iter, "Texgen",    base_offset + (unsigned int)offsetof (TexmapDesc, transform));
      ParseVector4f (texmap_iter, "TexgenU",   base_offset + (unsigned int)offsetof (TexmapDesc, transform [0][0]));
      ParseVector4f (texmap_iter, "TexgenV",   base_offset + (unsigned int)offsetof (TexmapDesc, transform [1][0]));
      ParseVector4f (texmap_iter, "TexgenW",   base_offset + (unsigned int)offsetof (TexmapDesc, transform [2][0]));
      
      static const Tag2Value texcoord_sources [] = {
        {"Explicit",      TexcoordSource_Explicit},
        {"SphereMap",     TexcoordSource_SphereMap},
        {"ReflectionMap", TexcoordSource_ReflectionMap},
        {"ObjectSpace",   TexcoordSource_ObjectSpace},
        {"ViewerSpace",   TexcoordSource_ViewerSpace},
        {0, 0}
      };

      ParseEnum (texmap_iter, "TexcoordU", base_offset + offsetof (TexmapDesc, source_u), texcoord_sources);
      ParseEnum (texmap_iter, "TexcoordV", base_offset + offsetof (TexmapDesc, source_v), texcoord_sources);
      ParseEnum (texmap_iter, "TexcoordW", base_offset + offsetof (TexmapDesc, source_w), texcoord_sources);
      
      static const Tag2Value texture_blend_modes [] = {
        {"Replace",  TextureBlend_Replace},
        {"Modulate", TextureBlend_Modulate},
        {"Blend",    TextureBlend_Blend},
        {0, 0}
      };
      
      ParseEnum (texmap_iter, "Blend", base_offset + offsetof (TexmapDesc, blend), texture_blend_modes);
    }
  
      //������ �������
    void ParseShader ()
    {
      Parser::Iterator program_iter = parser.Root ();

      if (!program_iter)
        return;
        
        //������ ���������� ����������
        
      Parser::Iterator params_iter = program_iter->First ("Parameters");
        
      if (params_iter)
      {        
        ParseParameterDeclarations (params_iter, "float",    FfpDynamicParameterType_Float, 1);
        ParseParameterDeclarations (params_iter, "float3",   FfpDynamicParameterType_Float, 3);
        ParseParameterDeclarations (params_iter, "float4",   FfpDynamicParameterType_Float, 4);
        ParseParameterDeclarations (params_iter, "float4x4", FfpDynamicParameterType_Float, 16);
        ParseParameterDeclarations (params_iter, "int",      FfpDynamicParameterType_Int, 1);
        ParseParameterDeclarations (params_iter, "int3",     FfpDynamicParameterType_Int, 3);
        ParseParameterDeclarations (params_iter, "int4",     FfpDynamicParameterType_Int, 4);
        ParseParameterDeclarations (params_iter, "int4x4",   FfpDynamicParameterType_Int, 16);

        if (params_iter->NextNamesake ())
          parser.Log ().Warning (params_iter->NextNamesake (), "Second (and others) 'Parameters' block(s) ignored");
      }
      
        //������ ���������� ������������

      ParseFloatValues   (program_iter, "PointSize",          offsetof (FfpState, rasterization.point_size), 1);
      ParseFloatValues   (program_iter, "LineWidth",          offsetof (FfpState, rasterization.line_width), 1);
      ParseIntegerValues (program_iter, "LineStippleFactor",  offsetof (FfpState, rasterization.line_stipple_factor), 1);
      ParseIntegerValues (program_iter, "LineStipplePattern", offsetof (FfpState, rasterization.line_stipple_pattern), 1);


        //������ ���������� �������������

      ParseMatrix4f (program_iter, "ProjectionMatrix", offsetof (FfpState, viewer.projection_matrix));
      ParseMatrix4f (program_iter, "ViewMatrix", offsetof (FfpState, viewer.view_matrix));
      ParseMatrix4f (program_iter, "ObjectMatrix", offsetof (FfpState, object.matrix));
      
        //������ ���������� ���������
      
      ParseVector4f    (program_iter, "EmissionColor", offsetof (FfpState, material.emission_color));
      ParseVector4f    (program_iter, "AmbientColor",  offsetof (FfpState, material.ambient_color));
      ParseVector4f    (program_iter, "DiffuseColor",  offsetof (FfpState, material.diffuse_color));
      ParseVector4f    (program_iter, "SpecularColor", offsetof (FfpState, material.specular_color));
      ParseFloatValues (program_iter, "Shininess",     offsetof (FfpState, material.shininess), 1);
      
      static const Tag2Value color_material_modes [] = {
        {"Explicit",          ColorMaterial_Explicit},
        {"Emission",          ColorMaterial_Emission},
        {"Ambient",           ColorMaterial_Ambient},
        {"Diffuse",           ColorMaterial_Diffuse},
        {"Specular",          ColorMaterial_Specular},
        {"AmbientAndDiffuse", ColorMaterial_AmbientAndDiffuse},
        {0, 0}
      };

      ParseEnum (program_iter, "ColorMaterial", offsetof (FfpState, material.color_material), color_material_modes);

      static const Tag2Value compare_modes [] = {
        {"AlwaysFail",   CompareMode_AlwaysFail},
        {"AlwaysPass",   CompareMode_AlwaysPass},
        {"Equal",        CompareMode_Equal},
        {"NotEqual",     CompareMode_NotEqual},
        {"Less",         CompareMode_Less},
        {"LessEqual",    CompareMode_LessEqual},
        {"Greater",      CompareMode_Greater},
        {"GreaterEqual", CompareMode_GreaterEqual},
        {0, 0}
      };

      ParseEnum          (program_iter, "AlphaCompareMode", offsetof (FfpState, material.alpha_compare_mode), compare_modes);
      ParseFloatValues   (program_iter, "AlphaReference",   offsetof (FfpState, material.alpha_reference), 1);
      ParseIntegerValues (program_iter, "Normalize",        offsetof (FfpState, modes.normalize), 1);

        //������ ���������� ���������
        
      unsigned int light_index = 0;
        
      for (Parser::Iterator light_iter = program_iter->First ("Light"); light_iter && light_index < FFP_MAX_LIGHTS_COUNT; ++light_iter, ++light_index)
      {
        ParseLight (light_iter, (unsigned int)offsetof (FfpState, lights [0]) + light_index * sizeof (LightDesc));
      }

        //������ ���������� ���������������

      for (unsigned int i=0; i<DEVICE_SAMPLER_SLOTS_COUNT; i++)
      {
        char texmap_name [32];
        
        xtl::xsnprintf (texmap_name, sizeof texmap_name, "Texmap%u", i);
        
        Parser::Iterator texmap_iter = program_iter->First (texmap_name);
        
        if (texmap_iter)
          ParseTexmap (texmap_iter, (unsigned int)offsetof (FfpState, maps [0]) + i * sizeof (TexmapDesc));
      }
    }    

  private:
    FfpState&               base_state;         //������� ��������� ffp-�������
    FfpDynamicParameterMap& dynamic_parameters; //������������ ���������
    Parser                  parser;             //������ ��������� ������ ffp-�������
};

}

/*
    ����������� / ����������
*/

FfpProgram::FfpProgram (const ContextManager& manager, const ShaderDesc& shader_desc, const LogFunction& error_log)
  : ContextObject (manager)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FfpProgram::FfpProgram";

    //������������� ��������� ffp-�������
    
  memset (&base_state, 0, sizeof base_state);
  
  identity_matrix (base_state.viewer.projection_matrix);
  identity_matrix (base_state.viewer.view_matrix);
  identity_matrix (base_state.object.matrix);

  base_state.material.alpha_compare_mode = CompareMode_AlwaysPass;

  for (unsigned int i=0; i<FFP_MAX_LIGHTS_COUNT; i++)
  {
    LightDesc& light = base_state.lights [i];
    
    light.constant_attenuation = 1.0f;
  }
  
  for (unsigned int i=0; i<DEVICE_SAMPLER_SLOTS_COUNT; i++)
  {
    TexmapDesc& texmap = base_state.maps [i];

    identity_matrix (texmap.transform);
    identity_matrix (texmap.texgen);
  }

  base_state.rasterization.point_size           = 1.0f;  
  base_state.rasterization.line_width           = 1.0f;
  base_state.rasterization.line_stipple_pattern = ~0;
  base_state.rasterization.line_stipple_factor  = 1;

    //������ ffp-�������

  if (!shader_desc.profile)
    throw xtl::make_null_argument_exception (METHOD_NAME, "shader_desc.profile");

  if (!shader_desc.source_code)
    throw xtl::make_null_argument_exception (METHOD_NAME, "shader_desc.source_code");

  if (!shader_desc.name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "shader_desc.name");    

  error_log (format ("Compiling '%s'...", shader_desc.name).c_str ());

  FfpProgramParser (base_state, dynamic_parameters, shader_desc, error_log);
}

FfpProgram::~FfpProgram ()
{
}

/*
    ������ � ������������� �����������
*/

const FfpDynamicParameter* FfpProgram::FindDynamicParameter (const char* name) const
{
  if (!name)
    return 0;
    
  FfpDynamicParameterMap::const_iterator iter = dynamic_parameters.find (name);
  
  if (iter == dynamic_parameters.end ())
    return 0;
    
  return &iter->second;
}

/*
    �������� ���������, ��������������� � �������� OpenGL
*/

IBindableProgram* FfpProgram::CreateBindableProgram (ProgramParametersLayout* layout)
{
  try
  {
    return new FfpBindableProgram (GetContextManager (), *this, layout);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::FfpProgram::CreateBindableProgram");
    throw;
  }
}
