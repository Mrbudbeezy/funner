#include "shared.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4355) // this used in base initializer list
#endif

using namespace render;

typedef stl::hash_map<stl::string, RenderTarget> RenderTargetMap;

/*
    �������� ���������� ������� ���������
*/

struct RenderableView::Impl: public scene_graph::IViewportListener, public scene_graph::IScreenListener
{
  RenderManager&                    manager;                      //�������� ����������
  scene_graph::Screen&              screen;                       //�����
  scene_graph::Viewport&            viewport;                     //������� ������
  ISceneRenderPtr                   renderer;                     //������ �����
  RenderTargetMap                   render_targets;               //������� ������
  common::PropertyMap::EventHandler properties_update_handler; //���������� ���������� �������
  xtl::auto_connection              properties_update_connection; //���������� ���������� �������
  bool                              is_active;                    //������� �� ������� ���������
  bool                              need_reconfiguration;         //������������ ��������
  bool                              need_update_renderer;         //��������� �������� ������
  bool                              need_update_render_targets;   //��������� �������� ������ ���������
  bool                              need_update_background;       //��������� �������� ��������� �������
  bool                              need_update_camera;           //��������� �������� ������
  bool                              need_update_properties;       //��������� ���������� �������
  
///�����������
  Impl (RenderManager& in_manager, scene_graph::Screen& in_screen, scene_graph::Viewport& in_viewport)
    : manager (in_manager)
    , screen (in_screen)
    , viewport (in_viewport)
    , properties_update_handler (xtl::bind (&Impl::OnPropertiesUpdate, this))
    , is_active (viewport.IsActive ())
    , need_reconfiguration (true)
    , need_update_renderer (true)
    , need_update_render_targets (true)    
    , need_update_background (true)
    , need_update_camera (true)
    , need_update_properties (true)
  {        
    properties_update_connection = viewport.Properties ().RegisterEventHandler (common::PropertyMapEvent_OnUpdate, properties_update_handler);
    
    screen.AttachListener (this);
    
    try
    {
      viewport.AttachListener (this);
    }
    catch (...)
    {
      screen.DetachListener (this);
      throw;
    }
  }  
  
///����������
  ~Impl ()
  {
    try
    {
      viewport.DetachListener (this);
    }
    catch (...)
    {
    }
    
    try
    {
      screen.DetachListener (this);
    }
    catch (...)
    {
    }    
  }

///������� ������ ���������
  void OnScreenChangeArea (const Rect&)
  {
    need_reconfiguration       = true;    
    need_update_render_targets = true;
  }

///������� ������ ���������
  void OnViewportChangeArea (const Rect&)
  {
    need_reconfiguration       = true;
    need_update_render_targets = true;
  }

///�������� ������
  void OnViewportChangeCamera (scene_graph::Camera* new_camera)
  {
    need_reconfiguration = true;
    need_update_camera   = true;
  }  

///�������� ���������� ������� ������
  void OnViewportChangeActive (bool new_state)
  {
    is_active = new_state;
  }
  
///�������� ��������� ������� ������� ������
  void OnViewportChangeBackground (bool, const math::vec4f&)
  {
    need_reconfiguration   = true;
    need_update_background = true;
  }

///������� ���� ����������
  void OnViewportChangeRenderer (const char*)
  {
    need_reconfiguration = true;
    need_update_renderer = true;
  }

///�������� �������� ����������
  void OnViewportChangeProperties (const common::PropertyMap& properties)
  {
    need_update_properties = true;
    need_reconfiguration   = true;

    properties_update_connection = properties.RegisterEventHandler (common::PropertyMapEvent_OnUpdate, properties_update_handler);
  }

  void OnPropertiesUpdate ()
  {
    need_update_properties = true;
  }
  
///����������������
  void Reconfigure ()
  {
    try
    {
      if (!need_reconfiguration)
        return;
        
        //���������������� �������
        
      if (!renderer || need_update_renderer)
      {        
        renderer = ISceneRenderPtr ();

        renderer = SceneRenderManagerSingleton::Instance ()->CreateRender (manager, viewport.Renderer ());
        
        if (!renderer)
          throw xtl::format_operation_exception ("", "Can't create renderer '%s'", viewport.Renderer ());
        
        need_update_renderer       = false;
        need_update_render_targets = true;
        need_update_camera         = true;
        need_update_properties     = true;
        need_update_background     = true;
      }
            
        //���������������� ���������� �������
        
      if (need_update_background)
      {
        render::Frame& frame = renderer->Frame ();

        if (viewport.BackgroundState ())
        {
          frame.SetClearColor (viewport.BackgroundColor ());          
          frame.SetClearFlags (render::ClearFlag_All | render::ClearFlag_ViewportOnly);
        }
        else
        {
          frame.SetClearFlags (0u);
        }

        need_update_background = false;
      }

        //���������������� ������

      if (need_update_camera)
      {
        renderer->UpdateCamera (viewport.Camera ());

        need_update_camera = false;
      }

        //���������������� ������� ������� ���������
        
      if (need_update_render_targets && renderer)
      {        
        const scene_graph::Rect &screen_area   = screen.Area (),
                                &viewport_area = viewport.Area ();
                                
        Frame& frame = renderer->Frame ();

        for (RenderTargetMap::iterator iter=render_targets.begin (), end=render_targets.end (); iter!=end; ++iter)
        {
          size_t target_width = iter->second.Width (), target_height = iter->second.Height ();

          double x_scale = screen_area.width ? double (target_width) / screen_area.width : 0.0,
                 y_scale = screen_area.height ? double (target_height) / screen_area.height : 0.0;

          render::Rect target_rect (int ((viewport_area.left () - screen_area.left ()) * x_scale),
                               int ((viewport_area.top () - screen_area.top ()) * y_scale),
                               size_t (ceil (viewport_area.width * x_scale)),
                               size_t (ceil (viewport_area.height * y_scale)));                               

          frame.SetRenderTarget (iter->first.c_str (), iter->second, render::Viewport (target_rect));          
        }
        
        need_update_render_targets = false;
      }
      
        //���������������� ������� ������
      
      need_reconfiguration = false;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::RenderableView::Impl::Reconfigure");
      throw;
    }
  }
};

/*
    ����������� / ����������
*/

RenderableView::RenderableView (RenderManager& manager, scene_graph::Screen& screen, scene_graph::Viewport& viewport)
{
  try
  {
    impl = new Impl (manager, screen, viewport);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderableView::RenderableView");
    throw;
  }
}

RenderableView::~RenderableView ()
{
}

/*
    ������� ������ / �����
*/

const scene_graph::Viewport& RenderableView::Viewport ()
{
  return impl->viewport;
}

const scene_graph::Screen& RenderableView::Screen ()
{
  return impl->screen;
}

/*
    ����������� ������� ������� ���������
*/

void RenderableView::SetRenderTarget (const char* name, const RenderTarget& target)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    RenderTargetMap::iterator iter = impl->render_targets.find (name);
    
    if (iter != impl->render_targets.end ())
    {
      iter->second = target;
    }
    else
    {
      impl->render_targets.insert_pair (name, target);
    }
    
    impl->need_reconfiguration       = true;
    impl->need_update_render_targets = true;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderableView::SetRenderTarget");
    throw;
  }
}

void RenderableView::RemoveRenderTarget (const char* name)
{
  try
  {
    if (!name)
      return;

    RenderTargetMap::iterator iter = impl->render_targets.find (name);

    if (iter == impl->render_targets.end ())
      return;

    impl->render_targets.erase (iter);

    if (impl->renderer)
      impl->renderer->Frame ().RemoveRenderTarget (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderableView::RemoveRenderTarget");
    throw;
  }
}

/*
    ���������� �����
*/

void RenderableView::UpdateFrame (Frame* parent_frame)
{
  try
  {
      //��������� ������ � ������ ���������� ������� ������
    
    if (!impl->is_active)
      return;

      //���������������� � ������ ��������� ������������ ����������

    if (impl->need_reconfiguration)
      impl->Reconfigure ();
      
      //� ������ ���������� ������� ��������� �� ������������
      
    if (!impl->renderer)
      return;
      
      //���������������� �������������� ����������
      
    if (impl->need_update_properties)
    {
      impl->renderer->UpdateProperties (impl->viewport.Properties ());

      impl->need_update_properties = false;
    }
    
      //���������� �����
      
    impl->renderer->UpdateFrame ();
    
      //���������

    if (parent_frame) parent_frame->AddFrame (impl->renderer->Frame ());
    else              impl->renderer->Frame ().Draw ();    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderableView::Update");
    throw;
  }
}
