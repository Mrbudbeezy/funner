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
    ���������� ��������� ����� ����������
===================================================================================================
*/

class SceneRenderManagerImpl
{
  public:
    typedef SceneRenderManager::RenderCreater RenderCreater;
    typedef SceneRenderManager::Iterator      Iterator;
  
///����������� ����� ����������
    void RegisterRender (const char* path_name, const RenderCreater& creater)
    {
      static const char* METHOD_NAME = "render::SceneRenderManagerImpl::RegisterRender";

      if (!path_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "path_name");
        
      RenderPathMap::iterator iter = paths.find (path_name);
      
      if (iter != paths.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "path_name", path_name, "Render path has already registered");
        
      paths.insert_pair (path_name, RenderPathPtr (new RenderPath (path_name, creater), false));      
    }

///������ ����������� ���� ����������
    void UnregisterRender (const char* path_name)
    {
      if (!path_name)
        return;
        
      paths.erase (path_name);
    }

///������ ����������� ���� ����� ����������
    void UnregisterAllRenders ()
    {
      paths.clear ();
    }

///������� ����� ����������
    Iterator CreateIterator ()
    {
      LoadDefaultComponents ();

      return Iterator (paths.begin (), paths.begin (), paths.end (), RenderPathSelector ());        
    }

///�������� �������
    CustomSceneRenderPtr CreateRender (mid_level::IRenderer* renderer, const char* path_name)
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
          
        try
        {          
          return CustomSceneRenderPtr (iter->second->CreateRender (renderer), false);
        }
        catch (xtl::exception& exception)
        {
          exception.touch ("create render path '%s'", path_name);
          throw;
        }
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::SceneRenderManagerImpl::CreateRender");
        throw;
      }
    }
        
  private:
///�������� ����������� �� ���������
    void LoadDefaultComponents ()
    {
      static common::ComponentLoader loader (RENDER_PATH_COMPONENTS_MASK);
    }

  private:
///���������� ���� ����������
    struct RenderPath: public SceneRenderManager::IRenderPath, public xtl::reference_counter
    {
      RenderPath (const char* in_name, const RenderCreater& in_creater) : name (in_name), creater (in_creater) { }
      
///��������� ����� ����
      const char* Name () { return name.c_str (); }
      
///�������� ���� ����������
      ICustomSceneRender* CreateRender (mid_level::IRenderer* renderer)
      {
        return creater (renderer, name.c_str ());
      }

      stl::string   name;    //��� ���� ����������
      RenderCreater creater; //������� �������� �������
    };

    typedef xtl::intrusive_ptr<RenderPath>                           RenderPathPtr;
    typedef stl::hash_map<stl::hash_key<const char*>, RenderPathPtr> RenderPathMap;
    
///�������� ���������� ���� ����������
    struct RenderPathSelector
    {
      SceneRenderManager::IRenderPath& operator () (RenderPathMap::value_type& v) const { return *v.second; }
    };    

  private:
    RenderPathMap paths; //����� ����� ����������
};

//�������� ��� ��������� ����� ����������
typedef common::Singleton<SceneRenderManagerImpl> SceneRenderManagerSingleton;

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
    SceneRenderManagerSingleton::Instance ().RegisterRender (path_name, creater);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::SceneRenderManager::RegisterRender");
    throw;
  }
}

void SceneRenderManager::UnregisterRender (const char* path_name)
{
  SceneRenderManagerSingleton::Instance ().UnregisterRender (path_name);
}

void SceneRenderManager::UnregisterAllRenders ()
{
  SceneRenderManagerSingleton::Instance ().UnregisterAllRenders ();
}

/*
    ������� ����� ����������
*/

SceneRenderManager::Iterator SceneRenderManager::CreateIterator ()
{
  try
  {
    return SceneRenderManagerSingleton::Instance ().CreateIterator ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::SceneRenderManager::CreateIterator");
    throw;
  }
}

/*
    �������� ���� ����������
*/

namespace render
{

CustomSceneRenderPtr create_render_path (mid_level::IRenderer* renderer, const char* path_name)
{
  return SceneRenderManagerSingleton::Instance ().CreateRender (renderer, path_name);
}

}
