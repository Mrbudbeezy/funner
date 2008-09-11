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

void LowLevelFrame::DrawCore (render::low_level::IDevice* device)
{
  if (!draw_callback)
    return; //������������ �� ���������

    //��������� ������� ������

  BasicFrame::BindViewport (device);

    //������������

  draw_callback->Draw (*device);
}
