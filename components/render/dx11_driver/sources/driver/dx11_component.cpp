#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

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
      DriverManager::RegisterDriver (DRIVER_NAME, xtl::com_ptr<Driver> (new Driver, false).get ());
    }
};

}

}

extern "C"
{

common::ComponentRegistrator<components::dx11_driver::Component> DX11Driver (COMPONENT_NAME);

}
