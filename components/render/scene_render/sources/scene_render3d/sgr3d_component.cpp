#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

namespace components
{

namespace scene_render3d
{

/*
    ���������
*/

const char* COMPONENT_NAME = "render.scene.render3d"; //��� ����������
const char* RENDER_NAME    = "render3d";              //��� �������

/*
    ���������
*/

class Component
{
  public:
    Component ()
    {
      SceneRenderManager::RegisterRender (RENDER_NAME, &Component::CreateRender);
    }
    
  private:
    static ISceneRender* CreateRender (RenderManager& manager, const char*)
    {
      return new Render (manager);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> SceneRender3D (COMPONENT_NAME);

}

}

}
