#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::debug;

/*
    �����������
*/

ViewportFrame::ViewportFrame ()
{
  memset (&viewport, 0, sizeof viewport);
}

/*
    ��������� ������� ������
*/

void ViewportFrame::SetViewport (const Viewport& in_viewport)
{
  viewport = in_viewport;
}

void ViewportFrame::GetViewport (Viewport& out_viewport)
{
  out_viewport = viewport;
}

/*
    ������������
*/

void ViewportFrame::DrawCore ()
{ 
    //��������� ������� ������
    
  log.Printf ("Set viewport (left=%d, top=%d, width=%u, height=%u)", viewport.x, viewport.y, viewport.width, viewport.height);
}
