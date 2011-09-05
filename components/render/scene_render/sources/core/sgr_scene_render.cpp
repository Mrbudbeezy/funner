#include "shared.h"

using namespace render;

//TODO: change screen background

namespace
{

/*
    ���������
*/

const char* LOG_NAME = "render.scene_render"; //��� ������ ����������������

}

/*
    �������� ���������� ������� �����
*/

typedef stl::hash_map<stl::string, RenderTarget> RenderTargetMap;
typedef stl::vector<RenderableViewPtr>           ViewList;

struct SceneRender::Impl: public xtl::reference_counter, public scene_graph::IScreenListener, public scene_graph::IViewportListener
{
  render::RenderManager* manager;         //�������� ���������� ��������� � �������� �����
  scene_graph::Screen*   screen;          //�����
  RenderTargetMap        render_targets;  //������� ������ ����������
  ViewList               views;           //������ �������� ������
  bool                   need_sort_views; //������� ������ ������� ����������
  common::Log            log;             //����� ����������������

///�����������
  Impl ()
    : manager (0)
    , screen (0)
    , need_sort_views (false)
    , log (LOG_NAME)
  {    
  }

///����������
  ~Impl ()
  {
    RemoveAllRenderTargets ();
  }

///��������� �������� ������
  void SetRenderTarget (const char* name, const render::RenderTarget& target)
  {
    try
    {
      if (!name)
        throw xtl::make_null_argument_exception ("", "name");
        
      RenderTargetMap::iterator iter = render_targets.find (name);
      
      if (iter != render_targets.end ())
      {
        iter->second = target;
        return;
      }            

      render_targets.insert_pair (name, target);

      for (ViewList::iterator iter=views.begin (), end=views.end (); iter!=end; ++iter)
        (*iter)->SetRenderTarget (name, target);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::SceneRender::Impl::SetRenderTarget");
      throw;
    }
  }
  
///�������� �������� ������
  void RemoveRenderTarget (const char* name)
  {
    try
    {
      if (!name)
        return;
        
      RenderTargetMap::iterator iter = render_targets.find (name);
      
      if (iter == render_targets.end ())
        return;
      
      for (ViewList::iterator iter=views.begin (), end=views.end (); iter!=end; ++iter)
        (*iter)->RemoveRenderTarget (name);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::SceneRender::Impl::RemoveRenderTarget");
      throw;
    }
  }  
  
///�������� ���� ������� �������
  void RemoveAllRenderTargets ()
  {
    while (!render_targets.empty ())
      RemoveRenderTarget (render_targets.rbegin ()->first.c_str ());
  }
  
///��������� ������� ���� ������ ��������
  void OnScreenChangeBackground (bool state, const math::vec4f& new_color)
  {
    //TODO: change screen background
  }

///������������ ������� ������
  void OnScreenAttachViewport (scene_graph::Viewport& viewport)
  {
    try
    {
      if (!manager)
        return;
        
      if (!screen)
        return;

      RenderableViewPtr view (new RenderableView (*manager, *screen, viewport), false);

      for (RenderTargetMap::iterator iter=render_targets.begin (), end=render_targets.end (); iter!=end; ++iter)
        view->SetRenderTarget (iter->first.c_str (), iter->second);        
        
      views.push_back (view);
      
      try
      {
        viewport.AttachListener (this);
      }
      catch (...)
      {
        views.pop_back ();
        throw;
      }

      need_sort_views = true;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::SceneRender::Impl::OnScreenAttachViewport");
      throw;
    }
  }

///����������� ������� ������
  void OnScreenDetachViewport (scene_graph::Viewport& viewport)
  {
    try
    {
      size_t id = viewport.Id ();
      
      for (ViewList::iterator iter=views.begin (), end=views.end (); iter!=end; ++iter)
        if ((*iter)->Viewport ().Id () == id)
        {
          (*iter)->Viewport ().DetachListener (this);

          views.erase (iter);

          return;
        }
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::SceneRender::Impl::OnScreenAttachViewport");
      throw;
    }    
  }
  
///�������������
  void Initialize ()
  {
    try
    {
      if (!screen)
        return;
        
      if (!manager)
        return;
        
      for (size_t i=0, count=screen->ViewportsCount (); i<count; i++)
        OnScreenAttachViewport (screen->Viewport (i));
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::SceneRender::Impl::Initialize");
      throw;
    }
  }
  
///����������
  void Destroy ()
  {
    try
    {
      while (!views.empty ())
      {
        views.back ()->Viewport ().DetachListener (this);

        views.pop_back ();
      }
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::SceneRender::Impl::Destroy");
      throw;
    }
  }

///����� ������
  void OnScreenDestroy ()
  {
    try
    {
      Destroy ();
      
      screen = 0;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::SceneRender::Impl::OnScreenDestroy");
      throw;
    }
  }
  
///������� ������ �������
  void OnViewportChangeZOrder (int)
  {
    need_sort_views = true;
  }
  
///���������� �������� ������
  struct ViewComparator
  {
    bool operator () (const RenderableViewPtr& view1, const RenderableViewPtr& view2) const
    {
      return view1->Viewport ().ZOrder () < view2->Viewport ().ZOrder ();
    }
  };
  
///���������� �������� ������
  void SortViews ()
  {
    if (!need_sort_views)
      return;

    stl::sort (views.begin (), views.end (), ViewComparator ());

    need_sort_views = false;
  }
};

/*
    ����������� / ����������
*/

SceneRender::SceneRender ()
  : impl (new Impl)
{
}

SceneRender::SceneRender (const SceneRender& render)
  : impl (render.impl)
{
  addref (impl);
}

SceneRender::~SceneRender ()
{
  release (impl);
}

SceneRender& SceneRender::operator = (const SceneRender& render)
{
  SceneRender (render).Swap (*this);
  
  return *this;
}

/*
    ��������� / ����� ������� ����������
*/

void SceneRender::SetRenderManager (render::RenderManager* manager)
{
  try
  {
    if (manager == impl->manager)
      return;
      
    impl->manager = 0;
      
    impl->Destroy ();
    
    impl->manager = manager;        

    try
    {
      impl->Initialize ();
    }
    catch (...)
    {
      impl->manager = 0;
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::SceneRender::SetRenderManager");
    throw;
  }
}

render::RenderManager* SceneRender::RenderManager () const
{
  return impl->manager;
}

/*
    �������� ����� ���� ���������� � ��������
*/

bool SceneRender::IsBindedToRenderer () const
{
  return impl->manager && impl->screen;
}

/*
    ����� (�������� �������� - weak-ref)
*/

void SceneRender::SetScreen (scene_graph::Screen* screen)
{
  try
  {
    if (screen == impl->screen)
      return;
      
    impl->screen = 0;
    
    impl->Destroy ();
    
    impl->screen = screen;
    
    try
    {
      impl->Initialize ();
    }
    catch (...)
    {
      impl->screen = 0;
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::SceneRender::SetScreen");
    throw;
  }
}

scene_graph::Screen* SceneRender::Screen () const
{
  return impl->screen;
}

/*
    ����������� ������� ������� ���������
*/

void SceneRender::SetRenderTarget (const char* name, const render::RenderTarget& target)
{
  impl->SetRenderTarget (name, target);
}

void SceneRender::RemoveRenderTarget (const char* name)
{
  impl->RemoveRenderTarget (name);
}

void SceneRender::RemoveAllRenderTargets ()
{
  impl->RemoveAllRenderTargets ();
}

/*
    ��������� ������� ������� ���������
*/

bool SceneRender::HasRenderTarget (const char* name) const
{
  if (!name)
    return false;

  return impl->render_targets.find (name) != impl->render_targets.end ();
}

RenderTarget SceneRender::RenderTarget (const char* name) const
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    RenderTargetMap::iterator iter = impl->render_targets.find (name);
    
    if (iter == impl->render_targets.end ())
      throw xtl::make_argument_exception ("", "name", name, "Render target not found");
      
    return iter->second;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::SceneRender::RenderTarget");
    throw;
  }
}

/*
    ����������
*/

void SceneRender::Update (Frame* parent_frame)
{
  try
  {
      //�������� ������������ �������� ������
      
    if (impl->need_sort_views)
      impl->SortViews ();
      
      //����������
    
    for (ViewList::iterator iter=impl->views.begin (), end=impl->views.end (); iter!=end; ++iter)    
    {
      try
      {
        (*iter)->UpdateFrame (parent_frame);
      }
      catch (std::exception& e)
      {
        impl->log.Printf ("%s\n    at render::SceneRender::Update", e.what ());
      }
      catch (...)
      {
        impl->log.Printf ("unknown exception\n    at render::SceneRender::Update");
      }
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::SceneRender::Update");
    throw;
  }
}

/*
    �����
*/

void SceneRender::Swap (SceneRender& render)
{
  stl::swap (impl, render.impl);
}

namespace render
{

void swap (SceneRender& render1, SceneRender& render2)
{
  render1.Swap (render2);
}

}
