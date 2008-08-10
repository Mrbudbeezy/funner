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
        mid_level::IRenderTarget *color_buffer         = renderer->GetColorBuffer (i),
                                 *depth_stencil_buffer = renderer->GetDepthStencilBuffer (i); 
                                 
        stl::string color_attachment_name         = common::format ("FrameBuffer%u.Color", i),
                    depth_stencil_attachment_name = common::format ("FrameBuffer%u.DepthStencil", i);

        render_target_manager->RegisterAttachment (color_attachment_name.c_str (), color_buffer);
        render_target_manager->RegisterAttachment (depth_stencil_attachment_name.c_str (), depth_stencil_buffer);

        default_render_targets.push_back (render.CreateRenderTarget (color_attachment_name.c_str (), depth_stencil_attachment_name.c_str ()));
      }

        //���������� ������� ������ �����

      render_target_manager->RegisterAttachment ("Null.Color", 0);
      render_target_manager->RegisterAttachment ("Null.DepthStencil", 0);
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("render::SceneRender::Impl::UpdateRenderTargets");
      throw;
    }
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
      xtl::bind (&Impl::LogMessage, &*impl, _1));      

    new_manager.Swap (impl->render_path_manager);

    impl->render_target_manager->SetRenderPathManager (&impl->render_path_manager);
    
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
    return render::RenderTarget (*impl->render_target_manager, color_attachment_name, depth_stencil_attachment_name);
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
