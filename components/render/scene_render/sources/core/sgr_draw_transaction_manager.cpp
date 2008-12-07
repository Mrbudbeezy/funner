#include "shared.h"

using namespace render;

namespace
{

/*
    �������� ���������
*/

struct DrawContext
{
  size_t child_start_frame_position; //��������� ������ ������ ��� ��������� ����������
  size_t parent_frame_offset;        //���������� ��������� ������ ������������� ���������
  size_t transaction_id;             //������������� ����������
};

}

/*
    �������� ���������� ��������� ���������� ���������
*/

typedef stl::vector<DrawContext> DrawContextArray;

struct DrawTransactionManager::Impl
{
  mid_level::IRenderer&    renderer;       //������� ����������
  size_t                   max_draw_depth; //������������ ������� ����������� ���������
  DrawContextArray         draw_contexts;  //��������� ���������
  SceneRender::LogFunction log_handler;    //���������� ����������������

///�����������
  Impl (mid_level::IRenderer& in_renderer, const SceneRender::LogFunction& in_log_handler)
    : renderer (in_renderer),
      max_draw_depth (0),
      log_handler (in_log_handler)
  {
  }
};

/*
    �����������
*/

DrawTransactionManager::DrawTransactionManager (mid_level::IRenderer& renderer, const SceneRender::LogFunction& log_handler)
  : impl (new Impl (renderer, log_handler))
{
}

DrawTransactionManager::~DrawTransactionManager ()
{
}

/*
    ������������ ������� ����������� ����������
*/

void DrawTransactionManager::SetMaxDrawDepth (size_t level)
{ 
  impl->draw_contexts.reserve (level);

  impl->max_draw_depth = level;
}

/*
    ���������� ����������� ���������
*/

size_t DrawTransactionManager::BeginDraw (size_t last_update_transaction_id)
{
  if (impl->draw_contexts.size () == impl->max_draw_depth + 1)
    return 0; //��������� ������������ ������� �����������
    
  if (!impl->draw_contexts.empty () && last_update_transaction_id == impl->draw_contexts.back ().transaction_id)
    return 0; //��������� ���������� ���������� ���� ����������

    //��������� ������ ���������� ����������

  static size_t next_transaction_id = 0;

  size_t transaction_id = next_transaction_id++;

  if (!transaction_id)
    transaction_id = next_transaction_id++; //��������� ����

    //������������� ���������

  DrawContext context;  

  if (!impl->draw_contexts.empty ())
  {
    context.child_start_frame_position = impl->draw_contexts.back ().child_start_frame_position;
  }
  else
  {
    context.child_start_frame_position = 0;
  }  

  context.parent_frame_offset = impl->renderer.GetFramePosition () - context.child_start_frame_position;    
  context.transaction_id      = transaction_id;

    //��������� ������ ������ ������

  impl->renderer.SetFramePosition (context.child_start_frame_position);      

    //���������� ���������  

  impl->draw_contexts.push_back (context);

    //����������� ������ ����������    

  return impl->draw_contexts.size () == 1 ? transaction_id : (&impl->draw_contexts.back ())[-1].transaction_id;
}

void DrawTransactionManager::EndDraw ()
{
  try
  {
    if (impl->draw_contexts.empty ())
      return;      
      
    if (impl->draw_contexts.size () > 1)
    {
      DrawContext *context = &impl->draw_contexts.back (),
                  *parent  = context - 1;

        //��������� ������������� ������ ������ ��������� ����������                    

      parent->child_start_frame_position = impl->renderer.GetFramePosition ();

        //��������� ������������� �������� ������        

      impl->renderer.SetFramePosition (parent->child_start_frame_position + context->parent_frame_offset);
    }
    else
    {     
        //���� �������� �������� �������� - ������ ��� ����������� �����

      impl->renderer.DrawFrames ();      
    }

      //����������� � ������������� ���������

    impl->draw_contexts.pop_back ();    
  }
  catch (std::exception& exception)
  {
    impl->draw_contexts.pop_back ();    
    impl->renderer.CancelFrames ();
    log_printf (impl->log_handler, "%s\n    at render::DrawTransactionManager::EndDraw", exception.what ());
  }
  catch (...)
  {
    impl->draw_contexts.pop_back ();    
    impl->renderer.CancelFrames ();
    log_printf (impl->log_handler, "Unknown exception\n    at render::DrawTransactionManager::EndDraw");
  }
}
