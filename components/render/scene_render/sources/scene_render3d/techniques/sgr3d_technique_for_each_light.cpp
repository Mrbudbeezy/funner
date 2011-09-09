#include "../shared.h"

using namespace render;
using namespace render::scene_render3d;

/*
    �������� ���������� �������
*/

struct ForEachLightTechnique::Impl
{
};

/*
    ����������� / ����������
*/

ForEachLightTechnique::ForEachLightTechnique (RenderManager& manager, common::ParseNode& node)
  : impl (new Impl)
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
    
      //����������� ������ ����������, ���������� � ������
    
//    camera.Scene ()->Traverse (camera.Frustum (), );
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
