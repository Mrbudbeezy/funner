#include "shared.h"

using namespace render::scene::client;

/*
    Описание реализации области вывода
*/

struct RenderableView::Impl: public scene_graph::IViewportListener
{
  ConnectionPtr           connection;                   //соединение
  scene_graph::Viewport   viewport;                     //ссылка на область вывода
  PropertyMapSynchronizer properties_sync;              //синхронизатор свойств
  object_id_t             render_target_id;             //идентификатор цели рендеринга
  object_id_t             id;                           //идентификатор области вывода
  bool                    is_active;                    //активна ли области отрисовки
  scene_graph::Camera*    camera;                       //текущая камера
  ScenePtr                scene;                        //текущая сцена
  xtl::auto_connection    on_scene_changed_connection;  //соединение с сигналом оповещения об изменении сцены
  xtl::auto_connection    on_camera_updated_connection; //соединение с сигналом оповещения об обновлении трансформаций
  size_t                  camera_name_hash;             //хэш имени камеры
  bool                    need_reconfiguration;         //конфигурация изменена
  bool                    need_update_renderer;         //требуется обновить рендер
  bool                    need_update_background;       //требуется обновить параметры очистки
  bool                    need_update_camera;           //требуется обновить камеру
  bool                    need_update_transformations;  //требуется обновить трансформации
  bool                    need_update_scene;            //требуется обновить сцену
  bool                    need_update_properties;       //требуется обновление свойств
  bool                    need_update_activity;         //требуется обновление активности области вывода
  bool                    need_update_name;             //требуется обновление имени
  bool                    need_update_area;             //требуется обновление области вывода
  bool                    need_update_zorder;           //требуется обновление порядка вывода

/// Конструктор
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
    , need_update_zorder (true)
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

/// Деструктор
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

///Область вывода обновлена
  void OnViewportChangeArea (const scene_graph::Rect&, float, float)
  {
    need_reconfiguration = true;
    need_update_area     = true;
  }

///Порядок вывода обновлен
  void OnViewportChangeZOrder (int)
  {
    need_reconfiguration = true;
    need_update_zorder   = true;
  }

///Имя обновлено
  void OnViewportChangeName (const char*)
  {
    need_reconfiguration = true;
    need_update_name     = true;
  }

///Изменена камера
  void OnViewportChangeCamera (scene_graph::Camera* new_camera)
  {
    need_reconfiguration = true;
    need_update_camera   = true;
    camera               = new_camera;

    on_scene_changed_connection.disconnect ();
    on_camera_updated_connection.disconnect ();

    ResetSceneIfChanged ();
  }  

///Изменена активность области вывода
  void OnViewportChangeActive (bool)
  {
    need_reconfiguration = true;
    need_update_activity = true;
  }
  
///Изменены параметры очистки области вывода
  void OnViewportChangeBackground (bool, const math::vec4f&)
  {
    need_reconfiguration   = true;
    need_update_background = true;
  }

///Изменен путь рендеринга
  void OnViewportChangeTechnique (const char*)
  {
    need_reconfiguration = true;
    need_update_renderer = true;
  }

///Изменены свойства рендеринга
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

///Обновлены параметры камеры
  void OnCameraUpdated ()
  {
    need_update_transformations = true;

    //установка флага переконфигурации не нужна
  }

///Установка сцены
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

/// Оповещение об изменении сцены
  void OnSceneChanged ()
  {
    need_update_scene    = true;
    need_reconfiguration = true;

    ResetSceneIfChanged ();
  }

///Сброс сцены в случае необходимости
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
  
///Синхронизация
  void Synchronize ()
  {
    try
    {
      if (!need_reconfiguration)
        return;

      Context& context = connection->Context ();

        //переконфигурация имени

      if (need_update_name)
      {
        context.SetViewportName (id, viewport.Name ());

        need_update_name = false;
      }
        
        //переконфигурация области вывода

      if (need_update_area)
      {
        const scene_graph::Rect& r = viewport.Area ();

        context.SetViewportArea (id, r.x, r.y, r.width, r.height, viewport.MinDepth (), viewport.MaxDepth ());

        need_update_area = false;
      }

        //переконфигурация порядка вывода

      if (need_update_zorder)
      {
        context.SetViewportZOrder (id, viewport.ZOrder ());

        need_update_zorder = false;
      }

        //переконфигурация рендера
        
      if (need_update_renderer)
      {        
        context.SetViewportTechnique (id, viewport.Technique ());

        need_update_renderer = false;
      }

        //переконфигурация активности

      if (need_update_activity)
      {
        context.SetViewportActive (id, viewport.IsActive ());

        need_update_activity = false;
      }
            
        //переконфигурация параметров очистки
        
      if (need_update_background)
      {
        context.SetViewportBackground (id, viewport.BackgroundState (), viewport.BackgroundColor ());

        need_update_background = false;
      }

        //переконфигурация камеры

      if (need_update_camera)
      {
        camera = viewport.Camera ();

        if (camera)
        {
            //подписка на изменение сцены в камере

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
      
        //переконфигурация сцены

      if (need_update_scene)
      {
        if (camera) SetScene (camera->Scene ());
        else        SetScene (0);

        need_update_scene = false;
      }

        //переконфигурация свойств

      if (need_update_properties)
      {        
        properties_sync = connection->Client ().CreateSynchronizer (viewport.Properties ());

        connection->Client ().Synchronize ();

        context.SetViewportProperties (id, viewport.Properties ().Id ());

        need_update_properties = false;
      }

        //синхронизация сцены

      connection->Client ().SceneManager ().Update ();
      
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
    Конструктор / деструктор
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
    Область вывода
*/

const scene_graph::Viewport& RenderableView::Viewport ()
{
  return impl->viewport;
}

/*
    Идентификатор
*/

object_id_t RenderableView::Id ()
{
  return impl->id;
}

/*
    Синхронизация с сервером
*/

void RenderableView::Synchronize ()
{
  try
  {
      //синхронизация параметров клиента

    impl->connection->Client ().Synchronize ();

      //синхронизация области вывода

    impl->Synchronize ();

      //синхронизация трансформаций

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
