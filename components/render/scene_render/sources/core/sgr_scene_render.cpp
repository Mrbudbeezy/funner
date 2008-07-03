#include "shared.h"

using namespace render;

namespace
{

/*
   ���������
*/

const size_t VIEW_ARRAY_RESERVE          = 32;  //������������� ���������� �������� ���������
const size_t RENDER_PATHS_STRING_RESERVE = 256; //������������� ������ ������ � ���� ����� ����������
const size_t DEFAULT_WINDOW_WIDTH        = 100; //������ ���� �� ���������
const size_t DEFAULT_WINDOW_HEIGHT       = 100; //������ ���� �� ���������

/*
    �������� ���������������
*/

typedef xtl::com_ptr<IRenderView>                   RenderViewPtr;
typedef stl::hash_map<size_t, CustomSceneRenderPtr> RenderPathMap;

/*
    ����� ��������� ��� ���� ������� ������
*/

struct ViewCommon
{
  RenderPathMap  render_paths; //������� ����� ����������
  Rect           window;       //���������� ���� ������  
  bool           need_reorder; //���������� ��������������� ������� ������
  
  ViewCommon () : window (0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT), need_reorder (true) {}
};

/*
    ���������� ������� ������
*/

struct View: public IViewportListener, public xtl::reference_counter
{
  Viewport            viewport;           //������� ������
  ViewCommon&         common;             //����� ��� ���� �������� ������ ���������
  RenderViewPtr       render_view;        //������� ����������
  scene_graph::Scene* current_scene;      //������� �����
  bool                need_update_view;   //���������� �������� ��������� ������� ������
  bool                need_update_area;   //���������� �������� ���������� ������� ������
  bool                need_update_camera; //���������� �������� ������
  bool                need_update_path;   //���������� �������� ���� ����������

///����������� / ����������
  View (const Viewport& vp, ViewCommon& in_common)
    : viewport (vp),
      common (in_common),
      current_scene (0),
      need_update_view (true),
      need_update_area (true),
      need_update_camera (true),
      need_update_path (true)
  {
    viewport.AttachListener (this);
    
    common.need_reorder = true;    
  }  
  
  ~View ()
  {
    printf ("~View\n");
    viewport.DetachListener (this);
  }
  
///���������� ���������� ������� ����������
  void UpdateRenderView ()
  {
    try
    {
        //����������� �����

      scene_graph::Camera* camera = viewport.Camera ();
      scene_graph::Scene*  scene  = camera ? camera->Scene () : 0;            

      if (need_update_path || scene != current_scene) //������� ������������ ������� ������
        render_view = 0;

          //������������ ������� ������

      if (!render_view)
      {
          //��� ���������� ������ ���������� ������� ������� ����������

        if (!camera)
        {
          need_update_path = need_update_camera = need_update_view = need_update_area = false;
          current_scene    = 0;

          return;
        }

          //����� ���� ����������

        RenderPathMap::iterator iter = common.render_paths.find (viewport.RenderPathHash ());

        if (iter == common.render_paths.end ())
          throw xtl::format_not_supported_exception ("", "Unknown path '%s'", viewport.RenderPath ()); //���� �� ������          

          //�������� ������� ����������

        ICustomSceneRender& render = *iter->second;
        
        render_view = RenderViewPtr (render.CreateRenderView (scene), false);
        
          //�������� ������������ � ��������� ������� �� Viewport!!!

          //����� ���������� ���������� ���� ���������� ������

        need_update_area = need_update_camera = true;
        need_update_path = false;
      }

        //���������� ������

      if (need_update_camera)
      {
        current_scene = scene;
        
        need_update_camera = false;
      }
      
        //���������� ���������� ������� ������
        
      if (need_update_area)
      {
        float kx = 1.0f / common.window.width,
              ky = 1.0f / common.window.height;

        const Rect& viewport_rect = viewport.Area ();

        render_view->SetViewport (common.window.left * kx, common.window.top * ky, viewport_rect.width * kx, viewport_rect.height * ky);

        need_update_area = false;
      }

      need_update_view = false;
    }
    catch (xtl::exception& exception)
    {
      need_update_view = need_update_camera = need_update_path = need_update_area = false;
      render_view      = 0;
      current_scene    = 0;

      exception.touch ("render::View::UpdateRenderView");

      throw;
    }
    catch (...)
    {
      need_update_view = need_update_camera = need_update_path = need_update_area = false;
      render_view      = 0;
      current_scene    = 0;

      throw;
    }
  }

///���������
  void Draw ()
  {
    if (need_update_view)
      UpdateRenderView ();

    if (!render_view || !viewport.IsActive ())
      return;

    render_view->Draw ();
  }

///����������� �������
  void OnChangeArea       (const Rect&)          { need_update_view    = need_update_area   = true; }
  void OnChangeCamera     (scene_graph::Camera*) { need_update_view    = need_update_camera = true; }
  void OnChangeRenderPath (const char*)          { need_update_view    = need_update_path   = true; }
  void OnChangeZOrder     (int)                  { common.need_reorder = true; }

  void OnChangeProperty (const char* name, const char* value)
  {
    if (!render_view)
      return;

    render_view->SetProperty (name, value);
  }

///������� ������
  void AddRef  () { addref (this); }
  void Release () { release (this); }  
};

}

/*
    �������� ���������� SceneRender
*/

typedef xtl::com_ptr<mid_level::IRenderer> RendererPtr;
typedef xtl::com_ptr<View>                 ViewPtr;
typedef stl::vector<ViewPtr>               ViewArray;

struct SceneRender::Impl: public ViewCommon
{  
  RendererPtr              renderer;            //������� ����������
  stl::string              render_paths_string; //������ � ������� ��������� ����� ����������
  ViewArray                views;               //������� ������
  math::vec4f              background_color;    //���� ����
  SceneRender::LogFunction log_handler;         //������� ����������� ����������������
  bool                     need_update_background_color; //���������� �������� ���� ����

    //�����������
  Impl () : need_update_background_color (true)
  {
    views.reserve (VIEW_ARRAY_RESERVE);
  }
  
    //����������
  ~Impl ()
  {
    try
    {
      LogFunction render_log;

      for (RenderPathMap::iterator iter=render_paths.begin (), end=render_paths.end (); iter!=end; ++iter)
        iter->second->SetLogHandler (render_log);
    }
    catch (...)
    {
      //���������� ���� ����������
    }    
  }
  
    //���������� ����������������
  void LogMessage (const char* message)
  {
    if (!log_handler)
      return;

    try
    {
      log_handler (message);
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }
  
  void LogPrintf (const char* format, ...)
  {
    if (!log_handler)
      return;
    
    va_list args;
    
    va_start (args, format);
    
    try
    {
      log_handler (common::vformat (format, args).c_str ());
    }
    catch (...)
    {
      //���������� ���� ����������
    }    
  }
  
    //���������� �������� ������
  void SortViews ()
  {
    struct ViewComparator
    {
      bool operator () (const ViewPtr& vp1, const ViewPtr& vp2) const { return vp1->viewport.ZOrder () < vp2->viewport.ZOrder (); }
    };

    stl::sort (views.begin (), views.end (), ViewComparator ());
    
    need_reorder = false;    
  }
  
    //���������� ����� ����
  void UpdateBackgroundColor ()
  {
    for (RenderPathMap::iterator iter=render_paths.begin (), end=render_paths.end (); iter!=end; ++iter)
      iter->second->SetBackgroundColor (background_color);

    need_update_background_color = false;
  }
  
    //��������� ����������� ���� �������� �������� ������
  void Draw ()
  {        
      //������������ �������� ������
    
    if (need_reorder)
      SortViews ();
     
      //��������� ����� ����
      
    if (need_update_background_color)
      UpdateBackgroundColor ();
      
      //��������� �������� ������

    for (ViewArray::iterator iter=views.begin (), end=views.end (); iter!=end; ++iter)
    {
      try
      {
        (*iter)->Draw ();
      }
      catch (std::exception& exception)
      {
        Viewport& viewport = (*iter)->viewport;

        LogPrintf ("Exception at render viewport '%s' with render_path '%s' on renderer '%s': %s", viewport.Name (),
          viewport.RenderPath (), renderer->GetDescription (), exception.what ());
      }
      catch (...)
      {
        Viewport& viewport = (*iter)->viewport;

        LogPrintf ("Unknown exception at render viewport '%s' with render_path '%s' on renderer '%s': %s", viewport.Name (),
          renderer->GetDescription (), viewport.RenderPath ());
      }
    }
    
      //��������� ������
      
    try
    {
      renderer->DrawFrames ();
    }
    catch (std::exception& exception)
    {
      LogPrintf ("Exception at draw frames on renderer '%s': %s", renderer->GetDescription (), exception.what ());
    }
    catch (...)
    {
      LogPrintf ("Unknown exception at draw frames on renderer '%s'", renderer->GetDescription ());
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
      
      if (!strchr (render_path_mask, '*') && !strchr (render_path_mask, '?'))
      {
        path_names.insert (render_path_mask);
      }
      else
      {
        for (SceneRenderManager::Iterator render_path_iter=render_path_begin; render_path_iter; ++render_path_iter)
        {
          if (common::wcmatch (render_path_iter->Name (), render_path_mask))  
            path_names.insert (render_path_iter->Name ());
        }
      }
    }
    
      //�������� ����� ����������

    RenderPathMap render_paths;
    stl::string   render_paths_string;
    
    render_paths_string.reserve (RENDER_PATHS_STRING_RESERVE);
    
    LogFunction render_log = xtl::bind (&Impl::LogMessage, &*impl, _1);
    
    for (StringSet::iterator iter=path_names.begin (), end=path_names.end (); iter!=end; ++iter)
    {
      const char* path_name = iter->c_str ();

        //�������� ���� ����������
        
      CustomSceneRenderPtr render_path = SceneRenderManagerImpl::Instance ().CreateRender (renderer.get (), path_name);
      
        //��������� ������� ����������� ����������������

      render_path->SetLogHandler (render_log);

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
    
    impl->need_update_background_color = true;
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
  impl->views.push_back (ViewPtr (new View (viewport, *impl), false));
}

void SceneRender::Detach (const Viewport& viewport)
{
  size_t id = viewport.Id ();

  for (ViewArray::iterator iter=impl->views.begin (); iter!=impl->views.end ();)
    if ((*iter)->viewport.Id () == id) impl->views.erase (iter);
    else                               ++iter;
}

void SceneRender::DetachAllViewports ()
{
  impl->views.clear ();
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
  impl->background_color             = color;  
  impl->need_update_background_color = true;
}

const math::vec4f& SceneRender::BackgroundColor () const
{
  return impl->background_color;
}

/*
    ���������
*/

void SceneRender::Draw ()
{
  try
  {
    impl->Draw ();    
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

/*
    ��������� ������� ����������� ����������������
*/

void SceneRender::SetLogHandler (const LogFunction& log)
{
  impl->log_handler = log;
}

const SceneRender::LogFunction& SceneRender::LogHandler () const
{
  return impl->log_handler;
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
