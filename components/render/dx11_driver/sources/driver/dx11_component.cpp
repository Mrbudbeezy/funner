#include "shared.h"

namespace
{

/*
    ���������
*/

const char*  DRIVER_NAME    = "DX11";                  //��� ��������
const char*  COMPONENT_NAME = "render.low_level.dx11"; //��� ����������

}

/*
    ��������� ��������
*/

namespace components
{

namespace dx11_driver
{

class Component
{
  public:
    Component ()
    {
      throw xtl::make_not_implemented_exception ("components::dx11_driver::Component::Component");
//      DriverManager::RegisterDriver (DRIVER_NAME, xtl::com_ptr<Driver> (new Driver, false).get ());
    }
};

}

}

extern "C"
{

common::ComponentRegistrator<components::dx11_driver::Component> DX11Driver (COMPONENT_NAME);

}
