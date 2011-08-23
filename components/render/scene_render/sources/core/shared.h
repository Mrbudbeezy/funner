#ifndef RENDER_SCENE_RENDER_CORE_SHARED_HEADER
#define RENDER_SCENE_RENDER_CORE_SHARED_HEADER

#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/component.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <render/scene_render.h>

namespace render
{

typedef xtl::com_ptr<ISceneRender> ISceneRenderPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ��⥩ ७��ਭ��
///////////////////////////////////////////////////////////////////////////////////////////////////
class SceneRenderManagerImpl: public xtl::noncopyable
{
  public:
    typedef SceneRenderManager::RenderCreator RenderCreator;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� / ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SceneRenderManagerImpl  ();
    ~SceneRenderManagerImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ७��஢
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterRender       (const char* renderer, const RenderCreator& creator);
    void UnregisterRender     (const char* renderer);
    void UnregisterAllRenders ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ७���
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISceneRenderPtr CreateRender (RenderManager& manager, const char* renderer);    
    
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

typedef common::Singleton<SceneRenderManagerImpl> SceneRenderManagerSingleton;

}

#endif
