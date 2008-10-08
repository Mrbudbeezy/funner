#include <stl/vector>

#include <xtl/any.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/component.h>

#include <input/low_level/driver.h>

#include <client/engine.h>

using namespace client;
using namespace common;

namespace
{

const char* SUBSYSTEM_NAME = "DirectInputDriver";

/*
   ���������� direct input ��������               
*/

class DirectInputDriverSubsystem : public IEngineSubsystem, public xtl::reference_counter
{
  public:
/// �����������/����������
    DirectInputDriverSubsystem ()
    {
      input::low_level::DriverManager::FindDriver ("DirectInput8");
    }
    
/// ��������� �����
    const char* Name () { return SUBSYSTEM_NAME; }

/// ������� ������
    void AddRef ()
    {
      addref (this);
    }

    void Release ()
    {
      release (this);
    }

  private:
    DirectInputDriverSubsystem (const DirectInputDriverSubsystem&);             //no impl
    DirectInputDriverSubsystem& operator = (const DirectInputDriverSubsystem&); //no impl
};

/*
   �������� ��������
*/

void direct_input_driver_startup (common::VarRegistry& var_registry, IEngineStartupParams*, Engine& engine)
{
  try
  {
    if (!var_registry.HasVariable ("Load"))
      throw xtl::format_operation_exception ("", "There is no 'Load' variable in the configuration registry branch '%s'", var_registry.BranchName ());

    if (!atoi (var_registry.GetValue ("Load").cast<stl::string> ().c_str ()))
      return;

    xtl::com_ptr<IEngineSubsystem> new_subsystem (new DirectInputDriverSubsystem (), false);

    engine.AddSubsystem (new_subsystem.get ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("window_renderer_startup");
    throw;
  }
}

/*
   ��������� direct input �����
*/

class DirectInputDriverComponent
{
  public:
    //�������� ����������
    DirectInputDriverComponent () 
    {
      StartupManager::RegisterStartupHandler ("DirectInputDriver", &direct_input_driver_startup, StartupGroup_Level1);
    }
};

extern "C"
{

ComponentRegistrator<DirectInputDriverComponent> DirectInputDriver ("client.subsystems.direct_input_driver");

}

}
