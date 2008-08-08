#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::debug;

/*
    ���������
*/

namespace
{

const size_t FRAME_ARRAY_RESERVE_SIZE = 128; //������������� ����� ������

}

/*
    �����������
*/

BasicRenderer::BasicRenderer ()  
  : frame_id (0)
{
  color_buffer         = RenderTargetPtr (CreateRenderBuffer (SCREEN_WIDTH, SCREEN_HEIGHT), false);
  depth_stencil_buffer = RenderTargetPtr (CreateDepthStencilBuffer (SCREEN_WIDTH, SCREEN_HEIGHT), false);  
  
  frames.reserve (FRAME_ARRAY_RESERVE_SIZE);
}

/*
   ��������
*/

const char* BasicRenderer::GetDescription ()
{
  return "render::mid_level::debug::BasicRenderer";
}

/*
    ���������� ������� �����
*/

size_t BasicRenderer::GetFrameBuffersCount ()
{
  return 1;
}

/*
    ��������� ������ ����� � ������ ������������� ���������
*/

IRenderTarget* BasicRenderer::GetColorBuffer (size_t frame_buffer_index)
{
  if (frame_buffer_index)
    throw xtl::make_range_exception ("render::mid_level::debug::BasicRenderer::GetColorBuffer", "frame_buffer_index", frame_buffer_index, 1u);

  return color_buffer.get ();
}

IRenderTarget* BasicRenderer::GetDepthStencilBuffer (size_t frame_buffer_index)
{
  if (frame_buffer_index)
    throw xtl::make_range_exception ("render::mid_level::debug::BasicRenderer::GetDepthStencilBuffer", "frame_buffer_index", frame_buffer_index, 1u);

  return depth_stencil_buffer.get ();
}

/*
    �������� ��������
*/

IRenderTarget* BasicRenderer::CreateDepthStencilBuffer (size_t width, size_t height)
{
  return new RenderTarget (width, height, RenderTargetType_DepthStencil);
}

IRenderTarget* BasicRenderer::CreateRenderBuffer (size_t width, size_t height)
{
  return new RenderTarget (width, height, RenderTargetType_Color);
}

IClearFrame* BasicRenderer::CreateClearFrame ()
{
  return new ClearFrame;
}

/*
   ���������� ����� � ������ ���������
*/

void BasicRenderer::AddFrame (IFrame* in_frame)
{
  static const char* METHOD_NAME = "render::mid_level::debug::BasicRenderer::AddFrame";

  if (!in_frame)
    throw xtl::make_null_argument_exception (METHOD_NAME, "frame");
    
  BasicFrame* frame = dynamic_cast<BasicFrame*> (in_frame);
  
  if (!frame)  
    throw xtl::make_argument_exception (METHOD_NAME, "frame", typeid (in_frame).name (),
      "Frame type incompatible with render::mid_level::debug::BasicFrame");
      
  frames.push_back (frame);
}

/*
    ����� ��������� / ����� ���������
*/

void BasicRenderer::DrawFrames ()
{
  if (frames.empty ())
    return;
        
    //������ ���������
    
  log.Printf ("Begin draw frames (id=%u, frame_id=%u)", Id (), frame_id);

    //��������� ������

  for (FrameArray::iterator iter=frames.begin (), end=frames.end (); iter!=end; ++iter)
  {
    BasicFrame& frame = **iter;
    
    frame.Draw ();
  }
  
    //������� ������ ������
    
  frames.clear ();
  
    //����� ���������
    
  log.Printf ("End draw frames (id=%u, frame_id=%u)", Id (), frame_id);
    
    //���������� �������� ������
    
  frame_id++;
}

void BasicRenderer::CancelFrames ()
{
  frames.clear ();
  
  log.Printf ("Renderer cancel frames (id=%u)", Id ());
}
