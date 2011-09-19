#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

namespace
{

/*
    ���������
*/

const size_t SUB_TECHNIQUE_ARRAY_RESERVE              = 8;    //������������� ���������� ������ ����������
const size_t TRAVERSE_RESULT_LIGHTS_RESERVE_SIZE      = 64;   //������������� ���������� ���������� �����
const size_t TRAVERSE_RESULT_RENDERABLES_RESERVE_SIZE = 1024; //������������� ���������� ��������������� ��������

}

/*
    �������� ���������� �������
*/

typedef stl::vector<TechniquePtr> TechniqueArray;

struct View::Impl
{
  RenderPtr            render;                     //������
  RenderManager        manager;                    //�������� ����������
  stl::string          technique_name;             //��� ������� ����������
  TechniqueArray       sub_techniques;             //��������� �������
  render::Frame        frame;                      //����
  scene_graph::Camera* camera;                     //������� ������
  common::PropertyMap  properties;                 //�������� ����������
  ScenePtr             scene;                      //������� �����
  TraverseResult       camera_traverse_result;     //��������� ������ ����� �� ������
  bool                 need_update_properties;     //���������� �������� �������� ����������
  bool                 need_update_camera;         //���������� �������� ������
  bool                 need_update_sub_techniques; //���������� �������� ������� ����������
  Log                  log;                        //����� ����������������

///�����������
  Impl (RenderManager& in_manager, const char* in_technique)
    : render (Render::GetRender (in_manager))
    , manager (in_manager)
    , technique_name (in_technique)
    , frame (in_manager.CreateFrame ())
    , camera ()
    , need_update_properties (true)
    , need_update_camera (true)
    , need_update_sub_techniques (true)
  {
    sub_techniques.reserve (SUB_TECHNIQUE_ARRAY_RESERVE);
    camera_traverse_result.renderables.reserve (TRAVERSE_RESULT_RENDERABLES_RESERVE_SIZE);
    camera_traverse_result.lights.reserve (TRAVERSE_RESULT_LIGHTS_RESERVE_SIZE);    
    
    log.Printf ("View created for technique '%s'", technique_name.c_str ());
  }
  
///����������
  ~Impl ()
  {
    log.Printf ("View destroyed for technique '%s'", technique_name.c_str ());
  }
  
///���������� ������
  void UpdateSubTechniques ()
  {
    try
    {
      if (!need_update_sub_techniques)
        return;
        
        //�������� ������������
        
      sub_techniques.clear ();
      
      for (common::Parser::NamesakeIterator iter=manager.Configuration ().First ("technique"); iter; ++iter)
      {
        const char* technique = common::get<const char*> (*iter, "", "");        
        
        if (xtl::xstrcmp (technique, technique_name.c_str ()))
          continue;          
          
        common::ParseNode parent_technique_node = *iter;
          
        for (common::Parser::Iterator iter=parent_technique_node.First (); iter; ++iter)
        {
          try
          {
            TechniquePtr technique = TechniqueManager::CreateTechnique (iter->Name (), manager, *iter);
            
            if (!technique)
              throw xtl::format_operation_exception ("", "Can't create technique '%s'", iter->Name ());
            
            technique->SetName (iter->Name ());

            sub_techniques.push_back (technique);
          }
          catch (std::exception& e)
          {
            log.Printf ("%s\n    at create sub-technique '%s' for technique '%s'\n    at render::scene_render3d::View::Impl::UpdateSubTechniques\n    at render::scene_render3d::View::UpdateFrame",
              e.what (), iter->Name (), technique_name.c_str ());
          }
          catch (...)
          {
            log.Printf ("unknown exception\n    at create sub-technique '%s' for technique '%s'\n    at render::scene_render3d::View::Impl::UpdateSubTechniques\n    at render::scene_render3d::View::UpdateFrame",
              iter->Name (), technique_name.c_str ());
          }
        }
      }              
        
      need_update_sub_techniques = false;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene_render3d::View::Impl::UpdateSubTechniques");
      throw;
    }    
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

namespace
{

/// ��� ���������� ������ ����� 
struct CameraTraverseResult: public ITraverseResultCache
{
  scene_graph::Camera* camera;   //������
  Scene*               scene;    //�����
  TraverseResult&      result;   //��������� ������
  bool                 computed; //��������� ���������
  
///�����������
  CameraTraverseResult (scene_graph::Camera* in_camera, Scene* in_scene, TraverseResult& in_result)
    : camera (in_camera)
    , scene (in_scene)
    , result (in_result)
    , computed (false) { }
  
///��������� ����������
  TraverseResult& Result ()
  {
    try
    {
      if (computed)
        return result;
        
      if (!camera)
        throw xtl::format_operation_exception ("", "Camera is null");
        
      if (!scene)
        throw xtl::format_operation_exception ("", "Scene is null");

      scene->Traverse (camera->Frustum (), result, Collect_All);
       
      computed = true;
      
      return result;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene_render3d::CameraTraverseResult::Result");
      throw;
    }
  }
};

}

void View::UpdateFrame ()
{
  try
  {
      //�������� ������������� ���������� ������ ����������
      
    if (impl->need_update_sub_techniques)
      impl->UpdateSubTechniques ();
    
      //�������� ������������� ���������� ������� ����������
      
    if (impl->need_update_properties)
    {
      for (TechniqueArray::iterator iter=impl->sub_techniques.begin (), end=impl->sub_techniques.end (); iter!=end;)
      {
        try
        {
          (*iter)->UpdateProperties (impl->properties);
          
           ++iter;
           
           continue;
        }
        catch (std::exception& e)
        {
          impl->log.Printf ("%s\n    at update sub-technique '%s' properties for technique '%s'\n    at render::scene_render3d::View::UpdateFrame\n    at render::scene_render3d::View::UpdateFrame", e.what (), (*iter)->Name (), impl->technique_name.c_str ());
        }
        catch (...)
        {
          impl->log.Printf ("unknown exception\n    at update sub-technique '%s' properties for technique '%s'\n    at render::scene_render3d::View::UpdateFrame", (*iter)->Name (), impl->technique_name.c_str ());
        }
        
        impl->sub_techniques.erase (iter);
        
        end = impl->sub_techniques.end ();
      }      

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
      
      for (TechniqueArray::iterator iter=impl->sub_techniques.begin (), end=impl->sub_techniques.end (); iter!=end;)
      {
        try
        {
          (*iter)->SetCamera (impl->camera);
          
          ++iter;
          
          continue;
        }
        catch (std::exception& e)
        {
          impl->log.Printf ("%s\n    at update sub-technique '%s' camera for technique '%s'\n    at render::scene_render3d::View::UpdateFrame", e.what (), (*iter)->Name (), impl->technique_name.c_str ());          
        }
        catch (...)
        {
          impl->log.Printf ("unknown exception\n    at update sub-technique '%s' camera for technique '%s'\n    at render::scene_render3d::View::UpdateFrame", (*iter)->Name (), impl->technique_name.c_str ());
        }
                
        impl->sub_techniques.erase (iter);
        
        end = impl->sub_techniques.end ();        
      }

      impl->need_update_camera = false;
    }        
    
      //���������� ���� ����������� ������ �����
      
    CameraTraverseResult camera_traverse_result (impl->camera, &*impl->scene, impl->camera_traverse_result);
    
      //���������� �����
      
    if (impl->scene)
    {
      for (TechniqueArray::iterator iter=impl->sub_techniques.begin (), end=impl->sub_techniques.end (); iter!=end;)
      {
        try
        {
          (*iter)->UpdateFrame (*impl->scene, impl->frame, camera_traverse_result);
          
          ++iter;
          
          continue;
        }
        catch (std::exception& e)
        {
          impl->log.Printf ("%s\n    at update sub-technique '%s' frame for technique '%s'\n    at render::scene_render3d::View::UpdateFrame", e.what (), (*iter)->Name (), impl->technique_name.c_str ());          
        }
        catch (...)
        {
          impl->log.Printf ("unknown exception\n    at update sub-technique '%s' frame for technique '%s'\n    at render::scene_render3d::View::UpdateFrame", (*iter)->Name (), impl->technique_name.c_str ());
        }

        impl->sub_techniques.erase (iter);

        end = impl->sub_techniques.end ();
      }
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

void View::ResetTechniqueCache ()
{
  impl->sub_techniques.clear ();
  
  impl->need_update_sub_techniques = true;
}
