#include "../shared.h"

using namespace render;
using namespace render::scene_render3d;

namespace
{

/*
    ���������
*/

const size_t LIGHTS_RESERVE_SIZE      = 64;   //������������� ���������� ���������� �����
const size_t RENDERABLES_RESERVE_SIZE = 1024; //������������� ���������� ��������������� ��������

}

/*
    �������� ���������� �������
*/

typedef stl::vector<Frame> FrameArray;

struct ForEachLightTechnique::Impl
{
  RenderManager   manager;          //�������� ����������
  TraverseResults traverse_results; //���������� ������ �����
  FrameArray      light_frames;     //������ ���������� �����  
  
///�����������
  Impl (RenderManager& in_manager, common::ParseNode& cfg_node)
    : manager (in_manager)
  {
    traverse_results.lights.reserve (LIGHTS_RESERVE_SIZE);
    traverse_results.renderables.reserve (RENDERABLES_RESERVE_SIZE);

    light_frames.reserve (LIGHTS_RESERVE_SIZE);
  }    
  
///�������������� ������
  Frame& AllocateFrame (size_t light_index)
  {
    if (light_index < light_frames.size ())
      return light_frames [light_index];    
    
    Frame frame = manager.CreateFrame ();
      
    light_frames.push_back (frame);
      
    return light_frames.back ();
  }
  
///���������� ����� �� ��������� �����
  void UpdateFromLight (Scene& scene, Frame& parent_frame, Light& light, size_t light_index)
  {
      //�������������� ���������� ������
      
    Frame& frame = AllocateFrame (light_index);

      //��������� ���������� ���������� ������
      
    //TODO: configure frame
    
      //���������� ��������������� ��������

    for (TraverseResults::RenderableArray::iterator iter=traverse_results.renderables.begin (), end=traverse_results.renderables.end (); iter!=end; ++iter)
    {
      Renderable& renderable = **iter;
      
      renderable.UpdateFrame (frame);
    }

      //���������� ���������� ������ � ������������

    parent_frame.AddFrame (frame);
  }
};

/*
    ����������� / ����������
*/

ForEachLightTechnique::ForEachLightTechnique (RenderManager& manager, common::ParseNode& node)
  : impl (new Impl (manager, node))
{
}

ForEachLightTechnique::~ForEachLightTechnique ()
{
}

/*
    ���������� �����
*/

void ForEachLightTechnique::UpdateFrameCore (Scene& scene, Frame& frame)
{
  try
  {
    if (!Camera ())
      return;
      
      //��������� ������

    scene_graph::Camera& camera = *Camera ();

      //����������� ������ ���������� � ��������������� ��������, ���������� � ������
    
    scene.Traverse (camera.Frustum (), impl->traverse_results, Collect_All);

      //����� ����������
      
    size_t light_index = 0;

    for (TraverseResults::LightArray::iterator iter=impl->traverse_results.lights.begin (), end=impl->traverse_results.lights.end (); iter!=end; ++iter, ++light_index)
      impl->UpdateFromLight (scene, frame, **iter, light_index);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::ForEachLightTechnique::UpdateFrameCore");
    throw;
  }
}

/*
    ���������� �������
*/

void ForEachLightTechnique::ResetPropertiesCore ()
{
}

void ForEachLightTechnique::UpdatePropertiesCore (const common::PropertyMap&)
{
}
