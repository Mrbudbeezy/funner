#include "shared.h"

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

/*
    ���������
*/

const size_t TRAVERSE_RESULT_VISUAL_MODELS_RESERVE_SIZE = 1024; //������������� ���������� ��������������� �������

/*
    ����������� / ����������
*/

ShadowMapRenderer::ShadowMapRenderer (server::RenderManager& manager, const common::ParseNode& node)
try
  : BasicRenderer (manager, common::get<const char*> (node, "effect"))
  , shadow_map (manager.Manager ().CreateTexture (common::get<const char*> (node, "texture")))
{
  try
  {
    render_target_name = common::get<const char*> (node, "render_target");
    local_texture_name = common::get<const char*> (node, "framemap");

    size_t layers_count = shadow_map.Depth ();

    render_targets.reserve (layers_count);

    for (size_t i=0; i<layers_count; i++)
    {
      manager::RenderTarget target = shadow_map.RenderTarget (i, 0);

      render_targets.push_back (target);
    }

    traverse_result.visual_models.reserve (TRAVERSE_RESULT_VISUAL_MODELS_RESERVE_SIZE);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ShadowMapRenderer::ShadowMapRenderer");
    throw;
  }
}
catch (xtl::exception& e)
{
  e.touch ("render::scene::server::ShadowMapRenderer::ShadowMapRenderer");
  throw;
}

ShadowMapRenderer::~ShadowMapRenderer ()
{
}

/*
    ���������� �����
*/

namespace
{

struct TraverseResultCache: public ITraverseResultCache
{
  TraverseResult& result;

  TraverseResultCache (TraverseResult& in_result) : result (in_result) {}

  TraverseResult& Result () { return result; }
};

}

void ShadowMapRenderer::UpdateShadowMap (RenderingContext& parent_context, Light& light)
{
  try
  {
    if (!light.SceneOwner ())
      return;

    Scene& scene = *light.SceneOwner ();

    size_t pass_count = light.CamerasCount () < render_targets.size () ? light.CamerasCount () : render_targets.size ();

    manager::Frame& frame = Frame ();

    TraverseResultCache cache (traverse_result);
printf ("%s(%u) %u %u\n", __FUNCTION__, __LINE__, light.CamerasCount (), render_targets.size ());
    for (size_t i=0; i<pass_count; i++)
    {
      Camera& camera = const_cast<Camera&> (light.Camera (i));

      frame.SetRenderTarget (render_target_name.c_str (), render_targets [i]);

      traverse_result.Clear ();

      scene.Traverse (camera.Frustum (), traverse_result, Collect_VisualModels);

      RenderingContext context (frame, RenderManager (), cache, camera, &parent_context);
printf ("%s(%u)\n", __FUNCTION__, __LINE__);
      Draw (context);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ShadowMapRenderer::UpdateShadowMap");
    throw;
  }
}
