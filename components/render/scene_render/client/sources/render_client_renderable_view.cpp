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
  object_id_t             render_target_id;             //������������� ���� ����������
  object_id_t             id;                           //������������� ������� ������
  bool                    is_active;                    //������� �� ������� ���������
  scene_graph::Camera*    camera;                       //������� ������
  ScenePtr                scene;                        //������� �����
  xtl::auto_connection    on_scene_changed_connection;  //���������� � �������� ���������� �� ��������� �����
  xtl::auto_connection    on_camera_updated_connection; //���������� � �������� ���������� �� ���������� �������������
  size_t                  camera_name_hash;             //��� ����� ������
  bool                    need_reconfiguration;         //������������ ��������
  bool                    need_update_renderer;         //��������� �������� ������
  bool                    need_update_background;       //��������� �������� ��������� �������
  bool                    need_update_camera;           //��������� �������� ������
  bool                    need_update_transformations;  //��������� �������� �������������
  bool                    need_update_scene;            //��������� �������� �����
  bool                    need_update_properties;       //��������� ���������� �������
  bool                    need_update_activity;         //��������� ���������� ���������� ������� ������
  bool                    need_update_name;             //��������� ���������� �����
  bool                    need_update_area;             //��������� ���������� ������� ������

/// �����������
  Impl (const ConnectionPtr& in_connection, scene_graph::Viewport& in_viewport, object_id_t in_render_target_id)
    : connection (in_connection)
    , viewport (in_viewport)
    , render_target_id (in_render_target_id)
    , id ()
    , is_active (viewport.IsActive ())
    , camera ()
    , camera_name_hash ()
    , need_reconfiguration (true)
    , need_update_renderer (true)
    , need_update_background (true)
    , need_update_camera (true)
    , need_update_transformations (true)
    , need_update_scene (true)
    , need_update_properties (true)
    , need_update_activity (true)
    , need_update_name (true)
    , need_update_area (true)
  {
    if (!connection)
      throw xtl::make_null_argument_exception ("", "connection");

    id = connection->Client ().AllocateId (ObjectType_Viewport);

    bool attached = false;

    try
    {
      connection->Context ().AttachViewportToRenderTarget (render_target_id, id);

      attached = true;

      viewport.AttachListener (this);
    }
    catch (...)
    {
      if (attached)
        connection->Context ().DetachViewportFromRenderTarget (render_target_id, id);

      connection->Client ().DeallocateId (ObjectType_Viewport, id);

      throw;
    }
  }

/// ����������
  ~Impl ()
  {
    try
    {
      viewport.DetachListener (this);

      connection->Context ().DetachViewportFromRenderTarget (render_target_id, id);

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
    camera               = new_camera;

    on_scene_changed_connection.disconnect ();
    on_camera_updated_connection.disconnect ();

    ResetSceneIfChanged ();
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

///��������� ��������� ������
  void OnCameraUpdated ()
  {
    need_update_transformations = true;

    //��������� ����� ���������������� �� �����
  }

///��������� �����
  void SetScene (scene_graph::Scene* new_scene)
  {
    try
    {
      if (scene && &scene->SourceScene () == new_scene)
        return;

      if (!scene && !new_scene)
        return;

      need_reconfiguration = true;
      need_update_scene    = true;

      scene = ScenePtr ();

      if (!new_scene)
      {
        connection->Context ().SetViewportScene (id, 0);
        return;
      }

      scene = connection->Client ().SceneManager ().GetScene (*new_scene);

      connection->Context ().SetViewportScene (id, scene->Id ());
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::client::RenderableView::Impl::SetScene");
      throw;
    }
  }

/// ���������� �� ��������� �����
  void OnSceneChanged ()
  {
    need_update_scene    = true;
    need_reconfiguration = true;

    ResetSceneIfChanged ();
  }

///����� ����� � ������ �������������
  void ResetSceneIfChanged ()
  {
    if (!scene)
      return;

    if (camera)
    { 
      if (camera->Scene () != &scene->SourceScene ())
      {
        scene             = ScenePtr ();
        need_update_scene = true;
      }
    }
    else
    {
      scene             = ScenePtr ();
      need_update_scene = true;
    }
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
        if (camera)
        {
            //�������� �� ��������� ����� � ������

          on_scene_changed_connection  = camera->RegisterEventHandler (scene_graph::NodeEvent_AfterSceneChange, xtl::bind (&Impl::OnSceneChanged, this));
          on_camera_updated_connection = camera->RegisterEventHandler (scene_graph::NodeEvent_AfterUpdate, xtl::bind (&Impl::OnCameraUpdated, this));

          if (!scene || camera->Scene () != &scene->SourceScene ())
            need_update_scene = true;
        }
        else
        {
          on_scene_changed_connection.disconnect ();
          on_camera_updated_connection.disconnect ();

          if (scene)
            need_update_scene = true;
        }

        need_update_camera          = false;
        need_update_transformations = true;
      }
      
        //���������������� �����

      if (need_update_scene)
      {
        if (camera) SetScene (camera->Scene ());
        else        SetScene (0);

        need_update_scene = false;
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

RenderableView::RenderableView (const ConnectionPtr& connection, scene_graph::Viewport& viewport, object_id_t render_target_id)
{
  try
  {
    impl.reset (new Impl (connection, viewport, render_target_id));
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
      //������������� ���������� �������

    impl->connection->Client ().Synchronize ();

      //������������� ������� ������

    impl->Synchronize ();

      //������������� �������������

    if (impl->need_update_transformations)
    {
      if (impl->camera)
      {
        if (impl->camera->NameHash () != impl->camera_name_hash)
        {
          impl->connection->Context ().SetViewportCameraName (impl->id, impl->camera->Name ());

          impl->camera_name_hash = impl->camera->NameHash ();
        }

        impl->connection->Context ().SetViewportCameraWorldMatrix (impl->id, impl->camera->WorldTM ());
        impl->connection->Context ().SetViewportCameraProjectionMatrix (impl->id, impl->camera->ProjectionMatrix ());
      }
      else
      {
        impl->connection->Context ().SetViewportCameraName (impl->id, "");
        impl->connection->Context ().SetViewportCameraWorldMatrix (impl->id, math::mat4f (1.0f));
        impl->connection->Context ().SetViewportCameraProjectionMatrix (impl->id, math::mat4f (1.0f));

        impl->camera_name_hash = 0;
      }

      impl->need_update_transformations = false;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::RenderableView::Synchronize");
    throw;
  }
}
