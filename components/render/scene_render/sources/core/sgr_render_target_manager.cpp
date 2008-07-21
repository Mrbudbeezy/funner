#include "scene_render_shared.h"

using namespace render;

/*
===================================================================================================
    RenderTargetManager
===================================================================================================
*/

/*
    ���������
*/

RenderTargetManager::RenderTargetManager ()
  : render_path_manager (0),
    draw_depth (0)
{
}

/*
    ��������� 楫�� ७��ਭ��
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
    ���� ����ᮢ
*/

void RenderTargetManager::FlushResources ()
{
  for (RenderTargetSet::iterator iter=render_targets.begin (), end=render_targets.end (); iter!=end; ++iter)
    (*iter)->FlushResources ();
}

/*
    ��⠭���� ⥪�饣� �������� ��⥩ ७��ਭ��
*/

void RenderTargetManager::SetRenderPathManager (render::RenderPathManager* in_render_path_manager)
{
  FlushResources ();

  render_path_manager = in_render_path_manager;
}

/*
    ��砫� / ����� �࠭���樨 ���ᮢ�� 
*/

//१���� �����뢠�� �����⨬���� ���ᮢ�� (������� ��� ���饣� ����஫� ��㡨�� ���������� ७��ਭ��)
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
      //���㠫����� ����
          
    mid_level::IRenderer* renderer = render_path_manager->Renderer ();
    
    if (renderer)
      renderer->DrawFrames ();
  }
}

/*
===================================================================================================
    RenderTargetBase
===================================================================================================
*/

/*
    ��������� / ��������
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
