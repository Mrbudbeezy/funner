#include "shared.h"

using namespace render::scene::client;

/*
    �������� ���������� ������� ������
*/

struct RenderableView::Impl: public scene_graph::IViewportListener
{
  ConnectionPtr           connection;                   //����������
  scene_graph::Viewport&  viewport;                     //������ �� ������� ������
  PropertyMapSynchronizer properties_sync;              //������������� �������
  object_id_t             id;                           //������������� ������� ������
  bool                    is_active;                    //������� �� ������� ���������
  bool                    need_reconfiguration;         //������������ ��������
  bool                    need_update_renderer;         //��������� �������� ������
  bool                    need_update_background;       //��������� �������� ��������� �������
  bool                    need_update_camera;           //��������� �������� ������
  bool                    need_update_properties;       //��������� ���������� �������
  bool                    need_update_activity;         //��������� ���������� ���������� ������� ������
  bool                    need_update_name;             //��������� ���������� �����
  bool                    need_update_area;             //��������� ���������� ������� ������

/// �����������
  Impl (const ConnectionPtr& in_connection, scene_graph::Viewport& in_viewport)
    : connection (in_connection)
    , viewport (in_viewport)
    , id ()
    , is_active (viewport.IsActive ())
    , need_reconfiguration (true)
    , need_update_renderer (true)
    , need_update_background (true)
    , need_update_camera (true)
    , need_update_properties (true)
    , need_update_activity (true)
    , need_update_name (true)
    , need_update_area (true)
  {
    if (!connection)
      throw xtl::make_null_argument_exception ("", "connection");

    id = connection->Client ().AllocateId (ObjectType_Viewport);

    connection->Context ().CreateViewport (id);

    try
    {
      properties_sync = connection->Client ().CreateSynchronizer (viewport.Properties ());

      connection->Context ().SetViewportProperties (id, viewport.Properties ().Id ());

      viewport.AttachListener (this);
    }
    catch (...)
    {
      connection->Context ().DestroyViewport (id);
      throw;
    }
  }

/// ����������
  ~Impl ()
  {
    try
    {
      viewport.DetachListener (this);

      connection->Context ().DestroyViewport (id);

      connection->Client ().DeallocateId (ObjectType_Viewport, id);
    }
    catch (...)
    {
    }
  }

///������� ������ ���������
  void OnViewportChangeArea (const scene_graph::Rect&)
  {
    need_reconfiguration = true;
    need_update_area     = true;
  }

///��� ���������
  void OnViewportChangeName (const char*)
  {
    need_reconfiguration = true;
    need_update_name     = true;
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
    need_update_activity = true;
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

      Context& context = connection->Context ();

        //���������������� �����

      if (need_update_name)
      {
        context.SetViewportName (id, viewport.Name ());

        need_update_name = false;
      }
        
        //���������������� ������� ������

      if (need_update_area)
      {
        const scene_graph::Rect& r = viewport.Area ();

        context.SetViewportArea (id, r.x, r.y, r.width, r.height);
      }

        //���������������� �������
        
      if (need_update_renderer)
      {        
        context.SetViewportTechnique (id, viewport.Technique ());

        need_update_renderer = false;
      }

        //���������������� ����������

      if (need_update_activity)
      {
        context.SetViewportActive (id, viewport.IsActive ());

        need_update_activity = false;
      }
            
        //���������������� ���������� �������
        
      if (need_update_background)
      {
        context.SetViewportBackground (id, viewport.BackgroundState (), viewport.BackgroundColor ());

        need_update_background = false;
      }

        //���������������� ������

      if (need_update_camera)
      {
        //?????????????

        need_update_camera = false;
      }
      
        //���������������� �������

      if (need_update_properties)
      {        
        properties_sync = connection->Client ().CreateSynchronizer (viewport.Properties ());

        connection->Client ().Synchronize ();

        context.SetViewportProperties (id, viewport.Properties ().Id ());

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
    �������������
*/

object_id_t RenderableView::Id ()
{
  return impl->id;
}

/*
    ������������� � ��������
*/

void RenderableView::Synchronize ()
{
  try
  {
    impl->connection->Client ().Synchronize ();

    impl->Synchronize ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::RenderableView::Synchronize");
    throw;
  }
}
