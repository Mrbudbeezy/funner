#ifndef RENDER_SCENE_RENDER_CORE_SHARED_HEADER
#define RENDER_SCENE_RENDER_CORE_SHARED_HEADER

#include <stl/hash_map>
#include <stl/hash_set>
#include <stl/vector>
#include <stl/string>
#include <stl/algorithm>

#include <xtl/intrusive_ptr.h>
#include <xtl/shared_ptr.h>
#include <xtl/function.h>
#include <xtl/bind.h>
#include <xtl/iterator.h>
#include <xtl/reference_counter.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>

#include <common/singleton.h>
#include <common/strlib.h>

#include <sg/camera.h>

#include <render/mid_level/driver.h>

#include <render/manager.h>
#include <render/scene_render.h>

namespace render
{

typedef xtl::com_ptr<ICustomSceneRender> CustomSceneRenderPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ��⥩ ७��ਭ��
///////////////////////////////////////////////////////////////////////////////////////////////////
class SceneRenderManagerImpl
{
  public:
    typedef SceneRenderManager::RenderCreater RenderCreater;
    typedef SceneRenderManager::Iterator      Iterator;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��⥩ ७��ਭ��
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterRender       (const char* path_name, const RenderCreater& creater);
    void UnregisterRender     (const char* path_name);
    void UnregisterAllRenders ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��ॡ�� ��⥩ ७��ਭ��
///////////////////////////////////////////////////////////////////////////////////////////////////
    Iterator CreateIterator ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ७���
///////////////////////////////////////////////////////////////////////////////////////////////////
    CustomSceneRenderPtr CreateRender (mid_level::IRenderer* renderer, const char* path_name);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����祭�� ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static SceneRenderManagerImpl& Instance ();

  private:
    class RenderPath;

    typedef xtl::intrusive_ptr<RenderPath>                           RenderPathPtr;
    typedef stl::hash_map<stl::hash_key<const char*>, RenderPathPtr> RenderPathMap;

  private:
    RenderPathMap paths; //���� ��⥩ ७��ਭ��
};

}

#endif
