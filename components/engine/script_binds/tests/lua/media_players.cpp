#include "shared.h"

const char* SCRIPT_FILE_NAME = "data/media_players.lua";

void console_handler (const char* message)
{
  printf ("%s", message);
}

int main ()
{
  common::Console::RegisterEventHandler (common::ConsoleEvent_OnPrintLine, &console_handler);

  printf ("Results of media_players_test:\n");

  try
  {
    Environment env;

    Shell shell ("lua", env);

    xtl::com_ptr<IInterpreter> script (shell.Interpreter ());

    env.BindLibraries ("MediaPlayers");

    load_script (*script, SCRIPT_FILE_NAME);

    invoke<void> (*script, "test");
  }
  catch (xtl::bad_any_cast& exception)
  {
    printf ("%s: %s -> %s\n", exception.what (), exception.source_type ().name (), exception.target_type ().name ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
