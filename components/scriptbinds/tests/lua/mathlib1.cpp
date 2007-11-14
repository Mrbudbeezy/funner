#include "shared.h"

const char* SCRIPT_FILE_NAME = "data/mathlib.lua";

int main ()
{
  printf ("Results of mathlib1_test:\n");
  
  try
  {
    xtl::shared_ptr<Environment> env    (new Environment);
    xtl::com_ptr<IInterpreter>   script (create_lua_interpreter (env));
  
    bind_math_library (*env);
    load_script       (*script, SCRIPT_FILE_NAME);
    
    printf ("Test vec3f library:\n");

    invoke<void> (*script, "test", vec3f (1, 2, 3));
    
    printf ("Test mat4f library:\n");
    
    invoke<void> (*script, "test_matrix", mat4f (1));
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
