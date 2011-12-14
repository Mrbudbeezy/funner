#include "shared.h"

using namespace render::obsolete;

namespace
{

/*
    ���������
*/

const size_t DEFAULT_MAX_DRAW_DEPTH = 16; //������������ ������� ����������� ���������� �� ���������

}

/*
    �������� ���������� SceneRender
*/

typedef stl::auto_ptr<RenderManager> RenderManagerPtr;

struct SceneRender::Impl
{
  RenderManagerPtr         render_manager; //�������� ����������
  QueryManager             query_manager;  //�������� �������� ����������
  SceneRender::LogFunction log_handler;    //������� ����������� ����������������
  size_t                   max_draw_depth; //������������ ������� ����������� ����������

///�����������
  Impl () : max_draw_depth (DEFAULT_MAX_DRAW_DEPTH) {}

///�������� ������� ����������
  IRenderQuery* CreateRenderQuery
    (mid_level::IRenderTarget* render_target,
     mid_level::IRenderTarget* depth_stencil_target,
     const char*               query_string)
  {
    if (!render_manager)
      throw xtl::format_operation_exception ("render::SceneRender::Impl::CreateRenderQuery", "Null render manager");

    return query_manager.CreateQuery (render_target, depth_stencil_target, query_string, *render_manager);
  }

    //���������� ����������������
  void LogMessage (const char* message)
  {
    if (!log_handler)
      return;

    try
    {
      log_handler (message);
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }

  void LogPrintf (const char* format, ...)
  {
    if (!log_handler)
      return;

    va_list args;

    va_start (args, format);

    try
    {
      log_handler (common::vformat (format, args).c_str ());
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }
};

/*
    ����������� / ����������
*/

SceneRender::SceneRender ()
  : impl (new Impl)
{
}

SceneRender::SceneRender (const char* driver_name_mask, const char* renderer_name_mask, const char* render_path_masks)
  : impl (new Impl)
{
  try
  {
    SetRenderer (driver_name_mask, renderer_name_mask, render_path_masks);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::SceneRender::SceneRender");
    throw;
  }
}

SceneRender::~SceneRender ()
{
}

/*
    ���������/��������� ������� ����������
*/

void SceneRender::SetRenderer
 (const char* driver_name_mask,   //����� ����� �������� ������� ����������
  const char* renderer_name_mask, //����� ����� ������� ���������� � ��������
  const char* render_path_masks)  //������ ����� ��� ��������� ����� ���������� (����������� - ������)
{
  try
  {
    RenderManagerPtr new_manager (new RenderManager (driver_name_mask, renderer_name_mask, render_path_masks,
      xtl::bind (&Impl::LogMessage, &*impl, _1), xtl::bind (&Impl::CreateRenderQuery, &*impl, _1, _2, _3)));

    new_manager->DrawTransactionManager ().SetMaxDrawDepth (impl->max_draw_depth);

    impl->render_manager = new_manager;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::SceneRender::SetRenderer");
    throw;
  }
}

void SceneRender::ResetRenderer ()
{
  impl->render_manager = 0;
}

const char* SceneRender::RendererDescription () const
{
  return impl->render_manager ? impl->render_manager->Renderer ().GetDescription () : "";
}

const char* SceneRender::RenderPaths () const
{
  return impl->render_manager ? impl->render_manager->RenderPaths () : "";
}

bool SceneRender::HasRenderPath (const char* path_name) const
{
  return impl->render_manager ? impl->render_manager->HasRenderPath (path_name) : false;
}

/*
    ������ � ���������
*/

void SceneRender::LoadResource (const char* tag, const char* file_name)
{
  if (!impl->render_manager)
    throw xtl::format_operation_exception ("render::SceneRender::LoadResource", "Null render manager");

  impl->render_manager->LoadResource (tag, file_name);
}

void SceneRender::UnloadResource (const char* tag, const char* file_name)
{
  if (!impl->render_manager)
    throw xtl::format_operation_exception ("render::SceneRender::UnloadResource", "Null render manager");

  impl->render_manager->UnloadResource (tag, file_name);
}

/*
    ������� ��������� ����� ����������
*/

size_t SceneRender::RenderTargetsCount () const
{
  return impl->render_manager ? impl->render_manager->RenderTargetsCount () : 0;
}

RenderTarget SceneRender::RenderTarget (size_t index) const
{
  if (!impl->render_manager)
    throw xtl::make_range_exception ("render::SceneRender::RenderTarget", "index", index, 0u);

  return ConstructableRenderTarget (impl->render_manager->RenderTarget (index));
}

/*
    ����������� ������� ��������� �������� ���������� (�������� ���������)
*/

void SceneRender::RegisterQueryHandler (const char* query_string_mask, const QueryFunction& handler)
{
  try
  {
    impl->query_manager.RegisterQueryHandler (query_string_mask, handler);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::SceneRender::RegisterQueryHandler");
    throw;
  }
}

void SceneRender::UnregisterQueryHandler (const char* query_string_mask)
{
  impl->query_manager.UnregisterQueryHandler (query_string_mask);
}

void SceneRender::UnregisterAllQueryHandlers ()
{
  impl->query_manager.UnregisterAllQueryHandlers ();
}

/*
    ������������ ������� ����������� ����������
*/

void SceneRender::SetMaxDrawDepth (size_t level)
{
  impl->max_draw_depth = level;

  if (impl->render_manager)
    impl->render_manager->DrawTransactionManager ().SetMaxDrawDepth (level);
}

size_t SceneRender::MaxDrawDepth () const
{
  return impl->max_draw_depth;
}

/*
    ��������� ������� ����������� ����������������
*/

void SceneRender::SetLogHandler (const LogFunction& log)
{
  impl->log_handler = log;
}

const SceneRender::LogFunction& SceneRender::LogHandler () const
{
  return impl->log_handler;
}

/*
    ���������� ����������������
*/

namespace render
{

namespace obsolete
{

void log_printf (const SceneRender::LogFunction& log_handler, const char* format, ...)
{
  if (!log_handler || !format)
    return;

  va_list args;

  va_start (args, format);

  try
  {
    log_handler (common::vformat (format, args).c_str ());
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

}

}
