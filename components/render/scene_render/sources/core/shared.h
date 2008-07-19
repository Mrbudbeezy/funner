#ifndef RENDER_SCENE_RENDER_CORE_SHARED_HEADER
#define RENDER_SCENE_RENDER_CORE_SHARED_HEADER

#include <cstdarg>

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
#include <xtl/ref.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>

#include <common/singleton.h>
#include <common/strlib.h>
#include <common/component.h>

#include <sg/camera.h>

#include <render/mid_level/driver.h>

#include <render/custom_render.h>
#include <render/desktop.h>
#include <render/scene_render.h>

namespace render
{

typedef xtl::com_ptr<ICustomSceneRender> CustomSceneRenderPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SceneRenderManagerImpl
{
  public:
    typedef SceneRenderManager::RenderCreater RenderCreater;
    typedef SceneRenderManager::Iterator      Iterator;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterRender       (const char* path_name, const RenderCreater& creater);
    void UnregisterRender     (const char* path_name);
    void UnregisterAllRenders ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Iterator CreateIterator ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    CustomSceneRenderPtr CreateRender (mid_level::IRenderer* renderer, const char* path_name);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static SceneRenderManagerImpl& Instance ();
    
  private:
    void LoadDefaultComponents ();

  private:
    class RenderPath;

    typedef xtl::intrusive_ptr<RenderPath>                           RenderPathPtr;
    typedef stl::hash_map<stl::hash_key<const char*>, RenderPathPtr> RenderPathMap;

  private:
    RenderPathMap paths; //����� ����� ����������
};

}

#endif
