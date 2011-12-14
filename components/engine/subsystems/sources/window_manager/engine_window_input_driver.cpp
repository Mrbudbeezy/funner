#include <stl/list>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/component.h>
#include <common/parser.h>
#include <common/strlib.h>

#include <syslib/window.h>

#include <input/low_level/window_driver.h>

#include <engine/attachments.h>
#include <engine/subsystem_manager.h>

using namespace engine;
using namespace common;

namespace components
{

namespace window_input_driver_subsystem
{

/*
    ���������
*/

const char* SUBSYSTEM_NAME = "WindowInputDriver";                   //��� ����������
const char* COMPONENT_NAME = "engine.subsystems.WindowInputDriver"; //��� ����������

/*
    ���������� ����� ����
*/

class WindowDevice: public IAttachmentRegistryListener<syslib::Window>, public xtl::reference_counter
{
  public:
///�����������
    WindowDevice (ParseNode& node)
      : attachment_name (get<const char*> (node, "Window")),
        device_name (get<const char*> (node, "Id", ""))
    {
      if (device_name.empty ())
      {
        static size_t driver_index = 1;

        device_name = format ("WindowInputSubsystem%u", driver_index++);
      }

      AttachmentRegistry::Attach (this, AttachmentRegistryAttachMode_ForceNotify);
    }

///����������
    ~WindowDevice ()
    {
      AttachmentRegistry::Detach (this, AttachmentRegistryAttachMode_ForceNotify);
    }

///���������� ������� ����������� ����
    void OnRegisterAttachment (const char* name, syslib::Window& window)
    {
      if (name != attachment_name)
        return;

      input::low_level::WindowDriver::UnregisterDevice (device_name.c_str ());
      input::low_level::WindowDriver::RegisterDevice (device_name.c_str (), window);
    }

///���������� ������� ������ ����������� ����
    void OnUnregisterAttachment (const char* name, syslib::Window&)
    {
      if (name != attachment_name)
        return;

      input::low_level::WindowDriver::UnregisterDevice (device_name.c_str ());
    }

  private:
    stl::string attachment_name;
    stl::string device_name;
};

/*
    ���������� direct input ��������
*/

class WindowInputDriverSubsystem : public ISubsystem, public xtl::reference_counter
{
  public:
///�����������
    WindowInputDriverSubsystem (ParseNode& node)
    {
      for (Parser::NamesakeIterator iter=node.First ("Device"); iter; ++iter)
        devices.push_back (DevicePtr (new WindowDevice (*iter), false));
    }

///������� ������
    void AddRef ()  { addref (this); }
    void Release () { release (this); }

  private:
    WindowInputDriverSubsystem (const WindowInputDriverSubsystem&);             //no impl
    WindowInputDriverSubsystem& operator = (const WindowInputDriverSubsystem&); //no impl

  private:
    typedef xtl::intrusive_ptr<WindowDevice> DevicePtr;
    typedef stl::list<DevicePtr>             DeviceList;

  private:
    DeviceList devices;
};

/*
    ��������� �������� �����
*/

class WindowInputDriverComponent
{
  public:
    WindowInputDriverComponent ()
    {
      StartupManager::RegisterStartupHandler (SUBSYSTEM_NAME, &StartupHandler);
    }

  private:
    static void StartupHandler (ParseNode& node, SubsystemManager& manager)
    {
      try
      {
        xtl::com_ptr<ISubsystem> subsystem (new WindowInputDriverSubsystem (node), false);

        manager.Add (SUBSYSTEM_NAME, subsystem.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::WindowInputDriverComponent::StartupHandler");
        throw;
      }
    }
};

extern "C"
{

ComponentRegistrator<WindowInputDriverComponent> WindowInputDriverSubsystem (COMPONENT_NAME);

}

}

}
