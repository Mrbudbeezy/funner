#include "scene_render_shared.h"

using namespace render;

/*
===================================================================================================
    RenderTargetManager
===================================================================================================
*/

/*
    �����������
*/

RenderTargetManager::RenderTargetManager (const SceneRender::LogFunction& in_log_handler)
  : render_path_manager (0),
    draw_depth (0),
    log_handler (in_log_handler)
{
}

/*
    ����������� ����� ����������
*/

void RenderTargetManager::Register (RenderTargetBase& target)
{
  render_targets.insert (&target);
}

void RenderTargetManager::Unregister (RenderTargetBase& target)
{
  render_targets.erase (&target);
}

/*
    ����� ��������
*/

void RenderTargetManager::FlushResources ()
{
  for (RenderTargetSet::iterator iter=render_targets.begin (), end=render_targets.end (); iter!=end; ++iter)
    (*iter)->FlushResources ();
    
  attachments.clear ();
}

/*
    ��������� �������� ��������� ����� ����������
*/

void RenderTargetManager::SetRenderPathManager (render::RenderPathManager* in_render_path_manager)
{
  FlushResources ();

  render_path_manager = in_render_path_manager;
  
  if (render_path_manager)
    UpdateAttachmentsMap ();
}

/*
    ��������� ������� ����������
*/

mid_level::IRenderer& RenderTargetManager::Renderer ()
{
  static const char* METHOD_NAME = "render::RenderPathManager::Renderer";

  if (!render_path_manager)
    throw xtl::format_operation_exception (METHOD_NAME, "Null render path manager");

  render::mid_level::IRenderer* renderer = render_path_manager->Renderer ();

  if (!renderer)
    throw xtl::format_operation_exception (METHOD_NAME, "Null renderer");

  return *renderer;
}

/*
    ��������� ���� ����������
*/

ICustomSceneRender& RenderTargetManager::GetRenderPath (const char* render_path_name)
{
  try
  {
    if (!render_path_manager)
      throw xtl::format_operation_exception ("", "Null render path manager");

    return render_path_manager->GetRenderPath (render_path_name);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderTargetManager::GetRenderPath");
    throw;
  }
}

/*
    ����������� ��������������� ����� ����������
*/

void RenderTargetManager::RegisterAttachment (const char* name, mid_level::IRenderTarget* render_target)
{
  static const char* METHOD_NAME = "render::RenderTargetManager::RegisterAttachment";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  AttachmentMap::iterator iter = attachments.find (name);
  
  if (iter != attachments.end ())
    throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Attachment already registered");

  attachments.insert_pair (name, render_target);
}

void RenderTargetManager::UnregisterAttachment (const char* name)
{
  if (!name)
    return;
    
  attachments.erase (name);
}

mid_level::IRenderTarget* RenderTargetManager::GetAttachment (const char* name)
{
  static const char* METHOD_NAME = "render::RenderTargetManager::GetAttachment";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  AttachmentMap::iterator iter = attachments.find (name);

  if (iter == attachments.end ())
    throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Attachment not found");

  return iter->second.get ();
}

/*
    ���������� ����� ��������������� ����� ����������
*/

void RenderTargetManager::UpdateAttachmentsMap ()
{
  try
  {
      //������� ����� ��������������� ����� ����������

    attachments.clear ();
    
      //��������� ������� ������������
    
    mid_level::IRenderer& renderer = Renderer ();
    
      //���������� ������� �����
      
    for (size_t i=0, count=renderer.GetFrameBuffersCount (); i<count; i++)
    {
      mid_level::IRenderTarget *color_buffer         = renderer.GetColorBuffer (i),
                               *depth_stencil_buffer = renderer.GetDepthStencilBuffer (i); 

      RegisterAttachment (common::format ("Color.FrameBuffer%u", i).c_str (), color_buffer);
      RegisterAttachment (common::format ("DepthStencil.FrameBuffer%u", i).c_str (), depth_stencil_buffer);
    }
    
      //���������� ������ ������� �����
      
    RegisterAttachment ("Color.", 0);
    RegisterAttachment ("DepthStencil.", 0);    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderTargetManager::UpdateAttachmentsMap");
    throw;
  }
}

/*
    ������ / ����� ���������� ��������� 
*/

//��������� ���������� ������������ ��������� (������� ��� �������� �������� ������� ����������� ����������)
bool RenderTargetManager::BeginDraw ()
{
  if (draw_depth == ~0)
    return false;
    
  draw_depth++;
  
  return true;
}

void RenderTargetManager::EndDraw ()
{
  if (!draw_depth)
    return;
    
  if (!--draw_depth && render_path_manager)
  {
      //������������ �����
          
    mid_level::IRenderer* renderer = render_path_manager->Renderer ();
    
    if (renderer)
    {
      try
      {
        renderer->DrawFrames ();
      }
      catch (std::exception& exception)
      {
        LogMessage (exception.what ());
      }
      catch (...)
      {
        LogMessage ("Unknown exception\n    at render::mid_level::IRenderer::DrawFrames");
      }
    }
  }
}

/*
    ����������������
*/

void RenderTargetManager::LogMessage (const char* message)
{
  if (!message || !log_handler)
    return;
    
  log_handler (message);
}

/*
===================================================================================================
    RenderTargetBase
===================================================================================================
*/

/*
    ����������� / ����������
*/

RenderTargetBase::RenderTargetBase (RenderTargetManager& in_manager)
  : manager (&in_manager)
{
  manager->Register (*this);
}

RenderTargetBase::~RenderTargetBase ()
{
  manager->Unregister (*this);
}

/*
    ����������������
*/

void RenderTargetBase::LogVPrintf (const char* format, va_list list)
{
  if (!format)
    return;
    
  try
  {
    manager->LogMessage (common::vformat (format, list).c_str ());
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

void RenderTargetBase::LogPrintf (const char* format, ...)
{
  va_list list;
  
  va_start   (list, format);  
  LogVPrintf (format, list);
}
