#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    �����������
*/

StateBlock::StateBlock (Device& device, const StateBlockMask& in_mask)
  : mask (in_mask),
    output_stage_state (device.output_stage.CreateStateBlock ())
{
}

/*
    ��������� ����� �����
*/

void StateBlock::GetMask (StateBlockMask& out_mask)
{
  out_mask = mask;
}

/*
    ������ �������� ����������
*/

void StateBlock::Capture ()
{
  output_stage_state->Capture (mask);
}

/*
    ���������� �������� ����������
*/

void StateBlock::Apply ()
{
  output_stage_state->Apply (mask);
}
