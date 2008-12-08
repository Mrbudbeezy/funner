#include "shared.h"

using namespace scene_graph;

namespace
{

/*
    ���������
*/

const char* DEFAULT_COMPONENTS_MASK = "scene_graph.controllers.*";

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ControllerManagerImpl
{
  public:
    typedef ControllerManager::ControllerCreator ControllerCreator;
  
///����������� �������� �������� �����������
    void Register (const char* name, const ControllerCreator& creator)
    {
      static const char* METHOD_NAME = "scene_graph::ControllerManagerImpl::Register";
      
        //�������� ������������ ����������

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");

        //�������� ��������� �����������
        
      CreatorMap::iterator iter = creators.find (name);
      
      if (iter != creators.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Controller already registered");

        //����������� ������ �����������

      creators.insert_pair (name, creator);
    }
    
///������ ����������� �����������
    void Unregister (const char* name)
    {
      if (!name)
        return;
        
      creators.erase (name);
    }
    
///������ ����������� ���� ������������
    void UnregisterAll ()
    {
      creators.clear ();
    }

///��������������� �� ����������
    bool IsPresent (const char* name)
    {
      if (!name)
        return false;
        
      return creators.find (name) != creators.end ();
    }
    
///�������� �����������
    IController* CreateController (Node& node, const char* name, const xtl::any_reference& param)
    {
      try
      {                
          //�������� ������������ ����������

        if (!name)
          throw xtl::make_null_argument_exception ("", "name");
          
          //�������� ����������� �� ���������
          
        static common::ComponentLoader loader (DEFAULT_COMPONENTS_MASK);

          //����� �������� �������� �����������

        CreatorMap::iterator iter = creators.find (name);

        if (iter == creators.end ())
          throw xtl::make_argument_exception ("", "name", name, "Controller has not registered");

           //�������� �����������

        return iter->second (node, param);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("scene_graph::ControllerManagerImpl::CreateController");
        throw;
      }
    }    

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, ControllerCreator> CreatorMap;

  private:
    CreatorMap creators; //����� ������������ �������� ������������
};

typedef common::Singleton<ControllerManagerImpl> ControllerManagerSingleton;

}

/*
    ������ ��� �������� � ControllerManagerImpl
*/

void ControllerManager::Register (const char* name, const ControllerCreator& creator)
{
  ControllerManagerSingleton::Instance ().Register (name, creator);
}

void ControllerManager::Unregister (const char* name)
{
  ControllerManagerSingleton::Instance ().Unregister (name);
}

void ControllerManager::UnregisterAll ()
{
  ControllerManagerSingleton::Instance ().UnregisterAll ();
}

bool ControllerManager::IsPresent (const char* name)
{
  return ControllerManagerSingleton::Instance ().IsPresent (name);
}

namespace scene_graph
{

IController* create_controller (Node& node, const char* name, const xtl::any_reference& param)
{
  return ControllerManagerSingleton::Instance ().CreateController (node, name, param);
}

}
