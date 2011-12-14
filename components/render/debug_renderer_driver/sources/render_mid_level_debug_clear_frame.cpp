#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::debug;

/*
    �����������
*/

ClearFrame::ClearFrame ()
  : clear_flags (ClearFlag_All),
    clear_depth_value (1.0f),
    clear_stencil_index (0)
{
}

/*
    ����� ������� �����
*/

void ClearFrame::SetFlags (size_t in_clear_flags)
{
  clear_flags = in_clear_flags;
}

/*
    ��������� ������� ������ �����
*/

void ClearFrame::SetColor (const math::vec4f& color)
{
  clear_color = color;
}

void ClearFrame::GetColor (math::vec4f& out_color)
{
  out_color = clear_color;
}

/*
    ��������� ������� ������ ������������� ���������
*/

void ClearFrame::SetDepthValue (float depth_value)
{
  clear_depth_value = depth_value;
}

void ClearFrame::SetStencilIndex (unsigned char stencil_index)
{
  clear_stencil_index = stencil_index;
}

/*
    ���������� ������������
*/

void ClearFrame::DrawCore ()
{
    //���� ��������� ������� ������ ������� ������

  if (clear_flags & ClearFlag_ViewportOnly)
    BasicFrame::BindViewport ();

    //������� ������� ������� ���������

  if ((clear_flags & ClearFlag_RenderTarget) && GetRenderTarget ())
    log.Printf ("Clear render target [%.2f %.2f %.2f %.2f]", clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    
  if (GetDepthStencilTarget ())
  {
    switch (clear_flags & ClearFlag_DepthStencil)
    {
      case ClearFlag_DepthStencil:
        log.Printf ("Clear depth-stencil target [%.3f %u]", clear_depth_value, clear_stencil_index);
        break;
      case ClearFlag_Depth:
        log.Printf ("Clear depth-stencil target [%.3f no-clear]", clear_depth_value);
        break;
      case ClearFlag_Stencil:
        log.Printf ("Clear depth-stencil target [no-clear %u]", clear_stencil_index);
        break;        
    }
  }
}
