#include "shared.h"

//!!!���������� ������� ������� ������ �������� ������ � �� ��������. ������� �� viewport

using namespace render;

/*
   ���������
*/

const size_t VIEWPORT_ARRAY_RESERVE      = 32;  //������������� ���������� �������� ���������
const size_t RENDER_PATHS_STRING_RESERVE = 256; //������������� ������ ������ � ���� ����� ����������

/*
    �������� ���������� SceneRender
*/

typedef stl::vector<Viewport>                       ViewportArray;
typedef stl::hash_map<size_t, CustomSceneRenderPtr> RenderPathMap;
typedef xtl::com_ptr<mid_level::IRenderer>          RendererPtr;

struct SceneRender::Impl
{  
  RendererPtr   renderer;            //������� ����������
  stl::string   render_paths_string; //������ � ������� ��������� ����� ����������
  RenderPathMap render_paths;        //��������� ���� ����������  
  Rect          window;              //���������� ���� ������
  ViewportArray viewports;           //������� ������
  math::vec4f   background_color;    //���� ����
  
    //�����������
  Impl ()
  {
    viewports.reserve (VIEWPORT_ARRAY_RESERVE);
  }  
  
    //��������� ����������� ������� ������
  void Render (const Viewport& viewport, const LogHandler& log, bool need_clear)
  {
    try
    {
        //����� ���� ����������
      
      RenderPathMap::iterator iter = render_paths.find (viewport.RenderPathHash ());
      
      if (iter == render_paths.end ())
        throw xtl::format_not_supported_exception ("", "Unknown path"); //���� �� ������              

      CustomSceneRenderPtr render = iter->second;
      
        //������������
        
      RenderContext render_context (viewport, *renderer, window, need_clear, background_color);
        
      render->Render (render_context);
    }
    catch (std::exception& exception)
    {
      try
      {
        log (common::format ("Exception at render viewport '%s' render_path '%s': %s", viewport.Name (), viewport.RenderPath (),
          exception.what ()).c_str ());
      }
      catch (...)
      {
        //���������� ���� ����������
      }
    }
    catch (...)
    {
      try
      {
        log (common::format ("Unknown exception at render viewport '%s' render_path '%s': %s", viewport.Name (), viewport.RenderPath ()).c_str ());
      }
      catch (...)
      {
        //���������� ���� ����������
      }      
    }
  }
  
    //��������� ����������� ���� �������� �������� ������
  void Render (const LogHandler& log)
  {        
      //���������� �������� ������
    
    struct ViewportComparator
    {
      bool operator () (const Viewport& vp1, const Viewport& vp2) const
      {
        return vp1.ZOrder () < vp2.ZOrder ();
      }
    };
    
    stl::sort (viewports.begin (), viewports.end (), ViewportComparator ());
    
      //��������� �������� ������
      
    bool need_clear = true;
      
    for (ViewportArray::iterator iter=viewports.begin (), end=viewports.end (); iter!=end; ++iter)
    {
      const Viewport& viewport = *iter;
      
      if (viewport.IsActive ())
      {
        Render (viewport, log, need_clear);
        
        need_clear = false;
      }
    }
  }
};

/*
    ����������� / ����������
*/

SceneRender::SceneRender ()
  : impl (new Impl)
{
}

SceneRender::SceneRender (const char* driver_name_mask, const char* renderer_name_mask, const char* render_path_masks)
  : impl (new Impl)
{
  try
  {
    SetRenderer (driver_name_mask, renderer_name_mask, render_path_masks);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::SceneRender::SceneRender");
    throw;
  }
}

SceneRender::~SceneRender ()
{
}

/*
    ���������/��������� ������� ����������
*/

void SceneRender::SetRenderer
 (const char* driver_name_mask,   //����� ����� �������� ������� ����������
  const char* renderer_name_mask, //����� ����� ������� ���������� � ��������
  const char* render_path_masks)  //������ ����� ��� ��������� ����� ���������� (����������� - ������)
{
  try
  {
    if (!driver_name_mask)
      throw xtl::make_null_argument_exception ("", "driver_name_mask");
      
    if (!renderer_name_mask)
      throw xtl::make_null_argument_exception ("", "renderer_name_mask");
      
    if (!render_path_masks)
      throw xtl::make_null_argument_exception ("", "render_path_masks");      

      //�������� ������� ����������      

    RendererPtr renderer (mid_level::DriverManager::CreateRenderer (driver_name_mask, renderer_name_mask), false);

      //���������� ��������� ��� ����� ����������
      
    typedef stl::vector<stl::string>   StringArray;
    typedef stl::hash_set<stl::string> StringSet;
      
    StringSet                    path_names;
    StringArray                  path_masks        = common::split (render_path_masks);
    SceneRenderManager::Iterator render_path_begin = SceneRenderManager::CreateIterator ();

    for (StringArray::iterator iter=path_masks.begin (), end=path_masks.end (); iter!=end; ++iter)
    {
      const char* render_path_mask = iter->c_str ();
      
      for (SceneRenderManager::Iterator render_path_iter=render_path_begin; render_path_iter; ++render_path_iter)
        if (common::wcmatch (render_path_iter->Name (), render_path_mask))
          path_names.insert (render_path_iter->Name ());
    }
    
      //�������� ����� ����������

    RenderPathMap render_paths;
    stl::string   render_paths_string;
    
    render_paths_string.reserve (RENDER_PATHS_STRING_RESERVE);
    
    for (StringSet::iterator iter=path_names.begin (), end=path_names.end (); iter!=end; ++iter)
    {
      const char* path_name = iter->c_str ();
      
        //�������� ���� ����������
        
      CustomSceneRenderPtr render_path = SceneRenderManagerImpl::Instance ().CreateRender (renderer.get (), path_name);
      
        //����������� ���� ����������
        
      render_paths.insert_pair (common::strhash (path_name), render_path);

        //����������� ������ ��� ����� ����������

      if (!render_paths_string.empty ())
        render_paths_string += ' ';

      render_paths_string += path_name;
    }
    
      //��������� �������� ���������
      
    impl->renderer.swap (renderer);
    impl->render_paths.swap (render_paths);
    impl->render_paths_string.swap (render_paths_string);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::SceneRender::SetRenderer");
    throw;
  }
}

void SceneRender::ResetRenderer ()
{
  RenderPathMap ().swap (impl->render_paths);

  impl->render_paths_string.clear ();

  impl->renderer = 0;
}

const char* SceneRender::RendererDescription () const
{
  return impl->renderer ? impl->renderer->GetDescription () : "";
}

const char* SceneRender::RenderPaths () const
{
  return impl->render_paths_string.c_str ();
}

bool SceneRender::HasRenderPath (const char* path_name) const
{
  if (!path_name)
    return false;

  RenderPathMap::iterator iter = impl->render_paths.find (common::strhash (path_name));

  return iter != impl->render_paths.end ();
}

/*
    ���������� ��������� ������
*/

void SceneRender::Attach (const Viewport& viewport)
{
  impl->viewports.push_back (viewport);
}

void SceneRender::Detach (const Viewport& viewport)
{
  size_t id = viewport.Id ();

  for (ViewportArray::iterator iter=impl->viewports.begin (); iter!=impl->viewports.end ();)
    if (iter->Id () == id) impl->viewports.erase (iter);
    else                   ++iter;
}

void SceneRender::DetachAllViewports ()
{
  impl->viewports.clear ();
}

/*
    ��������� ����������� ���� ������
*/

void SceneRender::SetWindow (const Rect& window)
{
  impl->window = window;
}

void SceneRender::SetWindow (int left, int top, size_t width, size_t height)
{
  SetWindow (Rect (left, top, width, height));
}

const Rect& SceneRender::Window () const
{
  return impl->window;
}

/*
    ���� ������� ������ ���������
*/

void SceneRender::SetBackgroundColor (const math::vec4f& color)
{
  impl->background_color = color;
}

const math::vec4f& SceneRender::BackgroundColor () const
{
  return impl->background_color;
}

/*
    ���������
*/

namespace
{

void default_log_handler (const char*)
{
}

}

void SceneRender::Render ()
{
  try
  {
    Render (&default_log_handler);
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

void SceneRender::Render (const LogHandler& log)
{
  try
  {
    impl->Render (log);
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

/*
    ������ ����������� (screen-shot)
*/

void SceneRender::CaptureImage (media::Image&)
{
  throw xtl::make_not_implemented_exception ("render::SceneRender::CaptureImage(media::Image&)");
}

void SceneRender::CaptureImage (const char* image_name)
{
  throw xtl::make_not_implemented_exception ("render::SceneRender::CaptureImage(const char*)");
}
