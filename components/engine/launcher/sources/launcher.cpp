#include <cstdio>
#include <exception>

#include <stl/string>

#include <xtl/string.h>

#include <common/console.h>

#include <syslib/application.h>

#include <engine/subsystem_manager.h>

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
  "Usage: app_name [args]\n",
  "  args:\n",
  "    --config=config_file_name - launch with specified config file\n",
  "    --version (-v)            - print version\n",
  "    --no-main-loop            - app exits after starting subsystems\n",
  "    --main-loop               - app runs main loop after starting subsystems\n",
  "    --help                    - print this help\n",
};


/*
    ����������
*/

class Application
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
    void ParseCommandLine (size_t args_count, char* args [])
    {
      for (size_t i=0; i<args_count; i++)
      {
        const char* argument = args [i];

        if (!argument)
          continue;

        if (!xtl::xstrcmp (argument, KEY_CONFIGURATION))
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
        }
      }
    }

///���������� ����������
    void Run ()
    {
      manager.Start (configuration_name.c_str ());

      if (need_print_version)
        common::Console::Printf ("Application version: %s\n", VERSION);

      if (need_print_help)
        for (size_t i = 0, help_strings = sizeof (HELP) / sizeof (HELP[0]); i < help_strings; i++)
          common::Console::Print (HELP[i]);

        //������ ��������� ����� ����������

      if (has_main_loop)
      {
        syslib::Application::Run ();
      }
    }

  private:
    SubsystemManager manager;                    //�������� ���������
    bool             has_main_loop;              //���� �� ������� ���� ����������
    bool             has_explicit_configuration; //������������ ���������� ������� ����
    stl::string      configuration_name;         //��� ������������
    bool             need_print_version;         //����� ����������� ������ ������
    bool             need_print_help;            //����� ����������� ������ �� ������� ����������
};

}

//����� �����
int main (int argc, char* argv [])
{
  try
  {
    Application application;

    application.ParseCommandLine ((size_t)argc, argv);

    application.Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }
  catch (...)
  {
    printf ("unknown exception\n");
  }

  return syslib::Application::GetExitCode ();
}
