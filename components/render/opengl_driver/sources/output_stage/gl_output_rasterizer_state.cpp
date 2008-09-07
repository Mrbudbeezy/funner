#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   �����������
*/

RasterizerState::RasterizerState (const ContextManager& manager, const RasterizerDesc& in_desc)
  : ContextObject (manager),
    desc_hash (0)
{
  SetDesc (in_desc);
}

RasterizerState::~RasterizerState ()
{
}

/*
   ������� ���������
*/

void RasterizerState::Bind ()
{
  static const char* METHOD_NAME = "render::low_level::opengl::RasterizerState::Bind";
  
    //�������� ������������� �������� (����������� ���������)

  size_t *state_cache            = &GetContextDataTable (Stage_Output)[0],
         *state_common_cache     = &GetContextDataTable (Stage_Common)[0],
         &current_desc_hash      = state_cache [OutputStageCache_RasterizerStateHash],
         &current_scissor_enable = state_cache [CommonCache_ScissorEnable];

  if (current_desc_hash == desc_hash)
    return;

    //��������� ��������� � �������� OpenGL

  MakeContextCurrent ();  
  
    //���������� ������

  executer->ExecuteCommands ();

    //�������� ������

  CheckErrors (METHOD_NAME);

    //��������� ���-����������

  current_desc_hash      = desc_hash;
  current_scissor_enable = desc.scissor_enable;
}

/*
    ��������� �����������
*/

void RasterizerState::GetDesc (RasterizerDesc& out_desc)
{
  out_desc = desc;
}

/*
   ��������� �����������
*/

void RasterizerState::SetDesc (const RasterizerDesc& in_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::RasterizerState::SetDesc";
  
    //�������� ������� ����������� ����������
    
  const ContextCaps& caps = GetCaps ();

  if (in_desc.multisample_enable && !caps.has_arb_multisample)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Multisampling not supported (GL_ARB_multisample extension not supported)");
    
    //�������� ������������ ���������� � �������������� �����������

  GLenum gl_fill_mode;

  switch (in_desc.fill_mode)
  {
    case FillMode_Wireframe: gl_fill_mode = GL_LINE; break;
    case FillMode_Solid:     gl_fill_mode = GL_FILL; break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.fill_mode", in_desc.fill_mode);
      break;
  }

  switch (in_desc.cull_mode)
  {
    case CullMode_None:
    case CullMode_Front:
    case CullMode_Back:
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.cull_mode", in_desc.cull_mode);
      break;
  }

    //����� �������� ���������

  MakeContextCurrent ();  

    //������ ������ � �������� OpenGL
    
  CommandListBuilder cmd_list;

  cmd_list.Add (glPolygonMode, GL_FRONT_AND_BACK, gl_fill_mode);

  switch (in_desc.cull_mode)
  {
    case CullMode_None:
      cmd_list.Add (glDisable, GL_CULL_FACE);
      break;
    case CullMode_Front:
      cmd_list.Add (glEnable, GL_CULL_FACE);
      cmd_list.Add (glCullFace, GL_FRONT);
      break;
    case CullMode_Back:
      cmd_list.Add (glEnable, GL_CULL_FACE);
      cmd_list.Add (glCullFace, GL_BACK);
      break;
  }

  cmd_list.Add (glFrontFace, in_desc.front_counter_clockwise ? GL_CW : GL_CCW);

  if (in_desc.depth_bias)
  {
    cmd_list.Add (glEnable, GL_POLYGON_OFFSET_FILL);
    cmd_list.Add (glEnable, GL_POLYGON_OFFSET_LINE);
    cmd_list.Add (glEnable, GL_POLYGON_OFFSET_POINT);
    cmd_list.Add (glPolygonOffset, 0.f, (float)in_desc.depth_bias);
  }
  else
  {
    cmd_list.Add (glDisable, GL_POLYGON_OFFSET_FILL);
    cmd_list.Add (glDisable, GL_POLYGON_OFFSET_LINE);
    cmd_list.Add (glDisable, GL_POLYGON_OFFSET_POINT);
  }  

  if (in_desc.scissor_enable) cmd_list.Add (glEnable, GL_SCISSOR_TEST);
  else                        cmd_list.Add (glDisable, GL_SCISSOR_TEST);

  if (in_desc.antialiased_line_enable && !in_desc.multisample_enable)
  {
    cmd_list.Add (glEnable, GL_POINT_SMOOTH);
    cmd_list.Add (glEnable, GL_POLYGON_SMOOTH);
    cmd_list.Add (glEnable, GL_LINE_SMOOTH);
  }
  else
  {
    cmd_list.Add (glDisable, GL_POINT_SMOOTH);
    cmd_list.Add (glDisable, GL_LINE_SMOOTH);
    cmd_list.Add (glDisable, GL_POLYGON_SMOOTH);
  }

  if (caps.has_arb_multisample && in_desc.multisample_enable)
    cmd_list.Add (glEnable, GL_MULTISAMPLE_ARB);
    
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
