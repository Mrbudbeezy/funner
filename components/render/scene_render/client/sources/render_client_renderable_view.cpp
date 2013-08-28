#include "shared.h"

using namespace render::scene::client;

/*
    �������� ���������� ������� ������
*/

struct RenderableView::Impl: public scene_graph::IViewportListener
{
  ConnectionPtr                     connection;                   //����������
  scene_graph::Viewport&            viewport;                     //������ �� ������� ������
  common::PropertyMap::EventHandler properties_update_handler;    //���������� ���������� �������
  xtl::auto_connection              properties_update_connection; //���������� ���������� �������
  bool                              is_active;                    //������� �� ������� ���������
  bool                              need_reconfiguration;         //������������ ��������
  bool                              need_update_renderer;         //��������� �������� ������
  bool                              need_update_render_targets;   //��������� �������� ������ ���������
  bool                              need_update_background;       //��������� �������� ��������� �������
  bool                              need_update_camera;           //��������� �������� ������
  bool                              need_update_properties;       //��������� ���������� �������

/// �����������
  Impl (const ConnectionPtr& in_connection, scene_graph::Viewport& in_viewport)
    : connection (in_connection)
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
    if (!connection)
      throw xtl::make_null_argument_exception ("", "connection");

    properties_update_connection = viewport.Properties ().RegisterEventHandler (common::PropertyMapEvent_OnUpdate, properties_update_handler);

    viewport.AttachListener (this);    
  }

/// ����������
  ~Impl ()
  {
    try
    {
      viewport.DetachListener (this);
    }
    catch (...)
    {
    }
  }

///������� ������ ���������
  void OnViewportChangeArea (const scene_graph::Rect&)
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
  void OnViewportChangeActive (bool)
  {
    need_reconfiguration = true;
  }
  
///�������� ��������� ������� ������� ������
  void OnViewportChangeBackground (bool, const math::vec4f&)
  {
    need_reconfiguration   = true;
    need_update_background = true;
  }

///������� ���� ����������
  void OnViewportChangeTechnique (const char*)
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
    need_reconfiguration   = true;
  }
  
///�������������
  void Synchronize ()
  {
    try
    {
      if (!need_reconfiguration)
        return;
        
        //���������������� �������
        
      if (need_update_renderer)
      {        

        need_update_renderer       = false;
      }

        //���������������� ����������

      if (is_active != viewport.IsActive ())
      {
        //????
      }
            
        //���������������� ���������� �������
        
      if (need_update_background)
      {

        need_update_background = false;
      }

        //���������������� ������

      if (need_update_camera)
      {

        need_update_camera = false;
      }

        //���������������� ������� ������� ���������
        
      if (need_update_render_targets)
      {        
        
        need_update_render_targets = false;
      }     
      
        //���������������� �������

      if (need_update_properties)
      {

        need_update_properties = false;
      }
      
      need_reconfiguration = false;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::client::RenderableView::Impl::Synchronize");
      throw;
    }
  }
};

/*
    ����������� / ����������
*/

RenderableView::RenderableView (const ConnectionPtr& connection, scene_graph::Viewport& viewport)
{
  try
  {
    impl.reset (new Impl (connection, viewport));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::View::View");
    throw;
  }
}

RenderableView::~RenderableView ()
{
}

/*
    ������� ������
*/

const scene_graph::Viewport& RenderableView::Viewport ()
{
  return impl->viewport;
}

/*
    ������������� � ��������
*/

void RenderableView::Synchronize ()
{
  try
  {
    impl->Synchronize ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::RenderableView::Synchronize");
    throw;
  }
}
