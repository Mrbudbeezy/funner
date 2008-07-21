#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::low_level_driver;

/*
    �����������
*/

BasicFrame::BasicFrame ()
{
}

/*
    ������� ������ ���������
*/

void BasicFrame::SetRenderTargets (IRenderTarget* in_render_target, IRenderTarget* in_depth_stencil_target)
{
  static const char* METHOD_NAME = "render::mid_level::low_level_driver::BasicFrame::SetRenderTargets";

  if (in_render_target)
  {
    RenderTarget* casted_render_target = dynamic_cast<RenderTarget*> (in_render_target);
    
    if (!casted_render_target)
      throw xtl::make_argument_exception (METHOD_NAME, "render_target", typeid (in_render_target).name (),
        "Render target type incompatible with render::mid_level::low_level_driver::RenderTarget");
        
    render_target = casted_render_target;
  }
  else render_target = 0;
  
  if (in_depth_stencil_target)
  {
    RenderTarget* casted_depth_stencil_target = dynamic_cast<RenderTarget*> (in_depth_stencil_target);
    
    if (!casted_depth_stencil_target)
      throw xtl::make_argument_exception (METHOD_NAME, "depth_stencil_target", typeid (in_depth_stencil_target).name (),
        "Depth-stencil target type incompatible with render::mid_level::low_level_driver::RenderTarget");
        
    depth_stencil_target = casted_depth_stencil_target;
  }
  else depth_stencil_target = 0;  
}

//������� ����� �����
IRenderTarget* BasicFrame::GetRenderTarget ()
{
  return render_target.get ();
}

//������� ����� ������������� ���������
IRenderTarget* BasicFrame::GetDepthStencilTarget ()
{
  return depth_stencil_target.get ();
}

/*
    ������������
*/

void BasicFrame::Draw (render::low_level::IDevice* device, const render::low_level::Rect& device_viewport)
{
  using namespace render::low_level;

  if (!render_target && !depth_stencil_target)
    return;
    
    //��������� ������� ������
    
  SetDeviceViewportCore (device, device_viewport);

    //���������� ���������

  DrawCore (device);
}
