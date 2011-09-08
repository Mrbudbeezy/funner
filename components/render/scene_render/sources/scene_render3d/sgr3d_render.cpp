#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

namespace
{

///�������� ��������
class RenderManagerRegistry
{
  public:
///����������� �������
    void RegisterRender (const RenderManager& manager, Render& render)
    {
      renders.insert_pair (&manager, &render);
    }

///������ ����������� �������
    void UnregisterRender (const RenderManager& manager)
    {
      renders.erase (&manager);
    }
    
///��������� �������
    RenderPtr GetRender (const RenderManager& manager)
    {
      RenderMap::iterator iter = renders.find (&manager);

      if (iter != renders.end ())
        return iter->second;

      return RenderPtr (new Render (manager), false);
    }
    
///�������� ����������� �������
    bool CheckTechnique (const RenderManager& manager, const char* technique)
    {
      if (!technique)
        return false;
        
      RenderMap::iterator iter = renders.find (&manager);

      if (iter != renders.end ())
        return iter->second->HasTechnique (technique);         

      for (common::Parser::NamesakeIterator iter=manager.Configuration ().First ("technique"); iter; ++iter)
      {
        const char* technique_name = common::get<const char*> (*iter, "", "");
                
        if (!xtl::xstrcmp (technique_name, technique))
          return true;
      }

      return false;
    }

  private:
    typedef stl::hash_map<const RenderManager*, Render*> RenderMap;
    
  private:
    RenderMap renders; //����� ������������ ���������� ���������� �������� �����
};

typedef common::Singleton<RenderManagerRegistry> RenderManagerSingleton;

}

/*
    �������� ���������� �������
*/

typedef stl::hash_map<scene_graph::Scene*, Scene*> SceneMap;
typedef stl::list<View*>                           ViewList;

struct Render::Impl
{
  RenderManager        manager;                          //�������� ����������
  ViewList             views;                            //������ �������� ������
  SceneMap             scenes;                           //����� ����
  xtl::auto_connection configuration_changed_connection; //���������� � ������������ ��������� ������������
  Log                  log;                              //����� ����������������  
  
///�����������
  Impl (const RenderManager& in_manager)
    : manager (in_manager)
  {
    configuration_changed_connection = manager.RegisterEventHandler (RenderManagerEvent_OnConfigurationChanged, xtl::bind (&Impl::OnConfigurationChanged, this));
    
    log.Printf ("Render created");
  }
  
///����������
  ~Impl ()
  {
    log.Printf ("Render destroyed");    
  }
  
///����� ����� ������
  void ResetTechniqueCaches ()
  {
    for (ViewList::iterator iter=views.begin (), end=views.end (); iter!=end; ++iter)
      (*iter)->RemoveAllTechniques ();    
  }
  
///����������������
  void Reconfigure ()
  {
    try
    {
        //����� �����

      ResetTechniqueCaches ();

        //�������� ������������
      
      for (common::Parser::NamesakeIterator iter=manager.Configuration ().First ("technique"); iter; ++iter)
      {
        const char* technique = common::get<const char*> (*iter, "", "");
        
        if (!*technique)
          continue;
          
        //TODO: parse technique
      }      
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene_render3d::Render::Impl::Reconfigure");
      throw;
    }
  }
  
///���������� ��������� ������������
  void OnConfigurationChanged ()
  {
    ResetTechniqueCaches ();
  }
};

/*
    ����������� / ����������
*/

Render::Render (const RenderManager& manager)
{
  try
  {
    impl = new Impl (manager);
    
    RenderManagerSingleton::Instance ()->RegisterRender (manager, *this);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::Render::Render");
    throw;
  }
}

Render::~Render ()
{
  try
  {
    RenderManagerSingleton::Instance ()->UnregisterRender (impl->manager);
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

/*
    ����������� �������� ������
*/

void Render::RegisterView (View& view)
{
  for (ViewList::iterator iter=impl->views.begin (), end=impl->views.end (); iter!=end; ++iter)
    if (*iter == &view)
      return;
      
  impl->views.push_back (&view);
}

void Render::UnregisterView (View& view)
{
  impl->views.remove (&view);
}

/*
    ����������� ����
*/

void Render::RegisterScene (scene_graph::Scene& src_scene, Scene& render_scene)
{
  impl->scenes.insert_pair (&src_scene, &render_scene);
}

void Render::UnregisterScene (scene_graph::Scene& src_scene)
{
  impl->scenes.erase (&src_scene);
}

/*
    ��������� �����
*/

ScenePtr Render::GetScene (scene_graph::Scene& scene)
{
  try
  {
    SceneMap::iterator iter = impl->scenes.find (&scene);
    
    if (iter != impl->scenes.end ())
      return iter->second;
      
    return ScenePtr (new Scene (scene, this), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::Render::GetScene");
    throw;
  }
}
    
/*
    ��������� �������
*/

RenderPtr Render::GetRender (const RenderManager& manager)
{
  try
  {
    return RenderManagerSingleton::Instance ()->GetRender (manager);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::Render::GetRender");
    throw;
  }
}

/*
    �������� ����������� �������
*/

bool Render::CheckTechnique (const RenderManager& manager, const char* technique)
{
  try
  {
    return RenderManagerSingleton::Instance ()->CheckTechnique (manager, technique);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::Render::CheckTechnique");
    throw;
  }
}

bool Render::HasTechnique (const char* technique)
{
  return true; //???????TODO
}
