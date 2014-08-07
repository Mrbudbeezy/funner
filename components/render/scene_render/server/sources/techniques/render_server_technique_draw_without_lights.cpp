#include "shared.h"

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

namespace render
{

namespace scene
{

namespace server
{

///������� ����������: ��������� ����������� ��� ���������
class DrawWithoutLights: public BasicTechnique
{
  public:
///����������� / ����������
    DrawWithoutLights (RenderManager& manager, const common::ParseNode& node)
      : BasicTechnique (manager, node)
    {
    }    

/// ����� ��� �����������
    static const char* ClassName     () { return "draw_without_lights"; }
    static const char* ComponentName () { return "render.scene.server.techniques.draw_without_lights"; }
};

}

}

}

extern "C"
{

TechniqueComponentRegistrator<render::scene::server::DrawWithoutLights> DrawWithoutLights;

}
