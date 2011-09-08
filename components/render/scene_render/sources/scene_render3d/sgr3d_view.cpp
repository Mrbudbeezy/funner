#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

namespace
{

/*
    ���������
*/

const size_t TECHNIQUE_ARRAY_RESERVE = 8; //������������� ���������� ������ ����������

}

/*
    �������� ���������� �������
*/

typedef stl::vector<TechniquePtr> TechniqueArray;

struct View::Impl
{
  RenderPtr            render;                    //������
  stl::string          technique_name;            //��� ������� ����������
  TechniqueArray       techniques;                //�������
  render::Frame        frame;                     //����
  scene_graph::Camera* camera;                    //������� ������
  common::PropertyMap  properties;                //�������� ����������
  ScenePtr             scene;                     //������� �����
  bool                 need_update_properties;    //���������� �������� �������� ����������
  bool                 need_update_camera;        //���������� �������� ������
  Log                  log;                       //����� ����������������

///�����������
  Impl (RenderManager& in_manager, const char* in_technique)
    : render (Render::GetRender (in_manager))
    , technique_name (in_technique)
    , frame (in_manager.CreateFrame ())
    , camera ()
    , need_update_properties (true)
    , need_update_camera (true)
  {
    techniques.reserve (TECHNIQUE_ARRAY_RESERVE);
    
    log.Printf ("View created for technique '%s'", technique_name.c_str ());
  }
  
///����������
  ~Impl ()
  {
    log.Printf ("View destroyed for technique '%s'", technique_name.c_str ());
  }
};

/*
    ����������� / ����������
*/

View::View (RenderManager& manager, const char* technique)
{
  try
  {
    if (!technique)
      throw xtl::make_null_argument_exception ("", "technique");
    
    impl = new Impl (manager, technique);
    
    impl->render->RegisterView (*this);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::View::View");
    throw;
  }
}

View::~View ()
{
  try
  {
    impl->render->UnregisterView (*this);
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

/*
    ����
*/

render::Frame& View::Frame ()
{
  return impl->frame;
}

/*
    ���������� ������
*/

void View::UpdateCamera (scene_graph::Camera* camera)
{
  impl->camera             = camera;
  impl->need_update_camera = true;
}

/*
    ���������� ������� ����������
*/

void View::UpdateProperties (const common::PropertyMap& properties)
{
  impl->properties             = properties;
  impl->need_update_properties = true;
}

/*
    ���������� ����������� �����
*/

void View::UpdateFrame ()
{
  try
  {
      //�������� ������������� ���������� ������� ����������
      
    if (impl->need_update_properties)
    {
      for (TechniqueArray::iterator iter=impl->techniques.begin (), end=impl->techniques.end (); iter!=end; ++iter)
        (*iter)->UpdateProperties (impl->properties);

      impl->need_update_properties = false;
    }
    
      //�������� ������������� ���������� ������
      
    if (impl->need_update_camera)
    {
        //���������� �����
        
      if (impl->camera && impl->camera->Scene ())
      {
        impl->scene = impl->render->GetScene (*impl->camera->Scene ());
      }
      else
      {
        impl->scene = ScenePtr ();
      }
        
        //���������� ������
      
      for (TechniqueArray::iterator iter=impl->techniques.begin (), end=impl->techniques.end (); iter!=end; ++iter)
        (*iter)->UpdateCamera (impl->camera);

      impl->need_update_camera = false;
    }        
    
      //���������� �����
      
    if (impl->scene)
    {
      for (TechniqueArray::iterator iter=impl->techniques.begin (), end=impl->techniques.end (); iter!=end; ++iter)
        (*iter)->UpdateFrame (*impl->scene, impl->frame);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render:scene_graph3d::View::UpdateFrame");
    throw;
  }
}

/*
    ������� ������
*/

void View::AddRef ()
{
  addref (this);
}

void View::Release ()
{
  release (this);
}

/*
    ��� ������� ����������
*/

const char* View::TechniqueName ()
{
  return impl->technique_name.c_str ();
}

/*
    ���������� ��������� ����������
*/

void View::AddTechnique (const TechniquePtr& technique)
{
  impl->techniques.push_back (technique);
}

void View::RemoveAllTechniques ()
{
  impl->techniques.clear ();
}
