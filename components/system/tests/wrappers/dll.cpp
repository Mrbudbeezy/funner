#include "shared.h"

#if defined (WINCE)
  const char* LIBRARY_NAME = "coredll.dll";
  const char* SYMBOL_NAME  = "LoadLibraryW";
#elif defined (_WIN32)
  const char* LIBRARY_NAME = "kernel32.dll";
  const char* SYMBOL_NAME  = "LoadLibraryA";
#elif defined (__APPLE__)
  const char* LIBRARY_NAME = "libc.dylib";
  const char* SYMBOL_NAME  = "printf";
#elif defined (__linux__)
  const char* LIBRARY_NAME = "libdl.so";
  const char* SYMBOL_NAME  = "dlopen";
#elif defined (BADA)
  const char* LIBRARY_NAME = "libdl.so";
  const char* SYMBOL_NAME  = "dlopen";
#elif defined (ANDROID)
  const char* LIBRARY_NAME = "libdl.so";
  const char* SYMBOL_NAME  = "dlopen";  
#elif defined (TABLETOS)
  const char* LIBRARY_NAME = "libEGL.so";
  const char* SYMBOL_NAME  = "eglWaitClient";
#else
  #error Unknown platform
#endif

int main ()
{
  printf ("Results of dll_test:\n");

  try
  {
    DynamicLibrary library;

    library.Load (LIBRARY_NAME);

    printf ("Symbol %s\n", library.GetSymbol (SYMBOL_NAME) ? "found" : "not found");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
