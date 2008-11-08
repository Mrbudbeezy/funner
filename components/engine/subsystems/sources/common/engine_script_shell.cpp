#include <xtl/bind.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/shared_ptr.h>

#include <common/log.h>
#include <common/strlib.h>

#include <script/environment.h>
#include <script/shell.h>

#include "shared.h"

using namespace engine;
using namespace script;
using namespace common;

namespace
{

/*
    ���������
*/

const char* SUBSYSTEM_NAME = "Shell";                   //��� ����������
const char* COMPONENT_NAME = "engine.subsystems.Shell"; //��� ����������
const char* LOG_NAME       = COMPONENT_NAME;            //��� ������ ����������������

/*
    ���������� ����������� ��������������
*/

class ShellSubsystem : public ISubsystem, public xtl::reference_counter
{
  public:
/// �����������/����������
    ShellSubsystem (common::ParseNode& node)
      : log (COMPONENT_NAME),
        environment (new Environment)
    {
        //������ ������������
      
      const char *interpreter = get<const char*> (node, "Interpreter"),
                 *libraries   = get<const char*> (node, "Libraries", ""),
                 *sources     = get<const char*> (node, "Sources"),
                 *command     = get<const char*> (node, "Execute", ""),
                 *log_name    = get<const char*> (node, "Log", "");

      StringArray lib_list = split (libraries),
                  src_list = split (sources);

        //��������� ������ ����������������

      if (*log_name)
        Log (log_name).Swap (log);
        
        //�������� ���������
        
      for (size_t i=0; i<lib_list.Size (); i++)
        environment->BindLibraries (lib_list [i]);
        
        //�������� ��������������
        
      Shell (interpreter, environment).Swap (shell);

        //�������� � ���������� �������� ������

      Shell::LogFunction log_handler = xtl::bind (&Log::Print, &log, _1);

      for (size_t i=0; i<src_list.Size (); i++)
        shell.ExecuteFileList (src_list [i], log_handler);
        
        //���������� �������
        
      if (*command)
        shell.Execute (command, log_handler);
    }

/// ������� ������
    void AddRef ()  { addref (this); }
    void Release () { release (this); }
    
  private:
    typedef Shell::EnvironmentPtr EnvironmentPtr;

  private:
    Log            log;         //�������� ����������
    EnvironmentPtr environment; //���������� ���������
    Shell          shell;       //���������� �������������
};

/*
    ��������� �����������
*/

class Component
{
  public:
    Component () 
    {
      StartupManager::RegisterStartupHandler (SUBSYSTEM_NAME, &StartupHandler);
    }

  private:
    static void StartupHandler (ParseNode& node, SubsystemManager& manager)
    {
      try
      {
        xtl::com_ptr<ShellSubsystem> subsystem (new ShellSubsystem (node), false);

        manager.AddSubsystem (subsystem.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::ShellComponent::StartupHandler");
        throw;
      }      
    }
};

extern "C"
{

ComponentRegistrator<Component> ShellSubsystem (COMPONENT_NAME);

}

}
