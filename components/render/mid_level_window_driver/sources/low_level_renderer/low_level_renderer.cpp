#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::window_driver;

/*
    �������� ���������� �������������� ������� ������������
*/

struct LowLevelRenderer::Impl
{
};

/*
    ����������� / ����������
*/

LowLevelRenderer::LowLevelRenderer (render::low_level::IDevice*)
  : impl (new Impl)
{
}

LowLevelRenderer::~LowLevelRenderer ()
{
}

/*
    �������� ��������
*/

render::mid_level::ILowLevelFrame* LowLevelRenderer::CreateFrame ()
{
  return new LowLevelFrame;
}
