inline RenderingContext::RenderingContext (manager::Frame& in_frame, server::RenderManager& in_render_manager, ITraverseResultCache& in_traverse_result_cache, server::Camera& in_camera, RenderingContext* in_parent)
  : parent (in_parent)
  , frame (in_frame)
  , render_manager (in_render_manager)
  , traverse_result ()
  , traverse_result_cache (in_traverse_result_cache)
  , camera (in_camera)
{
}

inline RenderingContext::RenderingContext (RenderingContext& in_parent, manager::Frame& in_frame)
  : parent (&in_parent)
  , frame (in_frame)
  , render_manager (in_parent.render_manager)
  , traverse_result (in_parent.traverse_result)
  , traverse_result_cache (in_parent.traverse_result_cache)
  , camera (in_parent.camera)
{
}

inline server::TraverseResult& RenderingContext::TraverseResult ()
{
  if (traverse_result)
    return *traverse_result;

  try
  {
    traverse_result = &traverse_result_cache.Result ();

    return *traverse_result;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::RenderingContext::TraverseResult");
    throw;
  }
}
