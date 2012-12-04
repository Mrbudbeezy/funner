#include "shared.h"

using namespace render::mid_level;
using namespace render::low_level;
using namespace render::mid_level::window_driver;

/*
    ����������� / ����������
*/

LowLevelFrame::LowLevelFrame ()
  : draw_callback (0)
{
}

LowLevelFrame::~LowLevelFrame ()
{
}

/*
    ��������� ���������� ��������� ������
*/

void LowLevelFrame::SetCallback (IDrawCallback* new_draw_callback)
{
  draw_callback = new_draw_callback;
}

/*
    ���������� ������������
*/

void LowLevelFrame::DrawCore (render::low_level::IDevice* device, render::low_level::IDeviceContext* device_context)
{
  if (!draw_callback)
    return; //������������ �� ���������

    //��������� ������� ������
    
  int viewport_offset_x = 0, viewport_offset_y = 0;

  BasicFrame::BindViewport (device, device_context, viewport_offset_x, viewport_offset_y);

    //������������

  draw_callback->Draw (*device);
}
