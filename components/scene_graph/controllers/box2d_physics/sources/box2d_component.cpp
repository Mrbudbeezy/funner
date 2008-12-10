#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::physics;
using namespace scene_graph::physics::box2d;

namespace
{

/*
    ���������
*/

const char* COMPONENT_NAME  = "scene_graph.controllers.box2d_physics"; //��� ����������
const char* CONTROLLER_NAME = "Box2D.World";                           //��� ����������� ����������� ����

/*
    ����������� ����������
*/

class Component
{
  public:
    Component ()
    {
      ControllerManager::Register (CONTROLLER_NAME, &CreateWorldController);
    }
    
  private:
    static IController* CreateWorldController (Node& node, const xtl::any_reference& param)
    {
      try
      {
        ControllerDesc* desc = param.content<ControllerDesc> ();
        
          //�������� ������������ ����������
          
        if (!desc)
          throw xtl::make_argument_exception ("", "param", param.type ().name (), "Expected scene_graph::physics::ControllerDesc parameter");
          
        if (!desc->configuration_name)
          throw xtl::make_null_argument_exception ("", "param.configuration_name");

          //�������� ����������� ����������� ����

        return new WorldController (node, desc->world_bound_box, ConfigurationManager::GetConfiguration (desc->configuration_name));
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("scene_graph::box2d::Component::CreateWorldController");
        throw;
      }
    }
};

extern "C"
{

common::ComponentRegistrator<Component> Box2DController (COMPONENT_NAME);

}

}
