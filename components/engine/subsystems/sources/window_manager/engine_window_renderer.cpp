#include <stl/string>

#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/component.h>
#include <common/parser.h>

#include <render/mid_level/window_driver.h>

#include <engine/attachments.h>
#include <engine/subsystem_manager.h>

using namespace engine;
using namespace common;

namespace
{

/*
    ���������
*/

const char* COMPONENT_NAME = "engine.subsystems.WindowRenderer"; //��� ����������
const char* SUBSYSTEM_NAME = "WindowRenderer";                   //��� ����������

/*
    ���������� ������������� ������� ������� ����������
*/

class WindowRendererSubsystem: public ISubsystem, public xtl::reference_counter
{
  public:
///�����������
    WindowRendererSubsystem (ParseNode& node)
    {
      renderer_name = get<const char*> (node, "Name");

      render::mid_level::WindowDriver::RegisterRenderer (renderer_name.c_str (), node);      

      for (Parser::NamesakeIterator iter=node.First ("FrameBuffer"); iter; ++iter)
      {
        const char*     attachment = get<const char*> (*iter, "Window");
        syslib::Window& window     = AttachmentRegistry::Get<syslib::Window> (attachment);

        render::mid_level::WindowDriver::RegisterWindow (renderer_name.c_str (), window, *iter);
      }
    }

///����������
    ~WindowRendererSubsystem ()
    {
      render::mid_level::WindowDriver::UnregisterAllWindows (renderer_name.c_str ());
    }
    
///������� ������
    void AddRef () { addref (this); }
    void Release () { release (this); }
    
  private:
    stl::string renderer_name; //��� ������� ����������
};

/*
   ��������� ����������� ������� ������� ����������
*/

class WindowRendererComponent
{
  public:
    WindowRendererComponent () 
    {
      StartupManager::RegisterStartupHandler (SUBSYSTEM_NAME, &StartupHandler);
    }
    
  private:
    static void StartupHandler (ParseNode& node, SubsystemManager& manager)
    {
      try
      {        
        xtl::com_ptr<ISubsystem> subsystem (new WindowRendererSubsystem (node), false);

        manager.AddSubsystem (subsystem.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::WindowRendererComponent::StartupHandler");
        throw;
      }      
    }
};

extern "C"
{

ComponentRegistrator<WindowRendererComponent> WindowRenderer (COMPONENT_NAME);

}

}
