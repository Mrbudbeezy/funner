#include "shared.h"

const char* LIBRARY_NAME = "data/particle_designer/fire big.plist";

int main ()
{
  printf ("Results of particle_designer_emitter_loader_test:\n");
  
  try
  {
    ParticleSystemLibrary library (LIBRARY_NAME);

    //TODO dump info
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }
}
