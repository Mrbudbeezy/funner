#include "shared.h"

//�������� GL_NV_blend_squaring (gl 1.4)
//�������� blend-constant-color (gl 1.4)

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    ����������� / ����������
*/

BlendState::BlendState (const ContextManager& context_manager, const BlendDesc& in_desc)
  : ContextObject (context_manager),
    desc_hash (0)
{
  SetDesc (in_desc);
}

BlendState::~BlendState ()
{
}

/*
    ��������� / ��������� �����������
*/

namespace
{

void check_blend_operation (BlendOperation operation, const ContextCaps& caps, const char* method, const char* param)
{
  switch (operation)
  {
    case BlendOperation_Add:
      break;
    case BlendOperation_Subtraction:
    case BlendOperation_ReverseSubtraction:
      if (!caps.has_ext_blend_subtract)
        throw xtl::format_not_supported_exception (method, "Unsupported blend operation %s=%s (GL_EXT_blend_subtract not supported)", param, get_name (operation));

      break;
    case BlendOperation_Min:
    case BlendOperation_Max:
      if (!caps.has_ext_blend_minmax)
        throw xtl::format_not_supported_exception (method, "Unsupported blend operation %s=%s (GL_EXT_blend_minmax not supported)", param, get_name (operation));

      break;      
    default:
      break;
  }
}

GLenum get_blend_equation (BlendOperation operation, const char* method, const char* param)
{
  switch (operation)
  {
    case BlendOperation_Add:                return GL_FUNC_ADD_EXT;
    case BlendOperation_Subtraction:        return GL_FUNC_SUBTRACT_EXT;
    case BlendOperation_ReverseSubtraction: return GL_FUNC_REVERSE_SUBTRACT_EXT;
    case BlendOperation_Min:                return GL_MIN_EXT;
    case BlendOperation_Max:                return GL_MAX_EXT;
    default:
      throw xtl::make_argument_exception (method, param, operation);
  }
}

GLenum get_alpha_equivalent (GLenum color_arg)
{
  switch (color_arg)
  {
    default:
    case GL_ZERO:                return GL_ZERO;
    case GL_ONE:                 return GL_ONE;
    case GL_SRC_COLOR:
    case GL_SRC_ALPHA:           return GL_SRC_ALPHA;
    case GL_ONE_MINUS_SRC_COLOR:
    case GL_ONE_MINUS_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
    case GL_DST_COLOR:
    case GL_DST_ALPHA:           return GL_DST_ALPHA;
    case GL_ONE_MINUS_DST_COLOR:
    case GL_ONE_MINUS_DST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
  }
}

}

void BlendState::SetDesc (const BlendDesc& in_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::BlendState::SetDesc";
  
    //����� ���������

  MakeContextCurrent ();
  
    //����������� ��������� ���������� OpenGL
    
  const ContextCaps& caps = GetCaps ();

    //�������������� ������ �����������
    
  GLenum color_blend_equation = get_blend_equation (in_desc.blend_color_operation, METHOD_NAME, "desc.blend_color_operation"),
         alpha_blend_equation = get_blend_equation (in_desc.blend_alpha_operation, METHOD_NAME, "desc.blend_alpha_operation");

  GLenum src_color_arg, dst_color_arg;
  GLenum src_alpha_arg, dst_alpha_arg;
  
  switch (in_desc.blend_color_source_argument)
  {
    case BlendArgument_Zero:                    src_color_arg = GL_ZERO; break;
    case BlendArgument_One:                     src_color_arg = GL_ONE; break;
    case BlendArgument_SourceAlpha:             src_color_arg = GL_SRC_ALPHA; break;
    case BlendArgument_InverseSourceAlpha:      src_color_arg = GL_ONE_MINUS_SRC_ALPHA; break;
    case BlendArgument_DestinationColor:        src_color_arg = GL_DST_COLOR; break;
    case BlendArgument_DestinationAlpha:        src_color_arg = GL_DST_ALPHA; break;    
    case BlendArgument_InverseDestinationColor: src_color_arg = GL_ONE_MINUS_DST_COLOR; break;    
    case BlendArgument_InverseDestinationAlpha: src_color_arg = GL_ONE_MINUS_DST_ALPHA; break;
    case BlendArgument_SourceColor:
    case BlendArgument_InverseSourceColor:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.blend_color_source_argument", get_name (in_desc.blend_color_source_argument));
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.blend_color_source_argument", in_desc.blend_color_source_argument);
      break;
  }

  switch (in_desc.blend_color_destination_argument)
  {
    case BlendArgument_Zero:                    dst_color_arg = GL_ZERO; break;
    case BlendArgument_One:                     dst_color_arg = GL_ONE; break;
    case BlendArgument_SourceColor:             dst_color_arg = GL_SRC_COLOR; break;
    case BlendArgument_SourceAlpha:             dst_color_arg = GL_SRC_ALPHA; break;
    case BlendArgument_InverseSourceColor:      dst_color_arg = GL_ONE_MINUS_SRC_COLOR; break;
    case BlendArgument_InverseSourceAlpha:      dst_color_arg = GL_ONE_MINUS_SRC_ALPHA; break;    
    case BlendArgument_DestinationAlpha:        dst_color_arg = GL_DST_ALPHA; break;
    case BlendArgument_InverseDestinationAlpha: dst_color_arg = GL_ONE_MINUS_DST_ALPHA; break;
    case BlendArgument_DestinationColor:
    case BlendArgument_InverseDestinationColor:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.blend_color_destination_argument", get_name (in_desc.blend_color_destination_argument));
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.blend_color_destination_argument", in_desc.blend_color_destination_argument);
      break;    
  }

  switch (in_desc.blend_alpha_source_argument)
  {
    case BlendArgument_Zero:                    src_alpha_arg = GL_ZERO; break;
    case BlendArgument_One:                     src_alpha_arg = GL_ONE; break;
    case BlendArgument_SourceAlpha:             src_alpha_arg = GL_SRC_ALPHA; break;
    case BlendArgument_InverseSourceAlpha:      src_alpha_arg = GL_ONE_MINUS_SRC_ALPHA; break;
    case BlendArgument_DestinationAlpha:        src_alpha_arg = GL_DST_ALPHA; break;
    case BlendArgument_InverseDestinationAlpha: src_alpha_arg = GL_ONE_MINUS_DST_ALPHA; break;
    case BlendArgument_SourceColor:
    case BlendArgument_InverseSourceColor:
    case BlendArgument_DestinationColor:
    case BlendArgument_InverseDestinationColor:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.blend_alpha_source_argument", get_name (in_desc.blend_alpha_source_argument));
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.blend_alpha_source_argument", in_desc.blend_alpha_source_argument);
      break;
  }  
  
  switch (in_desc.blend_alpha_destination_argument)
  {
    case BlendArgument_Zero:                    dst_alpha_arg = GL_ZERO; break;
    case BlendArgument_One:                     dst_alpha_arg = GL_ONE; break;
    case BlendArgument_SourceAlpha:             dst_alpha_arg = GL_SRC_ALPHA; break;
    case BlendArgument_InverseSourceAlpha:      dst_alpha_arg = GL_ONE_MINUS_SRC_ALPHA; break;    
    case BlendArgument_DestinationAlpha:        dst_alpha_arg = GL_DST_ALPHA; break;
    case BlendArgument_InverseDestinationAlpha: dst_alpha_arg = GL_ONE_MINUS_DST_ALPHA; break;
    case BlendArgument_SourceColor:
    case BlendArgument_InverseSourceColor:
    case BlendArgument_DestinationColor:
    case BlendArgument_InverseDestinationColor:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.blend_alpha_destination_argument", get_name (in_desc.blend_alpha_destination_argument));
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.blend_alpha_destination_argument", in_desc.blend_alpha_destination_argument);
      break;    
  }
  
  if (!caps.has_arb_multisample && in_desc.sample_alpha_to_coverage) 
    throw xtl::format_not_supported_exception (METHOD_NAME, "Can't enable sample alpha to coverage mode (GL_ARB_multisample extension not supported)");

    //�������� ��������� ����������
    
  if (in_desc.blend_enable && (in_desc.color_write_mask & ColorWriteFlag_All))
  {  
    if (color_blend_equation != alpha_blend_equation && !caps.has_ext_blend_equation_separate)
      throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported configuration: desc.blend_color_operation=%s mismatch desc.blend_alpha_operation=%s"
        " (GL_EXT_blend_equation_separate not supported)", get_name (in_desc.blend_color_operation), get_name (in_desc.blend_alpha_operation));
    
    if (!caps.has_ext_blend_func_separate)
    {            
      if (get_alpha_equivalent (src_color_arg) != src_alpha_arg)
      {
        throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported configuration: desc.blend_color_source_argument=%s mismatch desc.blend_alpha_source_argument=%s (GL_EXT_blend_func_separate not supported)"
          " (GL_EXT_blend_func_separate not supported)", get_name (in_desc.blend_color_source_argument), get_name (in_desc.blend_alpha_source_argument));
      }

      if (get_alpha_equivalent (dst_color_arg) != dst_alpha_arg)
        throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported configuration: desc.blend_color_destination_argument=%s mismatch desc.blend_alpha_destination_argument=%s (GL_EXT_blend_func_separate not supported)"
        " (GL_EXT_blend_func_separate not supported)", get_name (in_desc.blend_color_destination_argument), get_name (in_desc.blend_alpha_destination_argument));
    }

    check_blend_operation (in_desc.blend_color_operation, caps, METHOD_NAME, "desc.blend_color_operation");
    check_blend_operation (in_desc.blend_alpha_operation, caps, METHOD_NAME, "desc.blend_alpha_operation");
  }

    //������ ������ � ��������� OpenGL
    
  CommandListBuilder cmd_list;    
  
  size_t mask = in_desc.color_write_mask;

  cmd_list.Add (glColorMask, (mask & ColorWriteFlag_Red) != 0, (mask & ColorWriteFlag_Green) != 0, (mask & ColorWriteFlag_Blue) != 0,
                (mask & ColorWriteFlag_Alpha) != 0);

  if (in_desc.blend_enable && (mask & ColorWriteFlag_All))
  {
    cmd_list.Add (glEnable, GL_BLEND);

    if      (glBlendEquationSeparate)    cmd_list.Add (glBlendEquationSeparate, color_blend_equation, alpha_blend_equation);
    else if (glBlendEquationSeparateEXT) cmd_list.Add (glBlendEquationSeparateEXT, color_blend_equation, alpha_blend_equation);
    else if (glBlendEquation)            cmd_list.Add (glBlendEquation, color_blend_equation);
    else if (glBlendEquationEXT)         cmd_list.Add (glBlendEquationEXT, color_blend_equation);

    if      (glBlendFuncSeparate)    cmd_list.Add (glBlendFuncSeparate, src_color_arg, dst_color_arg, src_alpha_arg, dst_alpha_arg);
    else if (glBlendFuncSeparateEXT) cmd_list.Add (glBlendFuncSeparateEXT, src_color_arg, dst_color_arg, src_alpha_arg, dst_alpha_arg);
    else                             cmd_list.Add (glBlendFunc, src_color_arg, dst_color_arg);      
  }
  else
  {
    cmd_list.Add (glDisable, GL_BLEND);
  }
  
  if (caps.has_arb_multisample)
  {
    if (in_desc.sample_alpha_to_coverage) cmd_list.Add (glEnable, GL_SAMPLE_ALPHA_TO_COVERAGE);
    else                                  cmd_list.Add (glDisable, GL_SAMPLE_ALPHA_TO_COVERAGE);
  }  
  
    //�������� ����������� ������    

  ExecuterPtr new_executer = cmd_list.Finish ();  

    //�������� ������

  CheckErrors (METHOD_NAME);
  
    //���������� ����������

  desc      = in_desc;
  desc_hash = crc32 (&desc, sizeof desc);
  executer  = new_executer;

    //���������� � ������������� ���������� ������

  StageRebindNotify (Stage_Output);
}

void BlendState::GetDesc (BlendDesc& out_desc)
{
  out_desc = desc;
}

/*
    ��������� ��������� � �������� OpenGL
*/

void BlendState::Bind ()
{
  static const char* METHOD_NAME = "render::low_level::opengl::BlendState::Bind";

    //�������� ������������� �������� (����������� ���������)
    
  size_t *state_cache              = &GetContextDataTable (Stage_Output)[0],
         *state_common_cache       = &GetContextDataTable (Stage_Common)[0],
         &current_desc_hash        = state_cache [OutputStageCache_BlendStateHash],
         &current_color_write_mask = state_common_cache [CommonCache_ColorWriteMask];

  if (current_desc_hash == desc_hash)
    return;

    //��������� ��������� � �������� OpenGL

  MakeContextCurrent ();  
  
    //���������� ������ 

  executer->ExecuteCommands ();

    //�������� ������

  CheckErrors (METHOD_NAME);

    //��������� ���-����������

  current_desc_hash        = desc_hash;
  current_color_write_mask = desc.color_write_mask;
}
