#include <cstdio>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sgi_stl/memory>

#include <launcher/application.h>

using namespace engine;
using namespace plarium::launcher;

//����� �����
extern "C" __attribute__ ((visibility("default"))) int main (int argc, const char* argv [], const char* env [])
{
  sgi_stl::auto_ptr<IEngine> funner (FunnerInit ());

  if (!funner.get ())
  {
    printf ("Funner startup failed!");
    return 1;
  }

  char app_dir_env_var [512];

  strcpy (app_dir_env_var, "APP_DIR=");

  char* app_dir_name = app_dir_env_var + strlen (app_dir_env_var);

  size_t app_dir_name_size = sizeof (app_dir_env_var) - strlen (app_dir_env_var) - 1;

  if (!getcwd (app_dir_name, app_dir_name_size))
  {
    printf ("::getcwd failed\n");
    return 1;
  }

  app_dir_name [sizeof (app_dir_env_var)-1] = 0;

  putenv (app_dir_env_var);

  funner->SetBaseDir ("app/native/assets");

  if (!funner->ParseCommandLine (argc, argv, env))
    return 1;

  Application application;

  application.Run (funner.get ());

  return 0;
}
