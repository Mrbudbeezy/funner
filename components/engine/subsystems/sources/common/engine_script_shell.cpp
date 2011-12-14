#include <xtl/shared_ptr.h>

#include <script/bind.h>
#include <script/environment.h>
#include <script/shell.h>

#include "shared.h"

using namespace engine;
using namespace script;
using namespace common;

namespace components
{

namespace shell_subsystem
{

/*
    ���������
*/

const char* SUBSYSTEM_NAME = "Shell";                   //��� ����������
const char* COMPONENT_NAME = "engine.subsystems.Shell"; //��� ����������

/*
    ���������� ����������� ��������������
*/

class ShellSubsystem : public ISubsystem, public xtl::reference_counter
{
  public:
/// �����������/����������
    ShellSubsystem (common::ParseNode& node, SubsystemManager& manager)
    {
        //������ ������������

      const char *interpreter         = get<const char*> (node, "Interpreter"),
                 *libraries           = get<const char*> (node, "Libraries", ""),
                 *sources             = get<const char*> (node, "Sources", ""),
                 *command             = get<const char*> (node, "Execute", ""),
                 *config_search_paths = get<const char*> (node, "SearchPaths", ""),
                 *shell_search_paths  = getenv ("LUA_SEARCH_PATHS");

      stl::string search_paths = config_search_paths;

      if (shell_search_paths)
      {
        if (!search_paths.empty ())
          search_paths.push_back (';');

        search_paths += shell_search_paths;
      }

      StringArray lib_list = split (libraries),
                  src_list = split (sources);                  

        //�������� ���������

      for (size_t i=0; i<lib_list.Size (); i++)
        environment.BindLibraries (lib_list [i]);

        //����������� ��������� ���������

      InvokerRegistry engine_lib = environment.Library ("Engine");
      InvokerRegistry global_lib = environment.Library ("global");

      engine_lib.Register ("get_SubsystemManager", make_const (xtl::ref (manager)));
      global_lib.Register ("searchpaths", make_const (search_paths));

        //�������� ��������������

      Shell (interpreter, environment).Swap (shell);

        //�������� � ���������� �������� ������

      for (size_t i=0; i<src_list.Size (); i++)
        shell.ExecuteFileList (src_list [i]);

        //���������� �������

      if (*command)
        shell.Execute (command);
    }
    
///���������� �������
    void Execute (const char* command)
    {
      if (!wcimatch (command, "*.lua") && !wcimatch (command, "*.luac") && !wcimatch (command, "lua:*"))
        return;                
        
      try
      {
        if (wcimatch (command, "lua:*"))
        {
          static const char* PREFIX_STRING = "lua:";
          static size_t      PREFIX_LENGTH = strlen (PREFIX_STRING);

          command += PREFIX_LENGTH;
          
          shell.Execute (command);
        }
        else
        {
          shell.ExecuteFile (command);          
        }
      }
      catch (xtl::exception& e)
      { 
        e.touch ("engine::ShellSubsystem::Execute");
        throw;
      }
    }

/// ������� ������
    void AddRef ()  { addref (this); }
    void Release () { release (this); }

  private:
    Environment environment; //���������� ���������
    Shell       shell;       //���������� �������������
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
        xtl::com_ptr<ShellSubsystem> subsystem (new ShellSubsystem (node, manager), false);

        manager.Add (SUBSYSTEM_NAME, subsystem.get ());
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

}
