#include "scene_render_shared.h"

using namespace render;

/*
    �������� ���������� SceneRender
*/

typedef xtl::intrusive_ptr<RenderTargetManager> RenderTargetManagerPtr;
typedef stl::vector<RenderTarget>               RenderTargetList;

struct SceneRender::Impl
{  
  RenderPathManager        render_path_manager;    //�������� ����� ����������
  RenderTargetManagerPtr   render_target_manager;  //�������� ����� ����������
  RenderTargetList         default_render_targets; //���� ���������� �� ���������
  QueryManager             query_manager;          //�������� �������� ����������
  SceneRender::LogFunction log_handler;            //������� ����������� ����������������

    //�����������
  Impl ()
  {
    render_target_manager = RenderTargetManagerPtr (new RenderTargetManager (xtl::bind (&Impl::LogMessage, this, _1)), false);
  }

  ~Impl ()
  {
    render_target_manager->SetRenderPathManager (0);
  }
  
    //���������� ������� ����� ���������� �� ���������
  void UpdateRenderTargets (SceneRender& render)
  {
    try
    {   
      default_render_targets.clear ();
      render_target_manager->UnregisterAllAttachments ();
      
        //��������� ������� ������������
      
      mid_level::IRenderer* renderer = render_path_manager.Renderer ();
      
      if (!renderer)
        throw xtl::format_operation_exception ("", "Null renderer");
      
        //���������� ������� �����
        
      for (size_t i=0, count=renderer->GetFrameBuffersCount (); i<count; i++)
      {
        default_render_targets.push_back (render_target_manager->CreateRenderTarget (common::format ("FrameBuffer%u", i).c_str (),
          renderer->GetColorBuffer (i), renderer->GetDepthStencilBuffer (i)));
      }

        //���������� ������� ������ �����

      render_target_manager->RegisterAttachment ("Null", 0);
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("render::SceneRender::Impl::UpdateRenderTargets");
      throw;
    }
  }
  
    //�������� ������� ����������
  IRenderQuery* CreateRenderQuery
    (mid_level::IRenderTarget* render_target,
     mid_level::IRenderTarget* depth_stencil_target,
     const char*               query_string)
  {
    return query_manager.CreateQuery (render_target, depth_stencil_target, query_string, *render_target_manager);
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
    RenderPathManager new_manager (driver_name_mask, renderer_name_mask, render_path_masks,
      xtl::bind (&Impl::LogMessage, &*impl, _1), xtl::bind (&Impl::CreateRenderQuery, &*impl, _1, _2, _3));

    new_manager.Swap (impl->render_path_manager);

    impl->render_target_manager->SetRenderPathManager (&impl->render_path_manager);
    
      //������ ����� ����� ��������� ���������� � ��������� SceneRender::SetRenderer �� ������� �������� ����������!!!
    
    impl->UpdateRenderTargets (*this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::SceneRender::SetRenderer");
    throw;
  }
}

void SceneRender::ResetRenderer ()
{
  RenderPathManager new_manager;

  new_manager.Swap (impl->render_path_manager);

  impl->render_target_manager->SetRenderPathManager (0);

  impl->default_render_targets.clear ();
}

const char* SceneRender::RendererDescription () const
{
  mid_level::IRenderer* renderer = impl->render_path_manager.Renderer ();
  
  return renderer ? renderer->GetDescription () : "";
}

const char* SceneRender::RenderPaths () const
{
  return impl->render_path_manager.RenderPaths ();
}

bool SceneRender::HasRenderPath (const char* path_name) const
{
  return impl->render_path_manager.HasRenderPath (path_name);
}

/*
    ������ � ���������
*/

void SceneRender::LoadResource (const char* tag, const char* file_name)
{
  impl->render_path_manager.LoadResource (tag, file_name, impl->log_handler);
}

/*
    �������� ���� ����������
*/

RenderTarget SceneRender::CreateRenderTarget (const char* color_attachment_name, const char* depth_stencil_attachment_name)
{
  try
  {
    return impl->render_target_manager->CreateRenderTarget (color_attachment_name, depth_stencil_attachment_name);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::SceneRender::CreateRenderTarget");
    throw;
  }
}

/*
    ������� ��������� ����� ����������
*/

size_t SceneRender::RenderTargetsCount () const
{
  return impl->render_target_manager->RenderTargetsCount ();
}

RenderTarget SceneRender::RenderTarget (size_t index) const
{
  return impl->render_target_manager->RenderTarget (index);
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
