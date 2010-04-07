#include <cstdio>
#include <exception>

#include <stl/auto_ptr.h>
#include <stl/string>

#include <xtl/bind.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/string.h>

#include <common/console.h>
#include <common/string.h>

#include <syslib/application.h>

#include <engine/subsystem_manager.h>

#include <engine/launcher.h>

using namespace engine;

namespace
{

#ifndef LAUNCHER_VERSION
  #define LAUNCHER_VERSION "unknown version"
#endif

/*
    ���������
*/

const char* VERSION                         = LAUNCHER_VERSION; //������ ������
const char* DEFAULT_CONFIGURATION_FILE_NAME = "config.xml";       //��� ����������������� ����� �� ���������
const bool  DEFAULT_HAS_MAIN_LOOP           = true;               //������� �������� ����� ���������� �� ���������

const char* KEY_CONFIGURATION = "--config=";      //��� ����� ����������������� �����
const char* KEY_NO_MAIN_LOOP  = "--no-main-loop"; //���������� �������� ����� ����������
const char* KEY_MAIN_LOOP     = "--main-loop";    //��������� �������� ����� ����������
const char* KEY_VERSION       = "--version";      //����� ������� � ����������� ����� ������
const char* KEY_VERSION_SHORT = "-v";             //����� ������� � ����������� ����� ������
const char* KEY_HELP          = "--help";         //����� ������� � ����������� ����� ������

const char* HELP [] = {
  "Usage: app_name [flags] [source] ...\n",
  "  flags:\n",
  "    --config=config_file_name - launch with specified config file\n",
  "    --version (-v)            - print version\n",
  "    --no-main-loop            - app exits after starting subsystems\n",
  "    --main-loop               - app runs main loop after starting subsystems\n",
  "    --help                    - print this help\n",
};

const size_t STARTUP_MAIN_LOOP_DELAY = 10; //�������� ����� �������� ������� ���� ���������� (� ������������) - ���������� ��� ������ �� iPhone

/*
    ����������
*/

class Application: public IFunnerApi
{
  public:
///�����������
    Application ()
    {
      has_main_loop      = DEFAULT_HAS_MAIN_LOOP;
      configuration_name = DEFAULT_CONFIGURATION_FILE_NAME;
      need_print_version = false;
      need_print_help    = false;
    }
        
///������ ���������� �������� ������
    bool ParseCommandLine (unsigned int args_count, const char** args)
    {
      try
      {
        commands.Reserve (args_count);
        
        for (size_t i=1; i<args_count; i++)
        {
          const char* argument = args [i];

          if (!argument)
            continue;

          if (!xtl::xstrncmp (argument, KEY_CONFIGURATION, xtl::xstrlen (KEY_CONFIGURATION)))
          {
            configuration_name = argument + xtl::xstrlen (KEY_CONFIGURATION);
          }
          else if (!xtl::xstrcmp (argument, KEY_MAIN_LOOP))
          {
            has_main_loop = true;
          }
          else if (!xtl::xstrcmp (argument, KEY_NO_MAIN_LOOP))
          {
            has_main_loop = false;
          }
          else if (!xtl::xstrcmp (argument, KEY_VERSION) || !xtl::xstrcmp (argument, KEY_VERSION_SHORT))
          {
            need_print_version = true;
          }
          else if (!xtl::xstrcmp (argument, KEY_HELP))
          {
            need_print_help = true;
            has_main_loop   = false;
          }
          else
          {
            commands.Add (argument);
          }
        }
        
        return true;
      }
      catch (std::exception& exception)
      {
        printf ("exception: %s\n", exception.what ());
      }
      catch (...)
      {
        printf ("unknown exception at Application::ParseCommandLineEntry\n");
      }
      
      return false;
    }

///���������� ����������
    void Run ()
    {
      try
      {
          //����������� ����������� ������ ����������

        syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnInitialized, xtl::bind (&Application::StartupHandler, this));

          //������ ��������� �����
  
        syslib::Application::Run ();
      }
      catch (std::exception& exception)
      {
        printf ("exception: %s\n", exception.what ());
      }
      catch (...)
      {
        printf ("unknown exception at Application::RunEntry\n");
      }      
    }

  private:
///���������� ������ ����������
    void StartupHandler ()
    {
      try
      {      
          //������ ���������

        if (need_print_version)
          common::Console::Printf ("Application version: %s\n", VERSION);

        if (need_print_help)
          for (size_t i = 0, help_strings = sizeof (HELP) / sizeof (HELP[0]); i < help_strings; i++)
            common::Console::Print (HELP [i]);

          //���� ��������� ����� ��� - ����� �� ����������

        if (!has_main_loop)
          syslib::Application::Exit (0);            

        if (!need_print_help && !need_print_version)
        {
          try
          {
            manager.Start (configuration_name.c_str ());
            
            for (size_t i=0, count=commands.Size (); i<count; i++)
            {
              const char* command = commands [i];

              manager.Execute (command);
            }
          }
          catch (...)
          {
            syslib::Application::Exit (-1);
            throw;
          }
        }
      }
      catch (std::exception& exception)
      {
        printf ("exception: %s\n", exception.what ());
      }
      catch (...)
      {
        printf ("unknown exception at engine::Application::StartupTimerHandler\n");
      }      
    }

  private:
    SubsystemManager    manager;                    //�������� ���������
    bool                has_main_loop;              //���� �� ������� ���� ����������
    bool                has_explicit_configuration; //������������ ���������� ������� ����
    stl::string         configuration_name;         //��� ������������
    bool                need_print_version;         //����� ����������� ������ ������
    bool                need_print_help;            //����� ����������� ������ �� ������� ����������
    common::StringArray commands;                   //������� �� ���������� ������������
};

}

/*
    ������ ��� ����������� � ����������
*/

extern "C" int MAKE_TARGET_LINK_INCLUDES_COMMA; 

static int touch [] = {MAKE_TARGET_LINK_INCLUDES_COMMA};

FUNNER_C_API IFunnerApi* FunnerInit ()
{
  try
  {    
    return new Application;
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  catch (...)
  {
    printf ("unknown exception at FunnerInit\n");
  }

  return 0;
}
