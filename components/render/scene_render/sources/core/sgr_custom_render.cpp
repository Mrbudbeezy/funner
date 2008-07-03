#include "shared.h"

using namespace render;

/*
    ���������
*/

namespace
{

const char* RENDER_PATH_COMPONENTS_MASK = "render.scene_render.*"; //����� ��� ����������� ����� ����������

}

/*
===================================================================================================
    SceneRenderManagerImpl::RenderPath
===================================================================================================
*/

class SceneRenderManagerImpl::RenderPath: public SceneRenderManager::IRenderPath, public xtl::reference_counter
{
  public:
///�����������
    RenderPath (const char* in_name, const RenderCreater& in_creater) : name (in_name), creater (in_creater) { }

///��� ����
    const char* Name () { return name.c_str (); }

///�������� �������
    ICustomSceneRender* CreateRender (mid_level::IRenderer* renderer)
    {
      try
      {
        return creater (renderer, name.c_str ());
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::SceneRenderManagerImpl::RenderPath::CreateRender(path_name='%s')", name.c_str ());
        throw;
      }
    }

  private:
    stl::string   name;    //��� ���� ����������
    RenderCreater creater; //������� �������� �������
};

/*
===================================================================================================
    SceneRenderManagerImpl
===================================================================================================
*/

/*
   �������� ����������� �� ���������
*/

void SceneRenderManagerImpl::LoadDefaultComponents ()
{
  static common::ComponentLoader loader (RENDER_PATH_COMPONENTS_MASK);  
}

/*
    ����������� ����� ����������
*/

void SceneRenderManagerImpl::RegisterRender (const char* path_name, const RenderCreater& creater)
{
  static const char* METHOD_NAME = "render::SceneRenderManagerImpl::RegisterRender";

  if (!path_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "path_name");
    
  RenderPathMap::iterator iter = paths.find (path_name);
  
  if (iter != paths.end ())
    throw xtl::make_argument_exception (METHOD_NAME, "path_name", path_name, "Render path has already registered");
    
  paths.insert_pair (path_name, RenderPathPtr (new RenderPath (path_name, creater), false));
}

void SceneRenderManagerImpl::UnregisterRender (const char* path_name)
{
  if (!path_name)
    return;
    
  paths.erase (path_name);
}

void SceneRenderManagerImpl::UnregisterAllRenders ()
{
  paths.clear ();
}

/*
    ������� ����� ����������
*/

SceneRenderManagerImpl::Iterator SceneRenderManagerImpl::CreateIterator ()
{
  struct RenderPathSelector
  {
    SceneRenderManager::IRenderPath& operator () (RenderPathMap::value_type& v) const { return *v.second; }
  };
  
  LoadDefaultComponents ();

  return Iterator (paths.begin (), paths.begin (), paths.end (), RenderPathSelector ());  
}

/*
    �������� �������
*/

CustomSceneRenderPtr SceneRenderManagerImpl::CreateRender (mid_level::IRenderer* renderer, const char* path_name)
{
  try
  {
    if (!renderer)
      throw xtl::make_null_argument_exception ("", "renderer");
      
    if (!path_name)
      throw xtl::make_null_argument_exception ("", "path_name");
      
    LoadDefaultComponents ();

    RenderPathMap::iterator iter = paths.find (path_name);

    if (iter == paths.end ())
      throw xtl::make_argument_exception ("", "path_name", path_name, "Render path has not registered");

    return CustomSceneRenderPtr (iter->second->CreateRender (renderer), false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::SceneRenderManagerImpl::CreateRender");
    throw;
  }
}

/*
    ��������� ���������� ���������
*/

SceneRenderManagerImpl& SceneRenderManagerImpl::Instance ()
{
    //�������� ��� ��������� ����� ����������
  typedef common::Singleton<SceneRenderManagerImpl> SceneRenderManagerSingleton;
  
  return SceneRenderManagerSingleton::Instance ();
}

/*
===================================================================================================
    SceneRenderManager
===================================================================================================
*/

/*
    ����������� ����� ����������
*/

void SceneRenderManager::RegisterRender (const char* path_name, const RenderCreater& creater)
{
  try
  {
    SceneRenderManagerImpl::Instance ().RegisterRender (path_name, creater);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::SceneRenderManager::RegisterRender");
    throw;
  }
}

void SceneRenderManager::UnregisterRender (const char* path_name)
{
  SceneRenderManagerImpl::Instance ().UnregisterRender (path_name);
}

void SceneRenderManager::UnregisterAllRenders ()
{
  SceneRenderManagerImpl::Instance ().UnregisterAllRenders ();
}

/*
    ������� ����� ����������
*/

SceneRenderManager::Iterator SceneRenderManager::CreateIterator ()
{
  try
  {
    return SceneRenderManagerImpl::Instance ().CreateIterator ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::SceneRenderManager::CreateIterator");
    throw;
  }
}
