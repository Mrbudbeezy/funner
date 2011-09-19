#include "../shared.h"

using namespace render;
using namespace render::scene_render3d;

namespace
{

/*
    ���������
*/

const size_t LIGHTS_RESERVE_SIZE = 64;   //������������� ���������� ���������� �����

}

/*
    �������� ���������� �������
*/

typedef stl::vector<Frame> FrameArray;

struct ForEachLightTechnique::Impl
{
  RenderManager manager;         //�������� ����������
  FrameArray    light_frames;    //������ ���������� �����  
  
///�����������
  Impl (RenderManager& in_manager, common::ParseNode& cfg_node)
    : manager (in_manager)
  {
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
  void UpdateFromLight (Scene& scene, Frame& parent_frame, Light& light, TraverseResult& result, size_t light_index)
  {
      //�������������� ���������� ������
      
    Frame& frame = AllocateFrame (light_index);

      //��������� ���������� ���������� ������
      
    //TODO: configure frame
    
      //���������� ��������������� ��������

    for (TraverseResult::RenderableArray::iterator iter=result.renderables.begin (), end=result.renderables.end (); iter!=end; ++iter)
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

void ForEachLightTechnique::UpdateFrameCore (Scene& scene, Frame& frame, ITraverseResultCache& traverse_result_cache)
{
  try
  {
    if (!Camera ())
      return;
      
      //��������� ������

    scene_graph::Camera& camera = *Camera ();

      //����������� ������ ���������� � ��������������� ��������, ���������� � ������
    
    TraverseResult& result = traverse_result_cache.Result ();

      //����� ����������
      
    size_t light_index = 0;

    for (TraverseResult::LightArray::iterator iter=result.lights.begin (), end=result.lights.end (); iter!=end; ++iter, ++light_index)
      impl->UpdateFromLight (scene, frame, **iter, result, light_index);
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
