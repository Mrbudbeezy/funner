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
  throw xtl::make_not_implemented_exception ("render::scene_render3d::ForEachLightTechnique::UpdateFrameCore");
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
