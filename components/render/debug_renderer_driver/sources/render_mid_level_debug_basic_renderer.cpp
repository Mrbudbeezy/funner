#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::debug;

/*
    �����������
*/

BasicRenderer::BasicRenderer ()  
  : frame_position (frames.end ()),
    frames_count (0),
    frame_id (0)
{
  color_buffer         = RenderTargetPtr (CreateRenderBuffer (SCREEN_WIDTH, SCREEN_HEIGHT), false);
  depth_stencil_buffer = RenderTargetPtr (CreateDepthStencilBuffer (SCREEN_WIDTH, SCREEN_HEIGHT), false);  
}

BasicRenderer::~BasicRenderer ()
{
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

namespace
{

//����� ����������
class RenderBuffer: public RenderTarget
{
  public:
    RenderBuffer (size_t width, size_t height, RenderTargetType type) : RenderTarget (width, height, type)
    {
      log.Printf ("Create %s %ux%u (id=%u)", type == RenderTargetType_Color ? "color-buffer" : "depth-stencil-buffer", width, height, Id ());
    }
    
    ~RenderBuffer ()
    {
      try
      {
        log.Printf ("Destroy %s (id=%u)", GetType () == RenderTargetType_Color ? "color-buffer" : "depth-stencil-buffer", Id ());
      }
      catch (...)
      {
        //���������� ���� ����������
      }      
    }
};

}

IRenderTarget* BasicRenderer::CreateDepthStencilBuffer (size_t width, size_t height)
{
  return new RenderBuffer (width, height, RenderTargetType_DepthStencil);
}

IRenderTarget* BasicRenderer::CreateRenderBuffer (size_t width, size_t height)
{
  return new RenderBuffer (width, height, RenderTargetType_Color);
}

IClearFrame* BasicRenderer::CreateClearFrame ()
{
  return new ClearFrame;
}

/*
    ���������� ������ / ������� ������� ���������� �����
*/

size_t BasicRenderer::FramesCount ()
{
  return frames_count;
}

void BasicRenderer::SetFramePosition (size_t position)
{
  if (position > frames_count)
    position = frames_count;

  frame_position = frames.begin ();
  
  advance (frame_position, position);
}

size_t BasicRenderer::GetFramePosition ()
{
  return size_t (distance (frames.begin (), frame_position));
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

  frames.insert (frame_position, frame);  

  frames_count++;
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

  for (FrameList::iterator iter=frames.begin (), end=frames.end (); iter!=end; ++iter)
  {
    BasicFrame& frame = **iter;
    
    frame.Draw ();
  }
  
    //������� ������ ������
    
  frames.clear ();

  frame_position = frames.end ();
  frames_count   = 0;

    //����� ���������
    
  log.Printf ("End draw frames (id=%u, frame_id=%u)", Id (), frame_id);
    
    //���������� �������� ������
    
  frame_id++;
}

void BasicRenderer::CancelFrames ()
{
  frames.clear ();
  
  frame_position = frames.end ();
  frames_count   = 0;
  
  log.Printf ("Renderer cancel frames (id=%u)", Id ());
}
