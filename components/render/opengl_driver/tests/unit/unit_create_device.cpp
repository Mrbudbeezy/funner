#include "shared.h"

int main ()
{
  printf ("Results of unit_create_device_test:\n");
  
  try
  {
    Test test;

    printf ("Device: '%s'\n", test.device->GetName ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
