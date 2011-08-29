#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::debug;

namespace
{

/*
    ���������
*/

const char* DRIVER_NAME           = "Debug";                  //��� ��������
const char* DRIVER_COMPONENT_NAME = "render.mid_level.Debug"; //��� ���������� ��������
const char* RENDERER2D_NAME       = "Renderer2d";             //��� ������� ���������� ����������

}

/*
    ����������� / ����������
*/

Driver::Driver ()
{
  log.Printf ("Create debug driver");
}

Driver::~Driver ()
{
  try
  {
    log.Printf ("Destroy debug driver");
  }
  catch (...)
  {
    //���������� ����������
  }
}

/*
    �������� ��������
*/

const char* Driver::GetDescription ()
{
  return "render::mid_level::debug::Driver";
}

/*
    ������������ ��������� ������ ������������
*/

size_t Driver::GetRenderersCount ()
{
  return 1;
}

const char* Driver::GetRendererName (size_t index)
{
  if (index)
    throw xtl::make_range_exception ("render::mid_level::debug::Driver::GetRendererName", "index", index, 1u);
    
  return RENDERER2D_NAME;
}

/*
    �������� ���������� ������������
*/

IRenderer* Driver::CreateRenderer (const char* name)
{
  static const char* METHOD_NAME = "render::mid_level::debug::Driver::CreateRenderer";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  if (xtl::xstrcmp (name, RENDERER2D_NAME))
    throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Unknown renderer");
    
  return new renderer2d::Renderer;
}

namespace components
{

namespace debug_renderer_driver
{

/*
    ��������� ����������� �������� ������������
*/

class DebugDriverComponent
{
  public:   
    DebugDriverComponent ()
    {
      DriverManager::RegisterDriver (DRIVER_NAME, xtl::com_ptr<Driver> (new Driver, false).get ());
    }
};

extern "C" common::ComponentRegistrator<DebugDriverComponent> DebugRendererDriver (DRIVER_COMPONENT_NAME);

}

}
