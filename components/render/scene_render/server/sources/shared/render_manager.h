#ifndef RENDER_SCENE_SERVER_RENDER_MANAGER_SHARED_HEADER
#define RENDER_SCENE_SERVER_RENDER_MANAGER_SHARED_HEADER

#include <common/log.h>

#include <render/manager.h>

#include <shared/batching_manager.h>

namespace common
{

//forward declaration
class ParseNode;

}

namespace render
{

namespace scene
{

namespace server
{

//forward declarations
struct TraverseResult;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Слушатель событий менеджера рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
class IRenderManagerListener
{
  public:
    virtual void OnRenderManagerConfigurationChanged (const common::ParseNode&) {}

  protected:
    virtual ~IRenderManagerListener () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обертка над менеджером рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderManager  (const char* name);
    RenderManager  (const RenderManager&);
    ~RenderManager ();

    RenderManager& operator = (const RenderManager&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поток отладочного протоколирования
///////////////////////////////////////////////////////////////////////////////////////////////////
    common::Log& Log ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер
///////////////////////////////////////////////////////////////////////////////////////////////////
    manager::RenderManager& Manager ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер пакетирования
///////////////////////////////////////////////////////////////////////////////////////////////////
    server::BatchingManager& BatchingManager ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Результат обхода сцены
///////////////////////////////////////////////////////////////////////////////////////////////////
    TraverseResult& TraverseResultStorage ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подписка на события менеджера
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AttachListener     (IRenderManagerListener*);
    void DetachListener     (IRenderManagerListener*);
    void DetachAllListeners ();

  private:
    struct Impl;
    Impl* impl;
};

}

}

}

#endif
