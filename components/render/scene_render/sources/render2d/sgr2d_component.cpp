#include "shared.h"

using namespace render;
using namespace render::render2d;

namespace
{

/*
    ���������
*/

const char* COMPONENT_NAME   = "render.scene_render.render2d"; //��� ����������
const char* RENDER_PATH_NAME = "render2d";                     //��� ���� ����������

/*
    ��������� ������� ��������� �����
*/

class SceneRender2dComponent
{
  public:
    SceneRender2dComponent ()
    {
      SceneRenderManager::RegisterRender (RENDER_PATH_NAME, &CreateRender);
    }

  private:
    static ICustomSceneRender* CreateRender (mid_level::IRenderer* renderer, const char*)
    {
      return new SceneRender2d;
    }
};

}

extern "C" 
{

common::ComponentRegistrator<SceneRender2dComponent> SceneRender2d (COMPONENT_NAME);

}
